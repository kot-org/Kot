#include <controller/controller.h>

void HDAControllerOnInterrupt(){
    HDAController* Controller = (HDAController*)Sys_GetExternalDataThread();

    for(uint8_t i = 0; i < Controller->StreamCount; i++){
        if(Controller->Registers->InterruptStatus & (1 << i)){
            if(Controller->Registers->Streams[i].Status & HDA_STREAM_STS_BCIS){
                Controller->Outputs[i]->Stream->CurrentPosition = Controller->Registers->Streams[i].CurrentBufferLinkPosition;
                if(Controller->Outputs[i]->Stream->CurrentPosition % Controller->Outputs[i]->Stream->SizeIOCToTrigger){
                    Controller->Outputs[i]->Stream->CurrentPosition -= Controller->Outputs[i]->Stream->CurrentPosition % Controller->Outputs[i]->Stream->SizeIOCToTrigger;
                    Controller->Outputs[i]->Stream->CurrentPosition += Controller->Outputs[i]->Stream->SizeIOCToTrigger;
                }
                Controller->Registers->Streams[i].Status |= HDA_STREAM_STS_BCIS; // Clear
                *(uint64_t*)((uint64_t)Controller->Outputs[i]->Stream->Buffer + Controller->Outputs[i]->Stream->PositionOfStreamData) = Controller->Outputs[i]->Stream->CurrentPosition; // Set the offset
                kot_arguments_t Parameters{
                    .arg[0] = Controller->Outputs[i]->Stream->CurrentPosition,
                };
                Sys_Event_Trigger(Controller->Outputs[i]->OffsetUpdateEvent, &Parameters);
            }
            Controller->Registers->InterruptStatus |= (1 << i);
        }
    }

    if(Controller->Registers->RIRBStatus & HDA_RIRB_OVERRUN){
        Controller->Registers->RIRBStatus |= HDA_RIRB_OVERRUN;
    }

    // Interrupt flags
    Controller->Registers->RIRBStatus |= HDA_RIRB_RESPONSEINTERRUPT;

    Sys_Event_Close();
}

HDAController::HDAController(PCIDeviceID_t DeviceID){
    // Clear lock
    Lock = NULL;

    PCIEnableMemorySpace(DeviceID);
    PCIEnableBusMastering(DeviceID);
    PCIEnableInterrupts(DeviceID);

    srv_pci_callback_t* CallbackPCI = Srv_Pci_GetBAR(DeviceID, 0x0, true);
    srv_pci_bar_info_t* BarInfo = (srv_pci_bar_info_t*)CallbackPCI->Data;
    free(CallbackPCI);

    kot_Sys_CreateThread(Sys_GetProcess(), (void*)&HDAControllerOnInterrupt, PriviledgeDriver, (uint64_t)this, &InterruptThread);

    srv_system_callback_t* CallbackSys = Srv_System_BindFreeIRQ(InterruptThread, true, true);
    uint64_t Vector = CallbackSys->Data;
    free(CallbackSys);

    CallbackPCI = Srv_Pci_BindMSI(DeviceID, Vector, 0x0, 0x0, true);
    free(CallbackPCI);

    HDABaseAddress = MapPhysical(BarInfo->Address, BarInfo->Size);
    Registers = (HDAControllerRegs*)HDABaseAddress;
    // Start reset
    Registers->GlobalControl &= ~HDA_GCTL_RESET;
    // Stop reset
    Registers->GlobalControl |= HDA_GCTL_RESET;

    int64_t Timer = 200;
    while(!(Registers->GlobalControl & 0x1) && Timer--){
        Sleep(1);
    }
    if(Timer < 0){
        std::printf("[AUDIO/HDA] Timed out waiting for controller");
        return;
    }


    // Disable interrupt
    Registers->InterruptControl = 0;

    // Stop DMA CORB, RIRB
    Registers->CORBControl &= ~HDA_RIRB_AND_CORB_DMA;
    Registers->RIRBControl &= ~HDA_RIRB_AND_CORB_DMA;

    Timer = 200;
    while (((Registers->CORBControl | Registers->RIRBControl) & HDA_RIRB_AND_CORB_DMA) && Timer--)
        Sleep(1);
    if (Timer < 0) {
        std::printf("[AUDIO/HDA] Timed out waiting for controller");
        return;
    }

    Supports64Bit = HDA_GCAP_64BIT(Registers->GlobalCapabilities);
    NumInputStreams = HDA_GCAP_INPUT_STREAMS(Registers->GlobalCapabilities);
    NumOutputStreams = HDA_GCAP_OUTPUT_STREAMS(Registers->GlobalCapabilities);
    NumBidirectionalStreams = HDA_GCAP_BIDIRECTIONAL_STREAMS(Registers->GlobalCapabilities);

    Registers->DMAPositionBuffer = 0;

    /* Setup CORB */
    if(SetupCORB() != KSUCCESS){
        std::printf("[AUDIO/HDA] Error while setting the CORB");
        return;
    }

    /* Setup RIRB */
    if(SetupRIRB() != KSUCCESS){
        std::printf("[AUDIO/HDA] Error while setting the RIRB");
        return;
    }


    // Disable wake interrupts
    Registers->WakeEnable = (Registers->WakeEnable & ~0x7f); 

    // Enable interrupts and unsolicited repsonses
    Registers->GlobalControl |= HDA_GCTL_UNSOLICITED_RESPONSES;
    Registers->InterruptControl = HDA_INTCTL_GLOBAL_INTERRUPT | HDA_INTCTL_CONTROLLER_INTERRUPT;
    Registers->ResponseInterruptCount = 1; // we have suprising behavior

    uint32_t Status = Registers->WakeStatus;

    if(!Status){
        return;
    }
    StreamCount = NumInputStreams + NumOutputStreams + NumBidirectionalStreams;
    for(uint8_t i = 0; i < HDA_MAX_CODECS; i++){
        if(Status & (1 << i)){
            DetectCodec(i);
        }
    }
}

HDAController::~HDAController(){

}

KResult HDAController::ConfigureStreamFormat(HDAOutput* Output){
    uint64_t Response;
    uint16_t VerbFormat = 0; // See section 3.7.1 Stream Format Structure
    VerbFormat &= ~(1 << 15); // PCM
    VerbFormat |= (Output->Format.SampleBaseRate & 0b1) << 14;
    VerbFormat |= (Output->Format.SampleBaseRateMultiplier & 0b111) << 11;
    VerbFormat |= (Output->Format.SampleBaseRateDivisor & 0b111) << 8;
    VerbFormat |= (Output->Format.BitsPerSample & 0b111) << 4;
    VerbFormat |= (Output->Format.NumberOfChannels - 1) & 0b1111;
    uint16_t StreamFormat = 0; // See section 3.3.41 Stream Descriptor Format
    StreamFormat |= (Output->Format.SampleBaseRate & 0b1) << 14;
    StreamFormat |= (Output->Format.SampleBaseRateMultiplier & 0b111) << 11;
    StreamFormat |= (Output->Format.SampleBaseRateDivisor & 0b111) << 8;
    StreamFormat |= (Output->Format.BitsPerSample  & 0b111) << 4;
    StreamFormat |= (Output->Format.NumberOfChannels - 1) & 0b1111;
    KResult Status = SendVerb(MakeVerb((HDA_VERB_SET_STREAM_FORMAT << 8) | VerbFormat, Output->Function->Node, Output->Function->Codec->Index), &Response);
    Registers->Streams[Output->Stream->StreamNumber].Format = StreamFormat;
    return KSUCCESS;
}

KResult HDAController::SetSampleRate(HDAOutput* Output, uint32_t SampleRate){
    switch(SampleRate){
        case 44100:{
            Output->Format.SampleBaseRate = 1; // 44.1 kHz
            Output->AudioDevice.Info.Format.SampleRate = 44100;
            break;    
        }
        default:{
            Output->Format.SampleBaseRate = 0; // 48 kHz
            Output->AudioDevice.Info.Format.SampleRate = 48000;
            break;
        }
    }
    return ConfigureStreamFormat(Output);
}

KResult HDAController::SetSoundEncoding(HDAOutput* Output, AudioEncoding Encoding){
    switch(Encoding){
        case AudioEncodingPCMS8LE:{
            Output->Format.BitsPerSample = 0b000; // 8 bits
            break;   
        }
        case AudioEncodingPCMS16LE:{
            Output->Format.BitsPerSample = 0b001; // 16 bits
            break; 
        }
        case AudioEncodingPCMS20LE:{
            Output->Format.BitsPerSample = 0b010; // 20 bits
            break;        
        }
        case AudioEncodingPCMS24LE:{
            Output->Format.BitsPerSample = 0b011; // 24 bits
            break; 
        }
        case AudioEncodingPCMS32LE:{
            Output->Format.BitsPerSample = 0b100; // 32 bits
            break; 
        }
        default:{
            return KFAIL;
        }
    }
    Output->AudioDevice.Info.Format.Encoding = Encoding;
    return ConfigureStreamFormat(Output);
}

KResult HDAController::SetChannel(HDAOutput* Output, uint8_t Channels){
    Output->Format.NumberOfChannels = Channels;
    Output->AudioDevice.Info.Format.NumberOfChannels = Channels;
    return ConfigureStreamFormat(Output);
}

KResult HDAController::SetVolume(HDAOutput* Output, audio_volume_t Volume){
    uint32_t Data = (1 << 13) | (1 << 12); // left and right
    if(Volume == 0){
        Data |= (1 << 7); // mute
    }else{
        Data &= ~(1 << 7); // unmute
        Data |= Volume * Output->Function->GainStepOut / 255;
    }
    uint64_t Response;
    return SendVerb(MakeVerb((HDA_VERB_SET_AMP_GAIN_MUTE << 8) | Data, Output->Function->AMPOutNode, Output->Function->Codec->Index), &Response);
}

KResult HDAController::GetNodeConfiguration(HDAWidget* Widget, NodeConfiguration* Config){
    uint64_t Response;
    KResult Status = SendVerb(MakeVerb(HDA_VERB_GET_CONFIG_DEFAULT << 8, Widget->Node, Widget->Function->Codec->Index), &Response);
    *(uint32_t*)Config = Response & 0xffffffff;
    return Status;
}

KResult HDAController::ChangeStatus(HDAOutput* Output, bool IsRunning){
    atomicAcquire(&Lock, 1);
    if(Output->IsCurrentRunning == IsRunning){
        atomicUnlock(&Lock, 1);
        return KSUCCESS;
    } 
    if(IsRunning){
        Output->Stream->Descriptor->Control |= HDA_STREAM_CTL_RUN;
    }else{
        Output->Stream->Descriptor->Control &= ~HDA_STREAM_CTL_RUN;
    }
    Output->IsCurrentRunning = IsRunning;
    atomicUnlock(&Lock, 1);
    return KSUCCESS;
}

KResult HDAController::GetOffset(HDAOutput* Output, uint64_t* Offset){
    *Offset = Output->Stream->Descriptor->CurrentBufferLinkPosition;
    return KSUCCESS;
}

void HDAController::DetectCodec(uint8_t Codec){
    Codecs[Codec].Index = Codec;
    Codecs[Codec].Functions = kot_vector_create();

    uint32_t VendorID = GetCodecParameter(Codec, 0, HDA_PARAMETER_VENDOR_ID);
    if (VendorID == 0xffffffff) {
        return;
    }

    uint32_t RevisionID = GetCodecParameter(Codec, 0, HDA_PARAMETER_REV_ID);
    uint32_t SubNodes = GetCodecParameter(Codec, 0, HDA_PARAMETER_NODE_COUNT);

    Codecs[Codec].FunctionsStart = (SubNodes >> 16) & 0xff;
    Codecs[Codec].FunctionsCount = SubNodes & 0xff;


    std::printf("[AUDIO/HDA] Codec %d, VendorID: %x, RevisionID: %x, Sub Nodes: %d", Codec, VendorID, RevisionID, SubNodes & 0xff);

    for(uint8_t Node = Codecs[Codec].FunctionsStart; Node < Codecs[Codec].FunctionsStart + Codecs[Codec].FunctionsCount; Node++){
        uint8_t FunctionType = GetCodecParameter(Codec, Node, HDA_PARAMETER_FUNCTION_TYPE) & 0xff;
        if(FunctionType != HDA_GRP_AUDIO_FUNCTION){
            continue;
        }

        HDAFunction* Function = (HDAFunction*)malloc(sizeof(HDAFunction));

        uint32_t WidgetNodeCount = GetCodecParameter(Codec, Node, HDA_PARAMETER_NODE_COUNT);
        Function->Codec = &Codecs[Codec];

        Function->WidgetsStart = (WidgetNodeCount >> 16) & 0xff;
        Function->WidgetsCount = WidgetNodeCount & 0xff;
        Function->Widgets = kot_vector_create();

        Function->Node = Node;

        // Set power to D0
        uint64_t Response;
        SendVerb(MakeVerb((HDA_VERB_SET_POWER_STATE << 8) | 0, Node, Codec), &Response);
        // Initialize widgets
        for(uint8_t Widget = Function->WidgetsStart; Widget < Function->WidgetsStart + Function->WidgetsCount; Widget++){
            WidgetInitialize(Function, Widget);
        }
        
        // Initalize function
        Function->Type = HDA_W_AUDIO_TYPE(GetCodecParameter(Codec, Node, HDA_PARAMETER_AUDIO_FG_CAP));
        if(Function->Type == HDA_W_AUDIO_OUTPUT){
            AddOutputFunction(Function);
        }
        kot_vector_push(Codecs[Codec].Functions, Function);
    }
}

void HDAController::WidgetInitialize(HDAFunction* Function, uint8_t WidgetNode){
    HDAWidget* Widget = (HDAWidget*)malloc(sizeof(HDAWidget));
    Widget->Function = Function;
    Widget->Node = WidgetNode;
    Widget->AudioCapabilities = GetCodecParameter(Function->Codec->Index, WidgetNode, HDA_PARAMETER_AUDIO_WIDGET_CAP);
    Widget->Type = HDA_W_AUDIO_TYPE(Widget->AudioCapabilities);
    uint32_t EAPDBTL = GetCodecParameter(Function->Codec->Index, WidgetNode, HDA_PARAMETER_PIN_CAP);

    switch (Widget->Type){
        case HDA_W_AUDIO_OUTPUT:{
            Function->GainStepOut = HDA_W_AUDIO_GAIN_STEP(GetCodecParameter(Function->Codec->Index, WidgetNode, HDA_PARAMETER_AMP_OUT_CAP));
            Function->AMPOutNode = WidgetNode;
            uint64_t Response;
            SendVerb(MakeVerb((HDA_VERB_SET_EAPD_BTLENABLE << 8) | EAPDBTL | 0x2, Function->Node, Function->Codec->Index), &Response);
            break;
        }            
        case HDA_WID_PIN:{
            uint32_t PinCapabilities = GetCodecParameter(Function->Codec->Index, WidgetNode, HDA_PARAMETER_PIN_CAP);
            if(PinCapabilities & HDA_PINCAP_OUT){
                uint64_t Response;
                uint32_t PinControl = SendVerb(MakeVerb((HDA_VERB_GET_PIN_WIDGET_CONTROL << 8), Function->Node, Function->Codec->Index), &Response);
                PinControl |= (1 << 6); // enable output
                SendVerb(MakeVerb((HDA_VERB_SET_PIN_WIDGET_CONTROL << 8) | PinControl, Function->Node, Function->Codec->Index), &Response);
                SendVerb(MakeVerb((HDA_VERB_SET_EAPD_BTLENABLE << 8) | EAPDBTL | 0x2, Function->Node, Function->Codec->Index), &Response);
            }
            GetNodeConfiguration(Widget, &Function->Configuration);
            break;
        }
        default:
            break;
    }

    kot_vector_push(Function->Widgets, Widget);
}

void HDAController::AddOutputFunction(HDAFunction* Function){
    HDAOutput* Output = (HDAOutput*)calloc(sizeof(HDAOutput));
    Output->ControllerParent = this;
    Output->Function = Function;
    Output->IsCurrentRunning = false;
    Output->Stream = CreateStream(Function->Codec, StreamType::Output);
    Output->SupportedPCM = GetCodecParameter(Function->Codec->Index, Function->Node, HDA_PARAMETER_PCM);
    Outputs[Output->Stream->StreamNumber] = Output;

    uint64_t Response;
    uint32_t Result = SendVerb(MakeVerb((HDA_VERB_SET_CHANNEL_STREAMID << 8) | HDA_CODEC_SET_STREAM_CHAN(Output->Stream->StreamNumber, 0), Function->Node, Function->Codec->Index), &Response);
    SetChannel(Output, 2);
    SetVolume(Output, 0);
    if(Output->SupportedPCM & HDA_SUPPCM_BITS_32){
        SetSoundEncoding(Output, AudioEncodingPCMS32LE);
    }else if(Output->SupportedPCM & HDA_SUPPCM_BITS_24){
        SetSoundEncoding(Output, AudioEncodingPCMS24LE);
    }else if(Output->SupportedPCM & HDA_SUPPCM_BITS_20){
        SetSoundEncoding(Output, AudioEncodingPCMS20LE);
    }else if(Output->SupportedPCM & HDA_SUPPCM_BITS_16){
        SetSoundEncoding(Output, AudioEncodingPCMS16LE);
    }else if(Output->SupportedPCM & HDA_SUPPCM_BITS_8){
        SetSoundEncoding(Output, AudioEncodingPCMS8LE);
    }

    SetSampleRate(Output, 48000); // according to specs : must be supported by all codecs

    Output->AudioDevice.Info.Type = AudioDeviceTypeOut;

    InitializeSrv(Output);

    Srv_Audio_AddDevice(&Output->AudioDevice, true);
}

HDAStream* HDAController::CreateStream(HDACodec* Codec, StreamType Type){
    HDAStream* Stream = (HDAStream*)calloc(sizeof(HDAStream));

    // Calculate descriptor location
    if(Type == StreamType::Input){
        Stream->StreamNumber = Codec->Index;
    }else if (Type == StreamType::Output){
        Stream->StreamNumber = NumInputStreams + Codec->Index;
    }else if (Type == StreamType::Bidirectional){
        Stream->StreamNumber = NumInputStreams + NumOutputStreams + Codec->Index;
    }

    HDAStreamDescriptor* Descriptor = &Registers->Streams[Stream->StreamNumber];

    Stream->Descriptor = Descriptor;
    Stream->BufferDescriptorListEntries = HDA_BDL_ENTRY_COUNT;
    Stream->BufferDescriptorList = (HDABufferDescriptorEntry*)GetPhysical((void**)&Stream->BufferDescriptorListPhysicalAddress, HDA_BDL_ENTRY_COUNT * sizeof(HDABufferDescriptorEntry));
    Stream->Size = Stream->BufferDescriptorListEntries * HDA_BDL_ENTRY_SIZE;
    Stream->PositionOfStreamData = Stream->Size + sizeof(uint64_t); // add the offset field

    Stream->RealSize = Stream->Size + sizeof(uint64_t); // add the offset field
    Stream->Buffer = GetFreeAlignedSpace(Stream->RealSize);
    Sys_CreateMemoryField(Sys_GetProcess(), Stream->RealSize, &Stream->Buffer, &Stream->BufferKey, MemoryFieldTypeShareSpaceRW);

    Stream->SizeIOCToTrigger = HDA_INTERRUPT_ON_COMPLETION_BDL_COUNT * HDA_BDL_ENTRY_SIZE;
    for(uint64_t i = 0; i < Stream->BufferDescriptorListEntries; i++){
        Stream->BufferDescriptorList[i].Reserved = 0; // Clear reserved
        Stream->BufferDescriptorList[i].Length = HDA_BDL_ENTRY_SIZE;
        Stream->BufferDescriptorList[i].InterruptOnCompletion = ((i % HDA_INTERRUPT_ON_COMPLETION_BDL_COUNT) == 0) ? 1 : 0;
        Stream->BufferDescriptorList[i].Address = (uint64_t)Sys_GetPhysical((void*)((uint64_t)Stream->Buffer + i * HDA_BDL_ENTRY_SIZE));
    }
    memset(Stream->Buffer, 0x0, Stream->RealSize);

    Descriptor->BufferDescriptorListPointer = (uint64_t)Stream->BufferDescriptorListPhysicalAddress;

    // Exit stream reset
    Descriptor->Control &= ~HDA_STREAM_CTL_RST;

    int64_t Timer = 200;
    while ((Descriptor->Control & HDA_STREAM_CTL_RST) && Timer--) {
        Sleep(2);
    }
    assert(Timer > 0);

    Descriptor->Control = Descriptor->Control & ~(HDA_STREAM_CTL_RST | HDA_STREAM_CTL_RUN | HDA_STREAM_CTL_STREAM_NUM_MASK | HDA_STREAM_CTL_STRIPE_MASK | HDA_STREAM_CTL_TRAFFIC_PRIORITY | HDA_STREAM_CTL_BIDIRECTIONAL_DIR);
    Descriptor->Control |= HDA_STREAM_CTL_ICOE | HDA_STREAM_CTL_FEIE | HDA_STREAM_CTL_DEIE | HDA_STREAM_CTL_STREAM_NUM(Stream->StreamNumber);
    Registers->InterruptControl |= (1 << Stream->StreamNumber);

    Descriptor->Status |= HDA_STREAM_STS_BCIS | HDA_STREAM_STS_FIFO_ERROR | HDA_STREAM_STS_DESC_ERROR;

    Descriptor->CyclicBufferSize = Stream->Size;
    Descriptor->LastValidIndex = (Descriptor->LastValidIndex & ~HDA_STREAM_DESCRIPTOR_LVI_MASK) | (Stream->BufferDescriptorListEntries - 1); // LVI must be at least 1; i.e., there must be at least two valid entries in the buffer descriptor list before DMA operations can begin

    return Stream;
}

uint32_t HDAController::GetCodecParameter(uint32_t Codec, uint32_t Node, uint32_t Parameter){
    uint64_t Response;

    KResult Status = SendVerb(MakeVerb((HDA_VERB_GET_PARAMETERS << 8) | Parameter, Node, Codec), &Response);
    if(Status != KSUCCESS){
        return 0xffffffff;
    }

    uint32_t Value = Response & 0xffffffff;

    // Check the codec of the response
    uint32_t Info = Response >> 32;

    return Value;
}

KResult HDAController::SendVerb(uint32_t Verb, uint64_t* Response){
    atomicAcquire(&Lock, 0);
    while(Registers->RIRBStatus & HDA_RIRB_RESPONSEINTERRUPT){
        Sleep(1);
    }
    uint16_t WritePointer = (Registers->CORBWritePointer + 1) % CORBEntries;
    uint16_t ReadPointer = Registers->RIRBWritePointer;

    CORB[WritePointer] = Verb;
    Registers->CORBWritePointer = WritePointer;

    int64_t Timer = 200;

    while(Registers->RIRBWritePointer == ReadPointer && Timer--){
        Sleep(1);
    }

    if(Timer < 0){
        atomicUnlock(&Lock, 0);
        return KFAIL;
    }

    *Response = RIRB[(ReadPointer + 1) % RIRBEntries];
    atomicUnlock(&Lock, 0);
    return KSUCCESS;
}

KResult HDAController::SetupCORB(){
    int64_t Timer;

    if(Registers->CORBSize & HDA_SUPPORTED_CORB_SIZE_256){
        if((Registers->CORBSize & HDA_SUPPORTED_CORB_SIZE_MASK) != HDA_SUPPORTED_CORB_SIZE_256){
            Registers->CORBSize = (Registers->CORBSize & ~0x3) | HDA_CORB_SIZE_256;
        }
        CORBEntries = 256;
    }else if(Registers->CORBSize & HDA_SUPPORTED_CORB_SIZE_16){
        if((Registers->CORBSize & HDA_SUPPORTED_CORB_SIZE_MASK) != HDA_SUPPORTED_CORB_SIZE_16){
            Registers->CORBSize = (Registers->CORBSize & ~0x3) | HDA_CORB_SIZE_16;
        }
        CORBEntries = 16;
    }else if(Registers->CORBSize & HDA_SUPPORTED_CORB_SIZE_2){
        CORBEntries = 2;
    }

    CORB = (uint32_t*)GetPhysical((void**)&Registers->CORBBaseAddress, CORBEntries * sizeof(uint32_t));
    Registers->CORBWritePointer &= ~HDA_CORB_WRITEP_MASK;
    // Reset CORB pointer : sofwate set bit to 1, hardware will clear it and set it when reset is finish
    Registers->CORBReadPointer |= HDA_CORB_READP_RESET;

    // Reset CORB pointer : Clear back reset bit
    Timer = 200;
    while(!(Registers->CORBReadPointer & HDA_CORB_READP_RESET) && Timer--){
        Sleep(1);
    }
    if(Timer < 0){
        return KFAIL;
    }

    // Clear reset bit
    Registers->CORBReadPointer &= ~HDA_CORB_READP_RESET;

    Timer = 200;
    while((Registers->CORBReadPointer & HDA_CORB_READP_RESET) && Timer--){
        Sleep(1);
    }
    if(Timer < 0){
        return KFAIL;
    }
    // Start CORB and read value back
    Registers->CORBControl |= HDA_RIRB_AND_CORB_DMA | HDA_CORB_GENERATE_ERROR_INTERRUPT;

    Timer = 200;
    while(!(Registers->CORBControl) && Timer--){
        Sleep(1);
    }
    if(Timer < 0){
        return KFAIL;
    }
    std::printf("[AUDIO/HDA] CORB setup with %d entries with value of %d", CORBEntries, Registers->CORBSize);
    return KSUCCESS;
}

KResult HDAController::SetupRIRB(){
    int64_t Timer;

    if(Registers->RIRBSize & HDA_SUPPORTED_RIRB_SIZE_256){
        if((Registers->RIRBSize & HDA_SUPPORTED_RIRB_SIZE_MASK) != HDA_SUPPORTED_RIRB_SIZE_256){
            Registers->RIRBSize = (Registers->RIRBSize & ~0x3) | HDA_RIRB_SIZE_256;
        }
        RIRBEntries = 256;
    }else if(Registers->RIRBSize & HDA_SUPPORTED_RIRB_SIZE_16){
        if((Registers->RIRBSize & HDA_SUPPORTED_RIRB_SIZE_MASK) != HDA_SUPPORTED_RIRB_SIZE_16){
            Registers->RIRBSize = (Registers->RIRBSize & ~0x3) | HDA_RIRB_SIZE_16;
        }
        RIRBEntries = 16;
    }else if(Registers->RIRBSize & HDA_SUPPORTED_RIRB_SIZE_2){
        RIRBEntries = 2;
    }

    RIRB = (uint64_t*)GetPhysical((void**)&Registers->RIRBBaseAddress, RIRBEntries * sizeof(uint64_t));

    // Start RIRB
    Registers->RIRBControl |= HDA_RIRB_AND_CORB_DMA | HDA_RIRB_GENERATE_RESPONSE_INTERRUPT;
    Sleep(10);

    std::printf("[AUDIO/HDA] RIRB setup with %d entries with value of %d", RIRBEntries, Registers->RIRBSize);

    return KSUCCESS;
}