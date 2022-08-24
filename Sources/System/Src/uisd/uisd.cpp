#include <uisd/uisd.h>

thread_t UISDHandlerThread;

controller_info_t** UISDControllers;

size64_t ControllerTypeSize[ControllerCount] = {
    sizeof(graphics_t),
    sizeof(audio_t),
    sizeof(storage_t),
    sizeof(vfs_t),
    sizeof(usb_t),
    sizeof(pci_t)
};

thread_t UISDInitialize(process_t* process) {
    thread_t UISDthreadKey;
    uint64_t UISDKeyFlags = NULL;
    Keyhole_SetFlag(&UISDKeyFlags, KeyholeFlagPresent, true);
    Keyhole_SetFlag(&UISDKeyFlags, KeyholeFlagDataTypethreadIsExecutableWithQueue, true);

    process_t proc;
    Sys_GetProcessKey(&proc);

    Sys_Createthread(proc, (uintptr_t)UISDHandler, PriviledgeService, NULL, &UISDHandlerThread);
    Sys_Keyhole_CloneModify(UISDHandlerThread, &UISDthreadKey, NULL, UISDKeyFlags, PriviledgeApp);

    UISDControllers = (controller_info_t**)calloc(sizeof(controller_info_t*) * UISDMaxController);

    UISDKeyFlags = NULL;
    Keyhole_SetFlag(&UISDKeyFlags, KeyholeFlagPresent, true);
    Sys_Keyhole_CloneModify(proc, process, NULL, UISDKeyFlags, PriviledgeApp);

    return UISDthreadKey;

}

void UISDAddToQueu(enum ControllerTypeEnum Controller, thread_t Callback, uint64_t Callbackarg, process_t Self, uintptr_t Address){
    if(!UISDControllers[Controller]){
        UISDControllers[Controller] = (controller_info_t*)malloc(sizeof(controller_info_t));
        UISDControllers[Controller]->IsLoad = false;
        UISDControllers[Controller]->WaitingTasks = new std::Stack(0x50);
        UISDControllers[Controller]->NumberOfWaitingTasks = NULL;
    }
    
    callbackget_info_t* callbackInfo = (callbackget_info_t*)malloc(sizeof(callbackget_info_t));
    callbackInfo->Controller = Controller;
    callbackInfo->Self = Self;
    callbackInfo->Address = Address;
    callbackInfo->Callback = Callback;
    callbackInfo->Callbackarg = Callbackarg;
    UISDControllers[Controller]->WaitingTasks->push64((uint64_t)callbackInfo);
    UISDControllers[Controller]->NumberOfWaitingTasks++;
}

void UISDAccept(callbackget_info_t* Callback){
    KResult Statu = Sys_AcceptMemoryField(Callback->Self, UISDControllers[Callback->Controller]->DataKey, &Callback->Address);
    arguments_t parameters{
        .arg[0] = UISDGetTask,
        .arg[1] = (uint64_t)Statu,
        .arg[2] = Callback->Callbackarg,
        .arg[3] = (uint64_t)Callback->Address,
    };
    Sys_Execthread(Callback->Callback, &parameters, ExecutionTypeQueu, NULL);        
}

void UISDAcceptAll(enum ControllerTypeEnum Controller){
    for(uint64_t i = 0; i < UISDControllers[Controller]->NumberOfWaitingTasks; i++){
        callbackget_info_t* callback = (callbackget_info_t*)UISDControllers[Controller]->WaitingTasks->pop64();
        UISDAccept(callback);
        free(callback);
    }
}

KResult UISDCreate(enum ControllerTypeEnum Controller, thread_t Callback, uint64_t Callbackarg, ksmem_t DataKey) {
    KResult Statu = KFAIL;
    if(UISDControllers[Controller] == NULL || (UISDControllers[Controller] != NULL && !UISDControllers[Controller]->IsLoad)){
        enum MemoryFieldType Type;
        size64_t Size = NULL;
        process_t Target = NULL;
        uint64_t Flags = NULL;
        uint64_t Priviledge = NULL;
        if(Sys_Keyhole_Verify(DataKey, DataTypeSharedMemory, &Target, &Flags, &Priviledge) != KSUCCESS) return KKEYVIOLATION;
        if(Sys_GetInfoMemoryField(DataKey, (uint64_t*)&Type, &Size) == KSUCCESS){
            if(Type == MemoryFieldTypeShareSpaceRW || Type == MemoryFieldTypeShareSpaceRO){
                if(Size == ControllerTypeSize[Controller]){
                    if(!UISDControllers[Controller]){
                        UISDControllers[Controller] = (controller_info_t*)malloc(sizeof(controller_info_t));
                        UISDControllers[Controller]->NumberOfWaitingTasks = NULL;
                    }
                    UISDControllers[Controller]->DataKey = DataKey;
                    UISDControllers[Controller]->Data = getFreeAlignedSpace(Size);
                    if(Sys_AcceptMemoryField(proc, DataKey, (uintptr_t*)&UISDControllers[Controller])){
                        UISDControllers[Controller]->IsLoad = true;
                        UISDAcceptAll(Controller);
                        Statu = KSUCCESS;
                    }                    
                }
            }
        }
    }   
    return UISDCallbackStatu(UISDCreateTask, Callback, Callbackarg, Statu);
}

KResult UISDGet(enum ControllerTypeEnum Controller, thread_t Callback, uint64_t Callbackarg, process_t Self, uintptr_t Address) {
    process_t Target = NULL;
    uint64_t Flags = NULL;
    uint64_t Priviledge = NULL;
    if(Sys_Keyhole_Verify(Self, DataTypeProcess, &Target, &Flags, &Priviledge) != KSUCCESS) return UISDCallbackStatu(UISDGetTask, Callback, Callbackarg, KFAIL);
    if(!Keyhole_GetFlag(Flags, KeyholeFlagDataTypeProcessMemoryAccessible)) return UISDCallbackStatu(UISDGetTask, Callback, Callbackarg, KFAIL);
    if(UISDControllers[Controller] != NULL){
        if(UISDControllers[Controller]->IsLoad){
            callbackget_info_t info{
                .Controller = Controller,
                .Self = Self,
                .Address = Address,
                .Callback = Callback,
                .Callbackarg = Callbackarg,
            };
            UISDAccept(&info);
            return KSUCCESS;
        }
    }
    UISDAddToQueu(Controller, Callback, Callbackarg, Self, Address);
    return KSUCCESS;
}

KResult UISDCallbackStatu(uint64_t IPCTask, thread_t Callback, uint64_t Callbackarg, KResult Statu){
    arguments_t parameters{
        .arg[0] = IPCTask,
        .arg[1] = (uint64_t)Statu,
        .arg[2] = Callbackarg,
    };
    Sys_Execthread(Callback, &parameters, ExecutionTypeQueu, NULL);  
    return Statu;
}

void UISDHandler(uint64_t IPCTask, enum ControllerTypeEnum Controller, thread_t Callback, uint64_t Callbackarg, uint64_t GP0, uint64_t GP1) {
    KResult Statu = KFAIL;
    if(Controller <= 0xff && IPCTask <= 0x2){
        switch (IPCTask) {
        case UISDCreateTask:
            Statu = (KResult)UISDCreate(Controller, Callback, Callbackarg, (ksmem_t)GP0);
            break;
        case UISDGetTask:
            Statu = (KResult)UISDGet(Controller, Callback, Callbackarg, (process_t)GP0, (uintptr_t)GP1);
            break;
        default: /* TODO add free task */
            UISDCallbackStatu(IPCTask, Callback, Callbackarg, Statu);
            break;
        }
    }

    SYS_Close(Statu);
}