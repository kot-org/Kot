#include <kot/uisd/srvs/audio.h>

thread_t SrvAudioCallbackThread = NULL;

void Srv_Audio_Initialize(){
    uisd_audio_t* AudioData = (uisd_audio_t*)FindControllerUISD(ControllerTypeEnum_Audio);
    process_t proc = Sys_GetProcess();

    thread_t AudioThreadKeyCallback = NULL;
    Sys_CreateThread(proc, &Srv_Audio_Callback, PriviledgeDriver, NULL, &AudioThreadKeyCallback);
    SrvAudioCallbackThread = MakeShareableThreadToProcess(AudioThreadKeyCallback, AudioData->ControllerHeader.Process);
}

void Srv_Audio_Callback(KResult Status, struct srv_audio_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3){
    Callback->Status = Callback->Handler(Status, Callback, GP0, GP1, GP2, GP3);

    if(Callback->IsAwait){
        Sys_Unpause(Callback->Self);
    }
        
    Sys_Close(KSUCCESS);
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

    thread_t self = Sys_Getthread();

    struct srv_audio_callback_t* callback = (struct srv_audio_callback_t*)malloc(sizeof(struct srv_audio_callback_t));
    callback->Self = self;
    callback->Data = NULL;
    callback->Size = NULL;
    callback->IsAwait = IsAwait;
    callback->Status = KBUSY;
    callback->Handler = &Srv_Audio_AddDevice_Callback; 

    struct ShareDataWithArguments_t data;
    data.Data = Device;
    data.Size = sizeof(srv_audio_device_t);
    data.ParameterPosition = 0x2;

    struct arguments_t parameters;
    parameters.arg[0] = SrvAudioCallbackThread;
    parameters.arg[1] = callback;

    KResult Status = Sys_ExecThread(AudioData->AddDevice, &parameters, ExecutionTypeQueu, &data);
    if(Status == KSUCCESS && IsAwait){
        Sys_Pause(false);
    }
    return callback;
}