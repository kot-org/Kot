#include <kot/uisd/srvs/audio.h>
#include <stdlib.h>
#include <string.h>

extern "C" {

kot_thread_t kot_srv_audio_callback_thread = NULL;
kot_process_t kot_ShareProcess = NULL;

void kot_Srv_Audio_Initialize(){
    kot_uisd_audio_t* AudioData = (kot_uisd_audio_t*)kot_FindControllerUISD(ControllerTypeEnum_Audio);
    kot_process_t proc = kot_Sys_GetProcess();
    kot_ShareProcess = kot_ShareProcessKey(proc);

    kot_thread_t AudioThreadKeyCallback = NULL;
    kot_Sys_CreateThread(proc, (uintptr_t)&kot_Srv_Audio_Callback, PriviledgeDriver, NULL, &AudioThreadKeyCallback);
    kot_InitializeThread(AudioThreadKeyCallback);
    kot_srv_audio_callback_thread = kot_MakeShareableThreadToProcess(AudioThreadKeyCallback, AudioData->ControllerHeader.Process);
}

void kot_Srv_Audio_Callback(KResult Status, struct kot_srv_audio_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3){
    Callback->Status = Callback->Handler(Status, Callback, GP0, GP1, GP2, GP3);

    if(Callback->IsAwait){
        kot_Sys_Unpause(Callback->Self);
    }
        
    kot_Sys_Close(KSUCCESS);
}

/* RequestStream */
KResult Srv_Audio_RequestStream_Callback(KResult Status, struct kot_srv_audio_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3){
    if(Status == KSUCCESS){
        Callback->Data = (uint64_t)malloc(sizeof(kot_audio_share_buffer_t));
        memcpy((void*)Callback->Data, (void*)GP0, sizeof(kot_audio_share_buffer_t));
        Callback->Size = sizeof(kot_audio_share_buffer_t);
    }
    return Status;
}

struct kot_srv_audio_callback_t* kot_Srv_Audio_RequestStream(uint64_t OutputID, bool IsAwait){
    if(!kot_srv_audio_callback_thread) kot_Srv_Audio_Initialize();
    kot_uisd_audio_t* AudioData = (kot_uisd_audio_t*)kot_FindControllerUISD(ControllerTypeEnum_Audio);

    kot_thread_t self = kot_Sys_GetThread();

    struct kot_srv_audio_callback_t* callback = (struct kot_srv_audio_callback_t*)malloc(sizeof(struct kot_srv_audio_callback_t));
    callback->Self = self;
    callback->Data = NULL;
    callback->Size = NULL;
    callback->IsAwait = IsAwait;
    callback->Status = KBUSY;
    callback->Handler = &Srv_Audio_RequestStream_Callback; 

    struct kot_arguments_t parameters;
    parameters.arg[0] = kot_srv_audio_callback_thread;
    parameters.arg[1] = (uint64_t)callback;
    parameters.arg[2] = OutputID;
    parameters.arg[3] = kot_ShareProcess;

    KResult Status = kot_Sys_ExecThread(AudioData->RequestStream, &parameters, ExecutionTypeQueu, NULL);
    if(Status == KSUCCESS && IsAwait){
        kot_Sys_Pause(false);
    }
    return callback;
}

/* StreamCommand */
KResult Srv_Audio_StreamCommand_Callback(KResult Status, struct kot_srv_audio_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3){
    return Status;
}

struct kot_srv_audio_callback_t* kot_Srv_Audio_StreamCommand(kot_audio_share_buffer_t* ShareBuffer, uint64_t Command, uint64_t GP0, uint64_t GP1, uint64_t GP2, bool IsAwait){
    if(!kot_srv_audio_callback_thread) kot_Srv_Audio_Initialize();
    kot_uisd_audio_t* AudioData = (kot_uisd_audio_t*)kot_FindControllerUISD(ControllerTypeEnum_Audio);

    kot_thread_t self = kot_Sys_GetThread();

    struct kot_srv_audio_callback_t* callback = (struct kot_srv_audio_callback_t*)malloc(sizeof(struct kot_srv_audio_callback_t));
    callback->Self = self;
    callback->Data = NULL;
    callback->Size = NULL;
    callback->IsAwait = IsAwait;
    callback->Status = KBUSY;
    callback->Handler = &Srv_Audio_StreamCommand_Callback; 

    struct kot_arguments_t parameters;
    parameters.arg[0] = kot_srv_audio_callback_thread;
    parameters.arg[1] = (uint64_t)callback;
    parameters.arg[2] = Command;
    parameters.arg[3] = GP0;
    parameters.arg[4] = GP1;
    parameters.arg[5] = GP2;

    KResult Status = kot_Sys_ExecThread(ShareBuffer->StreamCommand, &parameters, ExecutionTypeQueu, NULL);
    if(Status == KSUCCESS && IsAwait){
        kot_Sys_Pause(false);
    }
    return callback;
}

KResult kot_CloseStream(kot_audio_share_buffer_t* ShareBuffer){
    struct kot_srv_audio_callback_t* Callback = kot_Srv_Audio_StreamCommand(ShareBuffer, AUDIO_STREAM_CLOSE, 0, 0, 0, true);
    KResult Status = Callback->Status;
    free(Callback);
    return Status;
}

KResult kot_ChangeVolumeStream(kot_audio_share_buffer_t* ShareBuffer, uint8_t Volume){
    struct kot_srv_audio_callback_t* Callback = kot_Srv_Audio_StreamCommand(ShareBuffer, AUDIO_STREAM_SET_VOLUME, Volume, 0, 0, true);
    KResult Status = Callback->Status;
    free(Callback);
    return Status;
}

/* ChangeVolume */
KResult Srv_Audio_ChangeVolume_Callback(KResult Status, struct kot_srv_audio_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3){
    return Status;
}

struct kot_srv_audio_callback_t* kot_Srv_Audio_ChangeVolume(uint64_t OutputID, uint8_t Volume, bool IsAwait){
    if(!kot_srv_audio_callback_thread) kot_Srv_Audio_Initialize();
    kot_uisd_audio_t* AudioData = (kot_uisd_audio_t*)kot_FindControllerUISD(ControllerTypeEnum_Audio);

    kot_thread_t self = kot_Sys_GetThread();

    struct kot_srv_audio_callback_t* callback = (struct kot_srv_audio_callback_t*)malloc(sizeof(struct kot_srv_audio_callback_t));
    callback->Self = self;
    callback->Data = NULL;
    callback->Size = NULL;
    callback->IsAwait = IsAwait;
    callback->Status = KBUSY;
    callback->Handler = &Srv_Audio_ChangeVolume_Callback; 

    struct kot_arguments_t parameters;
    parameters.arg[0] = kot_srv_audio_callback_thread;
    parameters.arg[1] = (uint64_t)callback;
    parameters.arg[2] = OutputID;
    parameters.arg[3] = Volume;

    KResult Status = kot_Sys_ExecThread(AudioData->ChangeVolume, &parameters, ExecutionTypeQueu, NULL);
    if(Status == KSUCCESS && IsAwait){
        kot_Sys_Pause(false);
    }
    return callback;
}

/* SetDefault */
KResult Srv_Audio_SetDefault_Callback(KResult Status, struct kot_srv_audio_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3){
    return Status;
}

struct kot_srv_audio_callback_t* kot_Srv_Audio_SetDefault(uint64_t OutputID, bool IsAwait){
    if(!kot_srv_audio_callback_thread) kot_Srv_Audio_Initialize();
    kot_uisd_audio_t* AudioData = (kot_uisd_audio_t*)kot_FindControllerUISD(ControllerTypeEnum_Audio);

    kot_thread_t self = kot_Sys_GetThread();

    struct kot_srv_audio_callback_t* callback = (struct kot_srv_audio_callback_t*)malloc(sizeof(struct kot_srv_audio_callback_t));
    callback->Self = self;
    callback->Data = NULL;
    callback->Size = NULL;
    callback->IsAwait = IsAwait;
    callback->Status = KBUSY;
    callback->Handler = &Srv_Audio_SetDefault_Callback; 

    struct kot_arguments_t parameters;
    parameters.arg[0] = kot_srv_audio_callback_thread;
    parameters.arg[1] = (uint64_t)callback;
    parameters.arg[2] = OutputID;

    KResult Status = kot_Sys_ExecThread(AudioData->SetDefault, &parameters, ExecutionTypeQueu, NULL);
    if(Status == KSUCCESS && IsAwait){
        kot_Sys_Pause(false);
    }
    return callback;
}

/* GetDeviceInfo */
KResult Srv_Audio_GetDeviceInfo_Callback(KResult Status, struct kot_srv_audio_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3){
    if(Status == KSUCCESS){
        Callback->Data = (uint64_t)malloc(sizeof(kot_srv_audio_device_info_t));
        memcpy((void*)Callback->Data, (void*)GP0, sizeof(kot_srv_audio_device_info_t));
        Callback->Size = sizeof(kot_srv_audio_device_info_t);
    }
    return Status;
}

struct kot_srv_audio_callback_t* kot_Srv_Audio_GetDeviceInfo(uint64_t OutputID, bool IsAwait){
    if(!kot_srv_audio_callback_thread) kot_Srv_Audio_Initialize();
    kot_uisd_audio_t* AudioData = (kot_uisd_audio_t*)kot_FindControllerUISD(ControllerTypeEnum_Audio);

    kot_thread_t self = kot_Sys_GetThread();

    struct kot_srv_audio_callback_t* callback = (struct kot_srv_audio_callback_t*)malloc(sizeof(struct kot_srv_audio_callback_t));
    callback->Self = self;
    callback->Data = NULL;
    callback->Size = NULL;
    callback->IsAwait = IsAwait;
    callback->Status = KBUSY;
    callback->Handler = &Srv_Audio_GetDeviceInfo_Callback; 

    struct kot_arguments_t parameters;
    parameters.arg[0] = kot_srv_audio_callback_thread;
    parameters.arg[1] = (uint64_t)callback;
    parameters.arg[2] = OutputID;

    KResult Status = kot_Sys_ExecThread(AudioData->GetDeviceInfo, &parameters, ExecutionTypeQueu, NULL);
    if(Status == KSUCCESS && IsAwait){
        kot_Sys_Pause(false);
    }
    return callback;
}

/* AddDevice */
KResult Srv_Audio_AddDevice_Callback(KResult Status, struct kot_srv_audio_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3){
    if(Status == KSUCCESS){
        Callback->Data = GP0;
        Callback->Size = sizeof(size64_t);
    }
    return Status;
}

struct kot_srv_audio_callback_t* kot_Srv_Audio_AddDevice(kot_srv_audio_device_t* Device, bool IsAwait){
    if(!kot_srv_audio_callback_thread) kot_Srv_Audio_Initialize();
    kot_uisd_audio_t* AudioData = (kot_uisd_audio_t*)kot_FindControllerUISD(ControllerTypeEnum_Audio);

    kot_thread_t self = kot_Sys_GetThread();

    struct kot_srv_audio_callback_t* callback = (struct kot_srv_audio_callback_t*)malloc(sizeof(struct kot_srv_audio_callback_t));
    callback->Self = self;
    callback->Data = NULL;
    callback->Size = NULL;
    callback->IsAwait = IsAwait;
    callback->Status = KBUSY;
    callback->Handler = &Srv_Audio_AddDevice_Callback; 

    struct kot_ShareDataWithArguments_t data;
    data.Data = (uintptr_t)Device;
    data.Size = sizeof(kot_srv_audio_device_t);
    data.ParameterPosition = 0x2;

    struct kot_arguments_t parameters;
    parameters.arg[0] = kot_srv_audio_callback_thread;
    parameters.arg[1] = (uint64_t)callback;

    KResult Status = kot_Sys_ExecThread(AudioData->AddDevice, &parameters, ExecutionTypeQueu, &data);
    if(Status == KSUCCESS && IsAwait){
        kot_Sys_Pause(false);
    }
    return callback;
}

}