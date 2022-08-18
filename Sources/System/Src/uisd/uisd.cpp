#include <uisd/uisd.h>

thread threadIPC;

controller_info_t** UISDControllers;

thread UISDInitialize() {

    thread IPCthreadKey;
    uint64_t IPCKeyFlags = NULL;
    Keyhole_SetFlag(&IPCKeyFlags, KeyholeFlagPresent, true);
    Keyhole_SetFlag(&IPCKeyFlags, KeyholeFlagDataTypethreadIsExecutableWithQueue, true);

    process_t process;
    Sys_GetProcessKey(&process);

    Sys_Createthread(process, (uintptr_t)UISDHandler, PriviledgeService, NULL, &threadIPC);
    Sys_Keyhole_CloneModify(threadIPC, &IPCthreadKey, NULL, IPCKeyFlags);

    UISDControllers = (controller_info_t**)calloc(sizeof(controller_info_t) * UISDMaxController);
    return IPCthreadKey;

}

KResult UISDAccept(callback_info_t callback){
    KResult Statu = Sys_AcceptMemoryField(callback.Self, UISDControllers[callback.Controller]->DataKey, &callback.Address);
    if(Statu == KSUCCESS) {
        arguments_t parameters{
            .arg[0] = callback.Callbackarg,
            .arg[1] = (uint64_t)callback.Address,
        };
        return Sys_Execthread(callback.Callback, &parameters, ExecutionTypeQueu, NULL);        
    }
    return Statu;
}

KResult UISDCreate(enum ControllerTypeEnum Controller, thread callback, uint64_t callbackarg, ksmem_t DataKey) {
    if(UISDControllers[Controller] == NULL || !UISDControllers[Controller]->IsLoad){
        enum MemoryFieldType Type;
        size_t Size = NULL;
        process_t Target = NULL;
        uint64_t Flags = NULL;
        if(Sys_Keyhole_Verify(DataKey, DataTypeSharedMemory, &Target, &Flags) != KSUCCESS) return KKEYVIOLATION;
        
        if(Sys_GetInfoMemoryField(DataKey, (uint64_t*)&Type, &Size) == KSUCCESS){
            if(Type == MemoryFieldTypeSendSpaceRO){
                if(!UISDControllers[Controller]){
                    UISDControllers[Controller] = (controller_info_t*)malloc(sizeof(controller_info_t));
                }
                UISDControllers[Controller]->DataKey = DataKey;
                UISDControllers[Controller]->Data = getFreeAlihnedSpace(Size);
                if(Sys_AcceptMemoryField(proc, DataKey, (uintptr_t*)&UISDControllers[Controller])){
                    UISDControllers[Controller]->IsLoad = true;
                    Printlog("New controller added !");
                    return KSUCCESS;
                }else{
                    Printlog("[Error] Unknow error");
                }
            }
        }
    }
    return KFAIL;
}

KResult UISDGet(enum ControllerTypeEnum Controller, thread Callback, uint64_t Callbackarg, process_t Self, uintptr_t Address) {
    process_t Target = NULL;
    uint64_t Flags = NULL;
    if(Sys_Keyhole_Verify(Self, DataTypeSharedMemory, &Target, &Flags) != KSUCCESS) return NULL;
    if(!Keyhole_GetFlag(Flags, KeyholeFlagDataTypeProcessMemoryAccessible)) return NULL;
    if(UISDControllers[Controller] == NULL){
        if(UISDControllers[Controller]->IsLoad){
            UISDAccept((struct callback_info_t){
                .Controller = Controller,
                .Self = Self,
                .Address = Address,
                .Callback = Callback,
                .Callbackarg = Callbackarg,
            });
            return KSUCCESS;
        }else{
            Printlog("[TODO]");
            /* TODO create stack for tasks */
        }
    }else{
        UISDControllers[Controller] = (controller_info_t*)malloc(sizeof(controller_info_t));
        UISDControllers[Controller]->IsLoad = false;
        Printlog("[TODO]");
        /* TODO create stack for task */
    }
    return NULL;
}

void UISDHandler(uint64_t IPCTask, enum ControllerTypeEnum Controller, thread Callback, uint64_t Callbackarg, uint64_t GP0, uint64_t GP1) {
    uint64_t ReturnValue = NULL;
    switch (IPCTask) {
    case UISDCreateTask:
        ReturnValue = (uint64_t)UISDCreate(Controller, Callback, Callbackarg, (ksmem_t)GP0);
        break;
    case UISDGetTask:
        ReturnValue = (uint64_t)UISDGet(Controller, Callback, Callbackarg, (process_t)GP0, (uintptr_t)GP1);
        break;
    case UISDFreeTask:
        break;
    }
    SYS_Exit(NULL, ReturnValue);
}