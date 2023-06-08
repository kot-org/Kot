#include <srv/srv.h>

kot_uisd_audio_t* SrvData;

KResult InitialiseServer(){
    AddDeviceExternalData* ExternalDataAddDevice = (AddDeviceExternalData*)malloc(sizeof(AddDeviceExternalData));
    kot_process_t proc = kot_Sys_GetProcess();

    void* address = kot_GetFreeAlignedSpace(sizeof(kot_uisd_audio_t));
    kot_key_mem_t key = NULL;
    kot_Sys_CreateMemoryField(proc, sizeof(kot_uisd_audio_t), &address, &key, MemoryFieldTypeShareSpaceRO);

    SrvData = (kot_uisd_audio_t*)address;
    memset(SrvData, 0, sizeof(kot_uisd_audio_t)); // Clear data

    SrvData->ControllerHeader.IsReadWrite = false;
    SrvData->ControllerHeader.Version = Audio_Srv_Version;
    SrvData->ControllerHeader.VendorID = Kot_VendorID;
    SrvData->ControllerHeader.Type = ControllerTypeEnum_Audio;
    SrvData->ControllerHeader.Process = kot_ShareProcessKey(proc);

    /* OnDeviceChanged */
    kot_event_t OnDeviceChanged;
    kot_Sys_Event_Create(&OnDeviceChanged);
    kot_Sys_Keyhole_CloneModify(OnDeviceChanged, &SrvData->OnDeviceChanged, NULL, KeyholeFlagPresent | KeyholeFlagDataTypeEventIsBindable, PriviledgeApp);

    ExternalDataAddDevice->OutputsClass = new Outputs(SrvData->OnDeviceChanged);

    /* RequestStream */
    kot_thread_t RequestStreamThread = NULL;
    kot_Sys_CreateThread(proc, (void*)&RequestStream, PriviledgeApp, (uint64_t)ExternalDataAddDevice, &RequestStreamThread);
    SrvData->RequestStream = kot_MakeShareableThread(RequestStreamThread, PriviledgeApp);

    /* ChangeVolume */
    kot_thread_t ChangeVolumeThread = NULL;
    kot_Sys_CreateThread(proc, (void*)&ChangeVolume, PriviledgeApp, (uint64_t)ExternalDataAddDevice, &ChangeVolumeThread);
    SrvData->ChangeVolume = kot_MakeShareableThread(ChangeVolumeThread, PriviledgeService);

    /* SetDefault */
    kot_thread_t SetDefaultThread = NULL;
    kot_Sys_CreateThread(proc, (void*)&SetDefault, PriviledgeApp, (uint64_t)ExternalDataAddDevice, &SetDefaultThread);
    SrvData->SetDefault = kot_MakeShareableThread(SetDefaultThread, PriviledgeService);

    /* GetDeviceCount */
    kot_thread_t GetDeviceCountThread = NULL;
    kot_Sys_CreateThread(proc, (void*)&GetDeviceCount, PriviledgeApp, (uint64_t)ExternalDataAddDevice, &GetDeviceCountThread);
    SrvData->GetDeviceCount = kot_MakeShareableThread(GetDeviceCountThread, PriviledgeService);

    /* GetDeviceInfo */
    kot_thread_t GetDeviceInfoThread = NULL;
    kot_Sys_CreateThread(proc, (void*)&GetDeviceInfo, PriviledgeApp, (uint64_t)ExternalDataAddDevice, &GetDeviceInfoThread);
    SrvData->GetDeviceInfo = kot_MakeShareableThread(GetDeviceInfoThread, PriviledgeService);

    /* AddDevice */
    kot_thread_t AddDeviceThread = NULL;
    kot_Sys_CreateThread(proc, (void*)&AddDevice, PriviledgeApp, (uint64_t)ExternalDataAddDevice, &AddDeviceThread);
    SrvData->AddDevice = kot_MakeShareableThread(AddDeviceThread, PriviledgeDriver);
    
    kot_CreateControllerUISD(ControllerTypeEnum_Audio, key, true);
    return KSUCCESS;
}

/* Input part of server */
UISDServerEntry KResult RequestStream(kot_thread_t Callback, uint64_t CallbackArg, uint64_t OutputID, kot_process_t ProcessKey){
    KResult Status = KFAIL;

    uint64_t PID = kot_Sys_GetPIDThreadLauncher();

    AddDeviceExternalData* ExternalDataAddDevice = (AddDeviceExternalData*)kot_Sys_GetExternalDataThread();

    StreamRequest_t* StreamRequest = ExternalDataAddDevice->OutputsClass->RequestStream(OutputID, ProcessKey, PID);
    
    if(StreamRequest){
        Status = KSUCCESS;
    
        kot_arguments_t arguments{
            .arg[0] = Status,           /* Status */
            .arg[1] = CallbackArg,      /* CallbackArg */
            .arg[2] = NULL,             /* OutputBuffer */
            .arg[3] = NULL,             /* GP1 */
            .arg[4] = NULL,             /* GP2 */
            .arg[5] = NULL,             /* GP3 */
        };

        kot_ShareDataWithArguments_t Data{
            .Size = sizeof(kot_audio_share_buffer_t),
            .Data = StreamRequest->ShareBuffer,
            .ParameterPosition = 0x2,
        };
        kot_Sys_ExecThread(Callback, &arguments, ExecutionTypeQueu, &Data);
    }else{
        kot_arguments_t arguments{
            .arg[0] = Status,           /* Status */
            .arg[1] = CallbackArg,      /* CallbackArg */
            .arg[2] = NULL,             /* OutputBuffer */
            .arg[3] = NULL,             /* GP1 */
            .arg[4] = NULL,             /* GP2 */
            .arg[5] = NULL,             /* GP3 */
        };
        
        kot_Sys_ExecThread(Callback, &arguments, ExecutionTypeQueu, NULL);
    }
    kot_Sys_Close(KSUCCESS);
}

KResult StreamCommand(kot_thread_t Callback, uint64_t CallbackArg, uint64_t Command, uint64_t GP0, uint64_t GP1, uint64_t GP2){
    StreamRequest_t* Stream = (StreamRequest_t*)kot_Sys_GetExternalDataThread();

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

    kot_arguments_t arguments{
        .arg[0] = Status,               /* Status */
        .arg[1] = CallbackArg,          /* CallbackArg */
        .arg[2] = NULL,                 /* GP0 */
        .arg[3] = NULL,                 /* GP1 */
        .arg[4] = NULL,                 /* GP2 */
        .arg[5] = NULL,                 /* GP3 */
    };

    kot_Sys_ExecThread(Callback, &arguments, ExecutionTypeQueu, NULL);
    if(Command == AUDIO_STREAM_CLOSE && Status == KSUCCESS){
        kot_Sys_Exit(KSUCCESS);
    }else{
        kot_Sys_Close(KSUCCESS);
    }
}

UISDServerEntry KResult ChangeVolume(kot_thread_t Callback, uint64_t CallbackArg, uint64_t OutputID, uint8_t Volume){
    AddDeviceExternalData* ExternalDataAddDevice = (AddDeviceExternalData*)kot_Sys_GetExternalDataThread();

    KResult Status = ExternalDataAddDevice->OutputsClass->ChangeVolume(OutputID, Volume);

    kot_arguments_t arguments{
        .arg[0] = Status,               /* Status */
        .arg[1] = CallbackArg,          /* CallbackArg */
        .arg[2] = NULL,                 /* GP0 */
        .arg[3] = NULL,                 /* GP1 */
        .arg[4] = NULL,                 /* GP2 */
        .arg[5] = NULL,                 /* GP3 */
    };

    kot_Sys_ExecThread(Callback, &arguments, ExecutionTypeQueu, NULL);
    kot_Sys_Close(KSUCCESS);
}

UISDServerEntry KResult SetDefault(kot_thread_t Callback, uint64_t CallbackArg, uint64_t OutputID){
    AddDeviceExternalData* ExternalDataAddDevice = (AddDeviceExternalData*)kot_Sys_GetExternalDataThread();

    KResult Status = ExternalDataAddDevice->OutputsClass->SetDefault(OutputID);

    kot_arguments_t arguments{
        .arg[0] = Status,               /* Status */
        .arg[1] = CallbackArg,          /* CallbackArg */
        .arg[2] = NULL,                 /* GP0 */
        .arg[3] = NULL,                 /* GP1 */
        .arg[4] = NULL,                 /* GP2 */
        .arg[5] = NULL,                 /* GP3 */
    };

    kot_Sys_ExecThread(Callback, &arguments, ExecutionTypeQueu, NULL);
    kot_Sys_Close(KSUCCESS);
}

UISDServerEntry KResult GetDeviceCount(kot_thread_t Callback, uint64_t CallbackArg){
    AddDeviceExternalData* ExternalDataAddDevice = (AddDeviceExternalData*)kot_Sys_GetExternalDataThread();

    kot_arguments_t arguments{
        .arg[0] = KSUCCESS,                                                     /* Status */
        .arg[1] = CallbackArg,                                                  /* CallbackArg */
        .arg[2] = ExternalDataAddDevice->OutputsClass->GetDeviceCount(),        /* DeviceCount */
        .arg[3] = NULL,                                                         /* GP1 */
        .arg[4] = NULL,                                                         /* GP2 */
        .arg[5] = NULL,                                                         /* GP3 */
    };

    kot_Sys_ExecThread(Callback, &arguments, ExecutionTypeQueu, NULL);
    kot_Sys_Close(KSUCCESS);
}

UISDServerEntry KResult GetDeviceInfo(kot_thread_t Callback, uint64_t CallbackArg, uint64_t OutputID){
    AddDeviceExternalData* ExternalDataAddDevice = (AddDeviceExternalData*)kot_Sys_GetExternalDataThread();

    kot_srv_audio_device_info_t Info;
    KResult Status = ExternalDataAddDevice->OutputsClass->GetDeviceInfo(OutputID, &Info);

    kot_arguments_t arguments{
        .arg[0] = Status,               /* Status */
        .arg[1] = CallbackArg,          /* CallbackArg */
        .arg[2] = NULL,                 /* Info */
        .arg[3] = NULL,                 /* GP1 */
        .arg[4] = NULL,                 /* GP2 */
        .arg[5] = NULL,                 /* GP3 */
    };

    kot_ShareDataWithArguments_t Data{
        .Size = sizeof(kot_srv_audio_device_info_t),
        .Data = &Info,
        .ParameterPosition = 0x2,
    };

    kot_Sys_ExecThread(Callback, &arguments, ExecutionTypeQueu, &Data);
    kot_Sys_Close(KSUCCESS);
}

/* Output part of server */
UISDServerEntry KResult AddDevice(kot_thread_t Callback, uint64_t CallbackArg, kot_srv_audio_device_t* Device){
    KResult Status = KFAIL;

    AddDeviceExternalData* ExternalDataAddDevice = (AddDeviceExternalData*)kot_Sys_GetExternalDataThread();

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

    kot_arguments_t arguments{
        .arg[0] = Status,               /* Status */
        .arg[1] = CallbackArg,          /* CallbackArg */
        .arg[2] = NULL,                 /* GP0 */
        .arg[3] = NULL,                 /* GP1 */
        .arg[4] = NULL,                 /* GP2 */
        .arg[5] = NULL,                 /* GP3 */
    };

    kot_Sys_ExecThread(Callback, &arguments, ExecutionTypeQueu, NULL);
    kot_Sys_Close(KSUCCESS);
}

kot_thread_t ChangeStatusCallbackThread = NULL;

void ChangeStatusCallback(KResult Status, struct CallbackAudio* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3){
    Callback->Status = Status;
    kot_Sys_Unpause(Callback->Self);
    kot_Sys_Close(KSUCCESS);
}

CallbackAudio* ChangeStatus(kot_srv_audio_device_t* Device, enum kot_AudioSetStatus Function, uint64_t GP0, uint64_t GP1, uint64_t GP2){
    if(!ChangeStatusCallbackThread){
        kot_thread_t AudioThreadKeyCallback = NULL;
        kot_Sys_CreateThread(kot_Sys_GetProcess(), (void*)&ChangeStatusCallback, PriviledgeDriver, NULL, &AudioThreadKeyCallback);
        ChangeStatusCallbackThread = kot_MakeShareableThread(AudioThreadKeyCallback, PriviledgeDriver);
    }

    CallbackAudio* Callback = (CallbackAudio*)malloc(sizeof(CallbackAudio));
    Callback->Self = kot_Sys_GetThread();
    Callback->Status = KBUSY;

    struct kot_arguments_t parameters{
        .arg[0] = ChangeStatusCallbackThread,
        .arg[1] = (uint64_t)Callback,
        .arg[2] = Function,
        .arg[3] = GP0,
        .arg[4] = GP1,
        .arg[5] = GP2,
    };

    KResult Status = kot_Sys_ExecThread(Device->ChangeStatus, &parameters, ExecutionTypeQueu, NULL);
    if(Status == KSUCCESS){
        kot_Sys_Pause(false);
    }
    return Callback;
}

KResult SetRunningState(kot_srv_audio_device_t* Device, bool IsRunning){
    CallbackAudio* Callback = ChangeStatus(Device, AudioSetStatusRunningState, IsRunning, 0, 0);
    KResult Status = Callback->Status;
    free(Callback);
    return Status;
}

KResult SetVolume(kot_srv_audio_device_t* Device, kot_audio_volume_t Volume){
    CallbackAudio* Callback = ChangeStatus(Device, AudioSetStatusVolume, Volume, 0, 0);
    KResult Status = Callback->Status;
    free(Callback);
    return Status;
}