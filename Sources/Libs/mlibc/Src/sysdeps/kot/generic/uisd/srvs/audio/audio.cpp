#include <kot/uisd/srvs/audio.h>
#include <stdlib.h>
#include <string.h>


namespace Kot{
    kot_thread_t SrvAudioCallbackThread = NULL;
    kot_process_t ShareProcess = NULL;

    void Srv_Audio_Initialize(){
        uisd_audio_t* AudioData = (uisd_audio_t*)FindControllerUISD(ControllerTypeEnum_Audio);
        kot_process_t proc = Sys_GetProcess();
        ShareProcess = ShareProcessKey(proc);

        kot_thread_t AudioThreadKeyCallback = NULL;
        Sys_CreateThread(proc, (uintptr_t)&Srv_Audio_Callback, PriviledgeDriver, NULL, &AudioThreadKeyCallback);
        InitializeThread(AudioThreadKeyCallback);
        SrvAudioCallbackThread = MakeShareableThreadToProcess(AudioThreadKeyCallback, AudioData->ControllerHeader.Process);
    }

    void Srv_Audio_Callback(KResult Status, struct srv_audio_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3){
        Callback->Status = Callback->Handler(Status, Callback, GP0, GP1, GP2, GP3);

        if(Callback->IsAwait){
            Sys_Unpause(Callback->Self);
        }
            
        Sys_Close(KSUCCESS);
    }

    /* RequestStream */
    KResult Srv_Audio_RequestStream_Callback(KResult Status, struct srv_audio_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3){
        if(Status == KSUCCESS){
            Callback->Data = (uint64_t)malloc(sizeof(audio_share_buffer_t));
            memcpy((void*)Callback->Data, (void*)GP0, sizeof(audio_share_buffer_t));
            Callback->Size = sizeof(audio_share_buffer_t);
        }
        return Status;
    }

    struct srv_audio_callback_t* Srv_Audio_RequestStream(uint64_t OutputID, bool IsAwait){
        if(!SrvAudioCallbackThread) Srv_Audio_Initialize();
        uisd_audio_t* AudioData = (uisd_audio_t*)FindControllerUISD(ControllerTypeEnum_Audio);

        kot_thread_t self = Sys_GetThread();

        struct srv_audio_callback_t* callback = (struct srv_audio_callback_t*)malloc(sizeof(struct srv_audio_callback_t));
        callback->Self = self;
        callback->Data = NULL;
        callback->Size = NULL;
        callback->IsAwait = IsAwait;
        callback->Status = KBUSY;
        callback->Handler = &Srv_Audio_RequestStream_Callback; 

        struct kot_arguments_t parameters;
        parameters.arg[0] = SrvAudioCallbackThread;
        parameters.arg[1] = (uint64_t)callback;
        parameters.arg[2] = OutputID;
        parameters.arg[3] = ShareProcess;

        KResult Status = Sys_ExecThread(AudioData->RequestStream, &parameters, ExecutionTypeQueu, NULL);
        if(Status == KSUCCESS && IsAwait){
            Sys_Pause(false);
        }
        return callback;
    }

    /* StreamCommand */
    KResult Srv_Audio_StreamCommand_Callback(KResult Status, struct srv_audio_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3){
        return Status;
    }

    struct srv_audio_callback_t* Srv_Audio_StreamCommand(audio_share_buffer_t* ShareBuffer, uint64_t Command, uint64_t GP0, uint64_t GP1, uint64_t GP2, bool IsAwait){
        if(!SrvAudioCallbackThread) Srv_Audio_Initialize();
        uisd_audio_t* AudioData = (uisd_audio_t*)FindControllerUISD(ControllerTypeEnum_Audio);

        kot_thread_t self = Sys_GetThread();

        struct srv_audio_callback_t* callback = (struct srv_audio_callback_t*)malloc(sizeof(struct srv_audio_callback_t));
        callback->Self = self;
        callback->Data = NULL;
        callback->Size = NULL;
        callback->IsAwait = IsAwait;
        callback->Status = KBUSY;
        callback->Handler = &Srv_Audio_StreamCommand_Callback; 

        struct kot_arguments_t parameters;
        parameters.arg[0] = SrvAudioCallbackThread;
        parameters.arg[1] = (uint64_t)callback;
        parameters.arg[2] = Command;
        parameters.arg[3] = GP0;
        parameters.arg[4] = GP1;
        parameters.arg[5] = GP2;

        KResult Status = Sys_ExecThread(ShareBuffer->StreamCommand, &parameters, ExecutionTypeQueu, NULL);
        if(Status == KSUCCESS && IsAwait){
            Sys_Pause(false);
        }
        return callback;
    }

    KResult CloseStream(audio_share_buffer_t* ShareBuffer){
        struct srv_audio_callback_t* Callback = Srv_Audio_StreamCommand(ShareBuffer, AUDIO_STREAM_CLOSE, 0, 0, 0, true);
        KResult Status = Callback->Status;
        free(Callback);
        return Status;
    }

    KResult ChangeVolumeStream(audio_share_buffer_t* ShareBuffer, uint8_t Volume){
        struct srv_audio_callback_t* Callback = Srv_Audio_StreamCommand(ShareBuffer, AUDIO_STREAM_SET_VOLUME, Volume, 0, 0, true);
        KResult Status = Callback->Status;
        free(Callback);
        return Status;
    }

    /* ChangeVolume */
    KResult Srv_Audio_ChangeVolume_Callback(KResult Status, struct srv_audio_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3){
        return Status;
    }

    struct srv_audio_callback_t* Srv_Audio_ChangeVolume(uint64_t OutputID, uint8_t Volume, bool IsAwait){
        if(!SrvAudioCallbackThread) Srv_Audio_Initialize();
        uisd_audio_t* AudioData = (uisd_audio_t*)FindControllerUISD(ControllerTypeEnum_Audio);

        kot_thread_t self = Sys_GetThread();

        struct srv_audio_callback_t* callback = (struct srv_audio_callback_t*)malloc(sizeof(struct srv_audio_callback_t));
        callback->Self = self;
        callback->Data = NULL;
        callback->Size = NULL;
        callback->IsAwait = IsAwait;
        callback->Status = KBUSY;
        callback->Handler = &Srv_Audio_ChangeVolume_Callback; 

        struct kot_arguments_t parameters;
        parameters.arg[0] = SrvAudioCallbackThread;
        parameters.arg[1] = (uint64_t)callback;
        parameters.arg[2] = OutputID;
        parameters.arg[3] = Volume;

        KResult Status = Sys_ExecThread(AudioData->ChangeVolume, &parameters, ExecutionTypeQueu, NULL);
        if(Status == KSUCCESS && IsAwait){
            Sys_Pause(false);
        }
        return callback;
    }

    /* SetDefault */
    KResult Srv_Audio_SetDefault_Callback(KResult Status, struct srv_audio_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3){
        return Status;
    }

    struct srv_audio_callback_t* Srv_Audio_SetDefault(uint64_t OutputID, bool IsAwait){
        if(!SrvAudioCallbackThread) Srv_Audio_Initialize();
        uisd_audio_t* AudioData = (uisd_audio_t*)FindControllerUISD(ControllerTypeEnum_Audio);

        kot_thread_t self = Sys_GetThread();

        struct srv_audio_callback_t* callback = (struct srv_audio_callback_t*)malloc(sizeof(struct srv_audio_callback_t));
        callback->Self = self;
        callback->Data = NULL;
        callback->Size = NULL;
        callback->IsAwait = IsAwait;
        callback->Status = KBUSY;
        callback->Handler = &Srv_Audio_SetDefault_Callback; 

        struct kot_arguments_t parameters;
        parameters.arg[0] = SrvAudioCallbackThread;
        parameters.arg[1] = (uint64_t)callback;
        parameters.arg[2] = OutputID;

        KResult Status = Sys_ExecThread(AudioData->SetDefault, &parameters, ExecutionTypeQueu, NULL);
        if(Status == KSUCCESS && IsAwait){
            Sys_Pause(false);
        }
        return callback;
    }

    /* GetDeviceInfo */
    KResult Srv_Audio_GetDeviceInfo_Callback(KResult Status, struct srv_audio_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3){
        if(Status == KSUCCESS){
            Callback->Data = (uint64_t)malloc(sizeof(srv_audio_device_info_t));
            memcpy((void*)Callback->Data, (void*)GP0, sizeof(srv_audio_device_info_t));
            Callback->Size = sizeof(srv_audio_device_info_t);
        }
        return Status;
    }

    struct srv_audio_callback_t* Srv_Audio_GetDeviceInfo(uint64_t OutputID, bool IsAwait){
        if(!SrvAudioCallbackThread) Srv_Audio_Initialize();
        uisd_audio_t* AudioData = (uisd_audio_t*)FindControllerUISD(ControllerTypeEnum_Audio);

        kot_thread_t self = Sys_GetThread();

        struct srv_audio_callback_t* callback = (struct srv_audio_callback_t*)malloc(sizeof(struct srv_audio_callback_t));
        callback->Self = self;
        callback->Data = NULL;
        callback->Size = NULL;
        callback->IsAwait = IsAwait;
        callback->Status = KBUSY;
        callback->Handler = &Srv_Audio_GetDeviceInfo_Callback; 

        struct kot_arguments_t parameters;
        parameters.arg[0] = SrvAudioCallbackThread;
        parameters.arg[1] = (uint64_t)callback;
        parameters.arg[2] = OutputID;

        KResult Status = Sys_ExecThread(AudioData->GetDeviceInfo, &parameters, ExecutionTypeQueu, NULL);
        if(Status == KSUCCESS && IsAwait){
            Sys_Pause(false);
        }
        return callback;
    }

    /* AddDevice */
    KResult Srv_Audio_AddDevice_Callback(KResult Status, struct srv_audio_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3){
        if(Status == KSUCCESS){
            Callback->Data = GP0;
            Callback->Size = sizeof(size64_t);
        }
        return Status;
    }

    struct srv_audio_callback_t* Srv_Audio_AddDevice(srv_audio_device_t* Device, bool IsAwait){
        if(!SrvAudioCallbackThread) Srv_Audio_Initialize();
        uisd_audio_t* AudioData = (uisd_audio_t*)FindControllerUISD(ControllerTypeEnum_Audio);

        kot_thread_t self = Sys_GetThread();

        struct srv_audio_callback_t* callback = (struct srv_audio_callback_t*)malloc(sizeof(struct srv_audio_callback_t));
        callback->Self = self;
        callback->Data = NULL;
        callback->Size = NULL;
        callback->IsAwait = IsAwait;
        callback->Status = KBUSY;
        callback->Handler = &Srv_Audio_AddDevice_Callback; 

        struct ShareDataWithArguments_t data;
        data.Data = (uintptr_t)Device;
        data.Size = sizeof(srv_audio_device_t);
        data.ParameterPosition = 0x2;

        struct kot_arguments_t parameters;
        parameters.arg[0] = SrvAudioCallbackThread;
        parameters.arg[1] = (uint64_t)callback;

        KResult Status = Sys_ExecThread(AudioData->AddDevice, &parameters, ExecutionTypeQueu, &data);
        if(Status == KSUCCESS && IsAwait){
            Sys_Pause(false);
        }
        return callback;
    }
}