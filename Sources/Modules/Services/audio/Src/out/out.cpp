#include <out/out.h>

void* FileBuf;
size64_t FileSize;
uint64_t FileOffset;

uint64_t FirstOffset = 0;

/// @brief This function mix audio together
/// @tparam T 
/// @param Size in byte
/// @param Dst 
/// @param Src 
template <typename T> 
void MixAudio(size64_t Size, uint64_t Offset, kot_audio_buffer_t* Dst, std::vector<StreamRequest_t*> Src){
    // TODO mix
    assert(Size <= Dst->Buffer.Size);

    size64_t SrcCount = Src.size();
    if(Src.size() >= 1){
        for(uint64_t i = 0; i < Size; i += sizeof(T)){
            *(T*)((uint64_t)Dst->Buffer.Base + ((Offset + i) % Dst->Buffer.Size)) = *(T*)((uint64_t)Src[0]->LocalBuffer->Buffer.Base  + ((Offset + i) % Src[0]->LocalBuffer->Buffer.Size)) ;
        }
        *(uint64_t*)((uint64_t)Src[0]->LocalBuffer->Buffer.Base + Src[0]->ShareBuffer->PositionOfStreamData) = (Offset + Size) % Src[0]->LocalBuffer->Buffer.Size;
    }
}

Outputs::Outputs(kot_event_t OnDeviceChangedEvent){
    NullDevice = (OutputDevice_t*)calloc(1, sizeof(OutputDevice_t));
    Devices.push(NullDevice); // default output
    OnDeviceChanged = OnDeviceChangedEvent;
    DeviceCount = 0;
    Lock = NULL;
}

void OnOffsetUpdate(){
    OutputDevice_t* OutputDevice = (OutputDevice_t*)kot_Sys_GetExternalDataThread();
    uint64_t Offset = (*(uint64_t*)((uint64_t)OutputDevice->OutputStream.Buffer.Base + OutputDevice->Device.Info.PositionOfStreamData) + (OutputDevice->Device.Info.SizeOffsetUpdateToTrigger * 2)) % OutputDevice->Device.Info.StreamSize;
    
    atomicAcquire(&OutputDevice->Lock, 0);

    if(OutputDevice->OutputStream.Format.Encoding == AudioEncodingPCMS8LE || OutputDevice->OutputStream.Format.Encoding == AudioEncodingPCMS16LE){
        MixAudio<uint16_t>(OutputDevice->Device.Info.SizeOffsetUpdateToTrigger, Offset, &OutputDevice->OutputStream, OutputDevice->InputStreams);
    }else{
        MixAudio<uint32_t>(OutputDevice->Device.Info.SizeOffsetUpdateToTrigger, Offset, &OutputDevice->OutputStream, OutputDevice->InputStreams);
    }
    atomicUnlock(&OutputDevice->Lock, 0);

    kot_arguments_t Parameters;

    kot_Sys_Event_Trigger(OutputDevice->ClientOnOffsetUpdate, &Parameters);

    kot_Sys_Event_Close();
}

KResult Outputs::AddOutputDevice(kot_srv_audio_device_t* Device){
    atomicAcquire(&Lock, 0);

    OutputDevice_t* OutputDevice = new OutputDevice_t;

    // Reset lock
    OutputDevice->Lock = NULL;

    memcpy(&OutputDevice->Device, Device, sizeof(kot_srv_audio_device_t));

    // Intialize stream buffer
    OutputDevice->OutputStream.Buffer.Base = kot_GetFreeAlignedSpace(Device->Info.StreamRealSize);
    kot_Sys_AcceptMemoryField(kot_Sys_GetProcess(), Device->StreamBufferKey, &OutputDevice->OutputStream.Buffer.Base);
    OutputDevice->OutputStream.Buffer.Size = Device->Info.StreamSize;

    OutputDevice->OutputStream.Format = Device->Info.Format;

    // Intialize stream events
    kot_Sys_CreateThread(kot_Sys_GetProcess(), (void*)&OnOffsetUpdate, PriviledgeApp, (uint64_t)OutputDevice, &OutputDevice->OnOffsetUpdateHandler);
    kot_Sys_Event_Bind(Device->OnOffsetUpdate, OutputDevice->OnOffsetUpdateHandler, false);

    kot_Sys_Event_Create(&OutputDevice->ClientOnOffsetUpdate);
    kot_Sys_Keyhole_CloneModify(OutputDevice->ClientOnOffsetUpdate, &OutputDevice->ClientOnOffsetUpdateShareableKey, NULL, KeyholeFlagPresent | KeyholeFlagDataTypeEventIsBindable, PriviledgeApp);

    // Intialize stream status
    OutputDevice->OutputStream.Volume = 255;
    SetVolume(Device, OutputDevice->OutputStream.Volume);

    OutputDevice->DeviceID = Devices.push(OutputDevice);
    DeviceCount++;
    {
        kot_arguments_t Paramters{
            .arg[0] = OutputDevice->DeviceID,
        };

        kot_Sys_Event_Trigger(OnDeviceChanged, &Paramters);
    }

    // Set default
    if(Devices[0] == NullDevice){
        Devices[0] = OutputDevice;
        Devices[0]->Device.Info.IsDefault = true;

        kot_arguments_t Paramters{
            .arg[0] = 0,
            .arg[1] = OutputDevice->DeviceID,
        };
        kot_Sys_Event_Trigger(OnDeviceChanged, &Paramters);
    }

    OutputDevice->StreamIsRunning = true;
    SetRunningState(Device, OutputDevice->StreamIsRunning);

    atomicUnlock(&Lock, 0);
    return KSUCCESS;
}

StreamRequest_t* Outputs::RequestStream(uint64_t OutputID, kot_process_t ProcessKey, uint64_t PID){
    if(OutputID >= Devices.size()){
        return NULL;
    }

    atomicAcquire(&Lock, 0);

    OutputDevice_t* OutputDevice = Devices[OutputID];
    StreamRequest_t* OutputRequestData = (StreamRequest_t*)malloc(sizeof(StreamRequest_t));
    kot_audio_share_buffer_t* ShareBuffer = (kot_audio_share_buffer_t*)malloc(sizeof(kot_audio_share_buffer_t));
    kot_audio_buffer_t* LocalBuffer = (kot_audio_buffer_t*)malloc(sizeof(kot_audio_buffer_t));

    OutputRequestData->ShareBuffer = ShareBuffer;
    OutputRequestData->LocalBuffer = LocalBuffer;
    OutputRequestData->PID = PID;
    OutputRequestData->OutputID = OutputID;
    OutputRequestData->ProcessKey = ProcessKey;
    OutputRequestData->Self = this;
    OutputRequestData->Device = OutputDevice;

    ShareBuffer->StreamSize = OutputDevice->OutputStream.Buffer.Size;
    ShareBuffer->StreamRealSize = ShareBuffer->StreamSize + sizeof(uint64_t); // Add offset field
    ShareBuffer->PositionOfStreamData = ShareBuffer->StreamSize;
    ShareBuffer->Format = OutputDevice->OutputStream.Format;
    ShareBuffer->OnOffsetUpdate = OutputDevice->ClientOnOffsetUpdateShareableKey;
    ShareBuffer->SizeOffsetUpdateToTrigger = OutputDevice->Device.Info.SizeOffsetUpdateToTrigger;

    LocalBuffer->Buffer.Size = ShareBuffer->StreamSize;
    LocalBuffer->Format = OutputDevice->OutputStream.Format;
    LocalBuffer->Volume = 255;

    LocalBuffer->Buffer.Base = kot_GetFreeAlignedSpace(ShareBuffer->StreamRealSize);
    kot_Sys_CreateMemoryField(kot_Sys_GetProcess(), ShareBuffer->StreamRealSize, &LocalBuffer->Buffer.Base, &OutputRequestData->StreamBufferLocalKey, MemoryFieldTypeShareSpaceRW);
    kot_Sys_Keyhole_CloneModify(OutputRequestData->StreamBufferLocalKey, &ShareBuffer->StreamBufferKey, ProcessKey, KeyholeFlagPresent, PriviledgeApp);
    memset(LocalBuffer->Buffer.Base, 0, LocalBuffer->Buffer.Size);

    OutputDevice->InputStreams.push(OutputRequestData);

    kot_Sys_CreateThread(kot_Sys_GetProcess(), (void*)&StreamCommand, PriviledgeApp, (uint64_t)OutputRequestData, &OutputRequestData->StreamCommandThread);
    OutputRequestData->ShareBuffer->StreamCommand = kot_MakeShareableThreadToProcess(OutputRequestData->StreamCommandThread, ProcessKey);

    atomicUnlock(&Lock, 0);

    return OutputRequestData; 
}

KResult Outputs::CloseStream(StreamRequest_t* Stream){
    atomicAcquire(&Stream->Device->Lock, 0);
    Stream->Device->InputStreams.remove(Stream->Index);
    atomicUnlock(&Stream->Device->Lock, 0);
    kot_Sys_CloseMemoryField(kot_Sys_GetProcess(), Stream->StreamBufferLocalKey, Stream->LocalBuffer->Buffer.Base);
    free(Stream->ShareBuffer);
    free(Stream->LocalBuffer);
    free(Stream);
    return KSUCCESS;
}

KResult Outputs::ChangeVolume(uint64_t OutputID, uint8_t Volume){
    if(OutputID >= Devices.size()){
        return KFAIL;
    }
    atomicAcquire(&Lock, 0);
    OutputDevice_t* OutputDevice = Devices[OutputID];
    OutputDevice->OutputStream.Volume = Volume;
    KResult Status = SetVolume(&OutputDevice->Device, OutputDevice->OutputStream.Volume);
    atomicUnlock(&Lock, 0);
    return Status;
}

KResult Outputs::SetDefault(uint64_t OutputID){
    if(OutputID >= Devices.size() && OutputID != 0){
        return KFAIL;
    }
    atomicAcquire(&Lock, 0);
    Devices[0]->Device.Info.IsDefault = false;
    Devices[0] = Devices[OutputID];
    Devices[0]->Device.Info.IsDefault = true;
    kot_arguments_t Paramters{
        .arg[0] = 0,
        .arg[1] = OutputID,
    };

    kot_Sys_Event_Trigger(OnDeviceChanged, &Paramters);
    atomicUnlock(&Lock, 0);
    return KSUCCESS;
}

uint64_t Outputs::GetDeviceCount(){
    atomicAcquire(&Lock, 0);
    uint64_t Count = DeviceCount; 
    atomicUnlock(&Lock, 0);
    return Count;
}

KResult Outputs::GetDeviceInfo(uint64_t OutputID, kot_srv_audio_device_info_t* Info){
    if(OutputID >= Devices.size()){
        return KFAIL;
    }
    atomicAcquire(&Lock, 0);

    memcpy(Info, &Devices[OutputID]->Device.Info, sizeof(kot_srv_audio_device_info_t));

    atomicUnlock(&Lock, 0);
    return KSUCCESS;    
}