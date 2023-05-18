#include <srv/srv.h>

uisd_audio_t* SrvData;

KResult InitialiseServer(){
    AddDeviceExternalData* ExternalDataAddDevice = (AddDeviceExternalData*)malloc(sizeof(AddDeviceExternalData));
    process_t proc = Sys_GetProcess();

    uintptr_t address = GetFreeAlignedSpace(sizeof(uisd_audio_t));
    ksmem_t key = NULL;
    Sys_CreateMemoryField(proc, sizeof(uisd_audio_t), &address, &key, MemoryFieldTypeShareSpaceRO);

    SrvData = (uisd_audio_t*)address;
    memset(SrvData, 0, sizeof(uisd_audio_t)); // Clear data

    SrvData->ControllerHeader.IsReadWrite = false;
    SrvData->ControllerHeader.Version = Audio_Srv_Version;
    SrvData->ControllerHeader.VendorID = Kot_VendorID;
    SrvData->ControllerHeader.Type = ControllerTypeEnum_Audio;
    SrvData->ControllerHeader.Process = ShareProcessKey(proc);

    /* OnDeviceChanged */
    event_t OnDeviceChanged;
    Sys_Event_Create(&OnDeviceChanged);
    Sys_Keyhole_CloneModify(OnDeviceChanged, &SrvData->OnDeviceChanged, NULL, KeyholeFlagPresent | KeyholeFlagDataTypeEventIsBindable, PriviledgeApp);

    ExternalDataAddDevice->OutputsClass = new Outputs(SrvData->OnDeviceChanged);

    /* RequestStream */
    thread_t RequestStreamThread = NULL;
    Sys_CreateThread(proc, (uintptr_t)&RequestStream, PriviledgeApp, (uint64_t)ExternalDataAddDevice, &RequestStreamThread);
    SrvData->RequestStream = MakeShareableThread(RequestStreamThread, PriviledgeApp);

    /* ChangeVolume */
    thread_t ChangeVolumeThread = NULL;
    Sys_CreateThread(proc, (uintptr_t)&ChangeVolume, PriviledgeApp, (uint64_t)ExternalDataAddDevice, &ChangeVolumeThread);
    SrvData->ChangeVolume = MakeShareableThread(ChangeVolumeThread, PriviledgeService);

    /* SetDefault */
    thread_t SetDefaultThread = NULL;
    Sys_CreateThread(proc, (uintptr_t)&SetDefault, PriviledgeApp, (uint64_t)ExternalDataAddDevice, &SetDefaultThread);
    SrvData->SetDefault = MakeShareableThread(SetDefaultThread, PriviledgeService);

    /* GetDeviceCount */
    thread_t GetDeviceCountThread = NULL;
    Sys_CreateThread(proc, (uintptr_t)&GetDeviceCount, PriviledgeApp, (uint64_t)ExternalDataAddDevice, &GetDeviceCountThread);
    SrvData->GetDeviceCount = MakeShareableThread(GetDeviceCountThread, PriviledgeService);

    /* GetDeviceInfo */
    thread_t GetDeviceInfoThread = NULL;
    Sys_CreateThread(proc, (uintptr_t)&GetDeviceInfo, PriviledgeApp, (uint64_t)ExternalDataAddDevice, &GetDeviceInfoThread);
    SrvData->GetDeviceInfo = MakeShareableThread(GetDeviceInfoThread, PriviledgeService);

    /* AddDevice */
    thread_t AddDeviceThread = NULL;
    Sys_CreateThread(proc, (uintptr_t)&AddDevice, PriviledgeApp, (uint64_t)ExternalDataAddDevice, &AddDeviceThread);
    SrvData->AddDevice = MakeShareableThread(AddDeviceThread, PriviledgeDriver);
    
    CreateControllerUISD(ControllerTypeEnum_Audio, key, true);
    return KSUCCESS;
}

/* Input part of server */
UISDServerEntry KResult RequestStream(thread_t Callback, uint64_t CallbackArg, uint64_t OutputID, process_t ProcessKey){
    KResult Status = KFAIL;

    uint64_t PID = Sys_GetPIDThreadLauncher();

    AddDeviceExternalData* ExternalDataAddDevice = (AddDeviceExternalData*)Sys_GetExternalDataThread();

    StreamRequest_t* StreamRequest = ExternalDataAddDevice->OutputsClass->RequestStream(OutputID, ProcessKey, PID);
    
    if(StreamRequest){
        Status = KSUCCESS;
    
        arguments_t arguments{
            .arg[0] = Status,           /* Status */
            .arg[1] = CallbackArg,      /* CallbackArg */
            .arg[2] = NULL,             /* OutputBuffer */
            .arg[3] = NULL,             /* GP1 */
            .arg[4] = NULL,             /* GP2 */
            .arg[5] = NULL,             /* GP3 */
        };

        ShareDataWithArguments_t Data{
            .Size = sizeof(audio_share_buffer_t),
            .Data = StreamRequest->ShareBuffer,
            .ParameterPosition = 0x2,
        };
        Sys_ExecThread(Callback, &arguments, ExecutionTypeQueu, &Data);
    }else{
        arguments_t arguments{
            .arg[0] = Status,           /* Status */
            .arg[1] = CallbackArg,      /* CallbackArg */
            .arg[2] = NULL,             /* OutputBuffer */
            .arg[3] = NULL,             /* GP1 */
            .arg[4] = NULL,             /* GP2 */
            .arg[5] = NULL,             /* GP3 */
        };
        
        Sys_ExecThread(Callback, &arguments, ExecutionTypeQueu, NULL);
    }
    Sys_Close(KSUCCESS);
}

KResult StreamCommand(thread_t Callback, uint64_t CallbackArg, uint64_t Command, uint64_t GP0, uint64_t GP1, uint64_t GP2){
    StreamRequest_t* Stream = (StreamRequest_t*)Sys_GetExternalDataThread();

    KResult Status = KFAIL;

    switch(Command){
        case AUDIO_STREAM_CLOSE:{
            Status = Stream->Self->CloseStream(Stream);
            break;
        }
        case AUDIO_STREAM_SET_VOLUME:{
            Stream->LocalBuffer->Volume = GP0 & 0xff;
            Status = KSUCCESS;
            break;
        }
        default:{
            break;
        }
    }

    arguments_t arguments{
        .arg[0] = Status,               /* Status */
        .arg[1] = CallbackArg,          /* CallbackArg */
        .arg[2] = NULL,                 /* GP0 */
        .arg[3] = NULL,                 /* GP1 */
        .arg[4] = NULL,                 /* GP2 */
        .arg[5] = NULL,                 /* GP3 */
    };

    Sys_ExecThread(Callback, &arguments, ExecutionTypeQueu, NULL);
    if(Command == AUDIO_STREAM_CLOSE && Status == KSUCCESS){
        Sys_Exit(KSUCCESS);
    }else{
        Sys_Close(KSUCCESS);
    }
}

UISDServerEntry KResult ChangeVolume(thread_t Callback, uint64_t CallbackArg, uint64_t OutputID, uint8_t Volume){
    AddDeviceExternalData* ExternalDataAddDevice = (AddDeviceExternalData*)Sys_GetExternalDataThread();

    KResult Status = ExternalDataAddDevice->OutputsClass->ChangeVolume(OutputID, Volume);

    arguments_t arguments{
        .arg[0] = Status,               /* Status */
        .arg[1] = CallbackArg,          /* CallbackArg */
        .arg[2] = NULL,                 /* GP0 */
        .arg[3] = NULL,                 /* GP1 */
        .arg[4] = NULL,                 /* GP2 */
        .arg[5] = NULL,                 /* GP3 */
    };

    Sys_ExecThread(Callback, &arguments, ExecutionTypeQueu, NULL);
    Sys_Close(KSUCCESS);
}

UISDServerEntry KResult SetDefault(thread_t Callback, uint64_t CallbackArg, uint64_t OutputID){
    AddDeviceExternalData* ExternalDataAddDevice = (AddDeviceExternalData*)Sys_GetExternalDataThread();

    KResult Status = ExternalDataAddDevice->OutputsClass->SetDefault(OutputID);

    arguments_t arguments{
        .arg[0] = Status,               /* Status */
        .arg[1] = CallbackArg,          /* CallbackArg */
        .arg[2] = NULL,                 /* GP0 */
        .arg[3] = NULL,                 /* GP1 */
        .arg[4] = NULL,                 /* GP2 */
        .arg[5] = NULL,                 /* GP3 */
    };

    Sys_ExecThread(Callback, &arguments, ExecutionTypeQueu, NULL);
    Sys_Close(KSUCCESS);
}

UISDServerEntry KResult GetDeviceCount(thread_t Callback, uint64_t CallbackArg){
    AddDeviceExternalData* ExternalDataAddDevice = (AddDeviceExternalData*)Sys_GetExternalDataThread();

    arguments_t arguments{
        .arg[0] = KSUCCESS,                                                     /* Status */
        .arg[1] = CallbackArg,                                                  /* CallbackArg */
        .arg[2] = ExternalDataAddDevice->OutputsClass->GetDeviceCount(),        /* DeviceCount */
        .arg[3] = NULL,                                                         /* GP1 */
        .arg[4] = NULL,                                                         /* GP2 */
        .arg[5] = NULL,                                                         /* GP3 */
    };

    Sys_ExecThread(Callback, &arguments, ExecutionTypeQueu, NULL);
    Sys_Close(KSUCCESS);
}

UISDServerEntry KResult GetDeviceInfo(thread_t Callback, uint64_t CallbackArg, uint64_t OutputID){
    AddDeviceExternalData* ExternalDataAddDevice = (AddDeviceExternalData*)Sys_GetExternalDataThread();

    srv_audio_device_info_t Info;
    KResult Status = ExternalDataAddDevice->OutputsClass->GetDeviceInfo(OutputID, &Info);

    arguments_t arguments{
        .arg[0] = Status,               /* Status */
        .arg[1] = CallbackArg,          /* CallbackArg */
        .arg[2] = NULL,                 /* Info */
        .arg[3] = NULL,                 /* GP1 */
        .arg[4] = NULL,                 /* GP2 */
        .arg[5] = NULL,                 /* GP3 */
    };

    ShareDataWithArguments_t Data{
        .Size = sizeof(srv_audio_device_info_t),
        .Data = &Info,
        .ParameterPosition = 0x2,
    };

    Sys_ExecThread(Callback, &arguments, ExecutionTypeQueu, &Data);
    Sys_Close(KSUCCESS);
}

/* Output part of server */
UISDServerEntry KResult AddDevice(thread_t Callback, uint64_t CallbackArg, srv_audio_device_t* Device){
    KResult Status = KFAIL;

    AddDeviceExternalData* ExternalDataAddDevice = (AddDeviceExternalData*)Sys_GetExternalDataThread();

    if(Device != NULL){
        // TODO : in
        switch(Device->Info.Type){
            case AudioDeviceTypeOut:{
                Status = ExternalDataAddDevice->OutputsClass->AddOutputDevice(Device);
                break;
            }
            default:{
                break;
            }
        }
    }

    arguments_t arguments{
        .arg[0] = Status,               /* Status */
        .arg[1] = CallbackArg,          /* CallbackArg */
        .arg[2] = NULL,                 /* GP0 */
        .arg[3] = NULL,                 /* GP1 */
        .arg[4] = NULL,                 /* GP2 */
        .arg[5] = NULL,                 /* GP3 */
    };

    Sys_ExecThread(Callback, &arguments, ExecutionTypeQueu, NULL);
    Sys_Close(KSUCCESS);
}

thread_t ChangeStatusCallbackThread = NULL;

void ChangeStatusCallback(KResult Status, struct CallbackAudio* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3){
    Callback->Status = Status;
    Sys_Unpause(Callback->Self);
    Sys_Close(KSUCCESS);
}

CallbackAudio* ChangeStatus(srv_audio_device_t* Device, enum AudioSetStatus Function, uint64_t GP0, uint64_t GP1, uint64_t GP2){
    if(!ChangeStatusCallbackThread){
        thread_t AudioThreadKeyCallback = NULL;
        Sys_CreateThread(Sys_GetProcess(), (uintptr_t)&ChangeStatusCallback, PriviledgeDriver, NULL, &AudioThreadKeyCallback);
        ChangeStatusCallbackThread = MakeShareableThread(AudioThreadKeyCallback, PriviledgeDriver);
    }

    CallbackAudio* Callback = (CallbackAudio*)malloc(sizeof(CallbackAudio));
    Callback->Self = Sys_GetThread();
    Callback->Status = KBUSY;

    struct arguments_t parameters{
        .arg[0] = ChangeStatusCallbackThread,
        .arg[1] = (uint64_t)Callback,
        .arg[2] = Function,
        .arg[3] = GP0,
        .arg[4] = GP1,
        .arg[5] = GP2,
    };

    KResult Status = Sys_ExecThread(Device->ChangeStatus, &parameters, ExecutionTypeQueu, NULL);
    if(Status == KSUCCESS){
        Sys_Pause(false);
    }
    return Callback;
}

KResult SetRunningState(srv_audio_device_t* Device, bool IsRunning){
    CallbackAudio* Callback = ChangeStatus(Device, AudioSetStatusRunningState, IsRunning, 0, 0);
    KResult Status = Callback->Status;
    free(Callback);
    return Status;
}

KResult SetVolume(srv_audio_device_t* Device, audio_volume_t Volume){
    CallbackAudio* Callback = ChangeStatus(Device, AudioSetStatusVolume, Volume, 0, 0);
    KResult Status = Callback->Status;
    free(Callback);
    return Status;
}