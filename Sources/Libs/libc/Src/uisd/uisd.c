#include <kot/uisd.h>

size_t ControllerTypeSize[ControllerCount] = {
    sizeof(graphics_t),
    sizeof(audio_t),
    sizeof(storage_t),
    sizeof(vfs_t),
    sizeof(usb_t),
    sizeof(pci_t)
};

thread_t CallBackUISDThread = NULL;
process_t ProcessKeyForUISD = NULL;

KResult CallbackUISD(uint64_t Task, KResult Statu, callbackInfo_t* Info, uint64_t GP0, uint64_t GP1);

KResult InitializeUISD(){
    thread_t UISDthreadKeyCallback;
    uint64_t UISDKeyFlags = NULL;
    Keyhole_SetFlag(&UISDKeyFlags, KeyholeFlagPresent, true);
    Keyhole_SetFlag(&UISDKeyFlags, KeyholeFlagDataTypethreadIsExecutableWithQueue, true);

    process_t Proc = NULL;
    Sys_GetProcessKey(&Proc);

    UISDKeyFlags = NULL;
    Sys_Createthread(Proc, &CallbackUISD, PriviledgeApp, NULL, &UISDthreadKeyCallback);
    Sys_Keyhole_CloneModify(UISDthreadKeyCallback, &CallBackUISDThread, KotSpecificData.UISDHandlerProcess, UISDKeyFlags);

    UISDKeyFlags = NULL;
    Keyhole_SetFlag(&UISDKeyFlags, KeyholeFlagPresent, true);
    Keyhole_SetFlag(&UISDKeyFlags, KeyholeFlagDataTypeProcessMemoryAccessible, true);
    Sys_Keyhole_CloneModify(Proc, &ProcessKeyForUISD, KotSpecificData.UISDHandlerProcess, UISDKeyFlags);
    return KSUCCESS;
}

KResult CallbackUISD(uint64_t Task, KResult Statu, callbackInfo_t* Info, uint64_t GP0, uint64_t GP1){
    Printlog("oki");
    if(Task == UISDGetTask) Info->Location = (uintptr_t)GP0;
    Info->Statu = Statu;
    if(Info->AwaitCallback){
        SYS_Unpause(Info->Self);
    }
    SYS_Close(NULL, KSUCCESS);
}

callbackInfo_t* GetControllerUISD(enum ControllerTypeEnum Controller, uintptr_t* Location, bool AwaitCallback){
    if(!CallBackUISDThread) InitializeUISD();
    thread_t Self = NULL;
    Sys_GetthreadKey(&Self);
    callbackInfo_t* Info = (callbackInfo_t*)malloc(sizeof(callbackInfo_t));
    Info->Self = Self;
    Info->AwaitCallback = AwaitCallback;
    Info->Location = NULL;
    Info->Statu = KFAIL;

    struct arguments_t parameters;
    parameters.arg[0] = UISDGetTask,
    parameters.arg[1] = Controller,
    parameters.arg[2] = CallBackUISDThread,
    parameters.arg[3] = Info,
    parameters.arg[4] = ProcessKeyForUISD,
    parameters.arg[5] = (uint64_t)*Location,
    Sys_Execthread(KotSpecificData.UISDHandler, &parameters, ExecutionTypeQueu, NULL);
    if(AwaitCallback){
        SYS_Pause(Self);
        *Location = Info->Location;
        return Info;
    }
    return Info;
}

callbackInfo_t* CreateControllerUISD(enum ControllerTypeEnum Controller, ksmem_t MemoryField, bool AwaitCallback){
    if(!CallBackUISDThread) InitializeUISD();
    thread_t Self = NULL;
    Sys_GetthreadKey(&Self);
    callbackInfo_t* Info = malloc(sizeof(callbackInfo_t));
    Info->Self = Self;
    Info->AwaitCallback = AwaitCallback;
    Info->Statu = KFAIL;

    uint64_t Flags = NULL;
    Keyhole_SetFlag(&Flags, KeyholeFlagPresent, true);
    ksmem_t MemoryFieldKey = NULL;
    Sys_Keyhole_CloneModify(MemoryField, &MemoryFieldKey, KotSpecificData.UISDHandlerProcess, Flags);

    struct arguments_t parameters;
    parameters.arg[0] = UISDCreateTask,
    parameters.arg[1] = Controller,
    parameters.arg[2] = CallBackUISDThread,
    parameters.arg[3] = Info,
    parameters.arg[4] = MemoryFieldKey,
    Sys_Execthread(KotSpecificData.UISDHandler, &parameters, ExecutionTypeQueu, NULL);
    if(AwaitCallback){
        SYS_Pause(Self);
        return Info;
    }
    return Info;
}

