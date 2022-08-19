#include <kot/uisd/uisd.h>

size_t ControllerTypeSize[ControllerCount] = {
    sizeof(graphics_t),
    sizeof(audio_t),
    sizeof(storage_t),
    sizeof(vfs_t),
    sizeof(usb_t),
    sizeof(pci_t)
};

thread CallBackUISDThread = NULL;

KResult CallbackUISD(uint64_t Task, KResult Statu, callbackInfo_t* Info);

KResult InitializeUISD(){
    thread UISDthreadKey;
    uint64_t UISDKeyFlags = NULL;
    Keyhole_SetFlag(&UISDKeyFlags, KeyholeFlagPresent, true);
    Keyhole_SetFlag(&UISDKeyFlags, KeyholeFlagDataTypethreadIsExecutableWithQueue, true);

    process_t Proc = NULL;
    Sys_GetProcessKey(&Proc);

    UISDKeyFlags = NULL;
    Sys_Createthread(Proc, &CallbackUISD, PriviledgeApp, NULL, &CallBackUISDThread);
    Sys_Keyhole_CloneModify(CallBackUISDThread, &UISDthreadKey, KotSpecificData.UISDHandlerProcess, UISDKeyFlags);
}

KResult CallbackUISD(uint64_t Task, KResult Statu, callbackInfo_t* Info, uint64_t GP0, uint64_t GP1){
    if(Task == UISDGetTask) Info->Location = (uintptr_t)GP0;
    Info->Statu = Statu;
    if(Info->AwaitCallback){
        KSys_UnPause(Info->Self);
    }
}

callbackInfo_t* GetControllerUISD(enum ControllerTypeEnum Controller, uintptr_t* Location, bool AwaitCallback){
    if(!CallBackUISDThread) InitializeUISD();
    thread Self = NULL;
    Sys_GetthreadKey(&Self);
    callbackInfo_t* Info = malloc(sizeof(callbackInfo_t));
    Info->Self = Self;
    Info->AwaitCallback = AwaitCallback;
    Info->Location = NULL;
    Info->Statu = KFAIL;

    process_t Proc = NULL;
    Sys_GetProcessKey(&Proc);
    struct arguments_t parameters;
    parameters.arg[0] = UISDGetTask,
    parameters.arg[1] = CallBackUISDThread,
    parameters.arg[2] = Self,
    parameters.arg[3] = Proc,
    parameters.arg[4] = (uint64_t)*Location,
    Sys_Execthread(KotSpecificData.UISDHandler, &parameters, ExecutionTypeQueu, NULL);
    if(AwaitCallback){
        Pause(Self);
        *Location = Info->Location;
        return Info;
    }
    return Info;
}

callbackInfo_t* CreateControllerUISD(enum ControllerTypeEnum Controller, ksmem_t MemoryField, bool AwaitCallback){
    if(!CallBackUISDThread) InitializeUISD();
    thread Self = NULL;
    Sys_GetthreadKey(&Self);
    callbackInfo_t* Info = malloc(sizeof(callbackInfo_t));
    Info->Self = Self;
    Info->AwaitCallback = AwaitCallback;
    Info->Location = Location;
    Info->Statu = KFAIL;

    struct arguments_t parameters;
    parameters.arg[0] = UISDCreateTask,
    parameters.arg[1] = CallBackUISDThread,
    parameters.arg[2] = Self,
    parameters.arg[3] = MemoryField,
    Sys_Execthread(KotSpecificData.UISDHandler, &parameters, ExecutionTypeQueu, NULL);
    if(AwaitCallback){
        Pause(Self);
        return Info;
    }
    return Info;
}

