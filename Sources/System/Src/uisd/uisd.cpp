#include <uisd/uisd.h>

thread threadIPC;

controller_t** UISDControllers;

thread UISDInitialize() {

    thread IPCthreadKey;
    uint64_t IPCKeyFlags = NULL;
    Keyhole_SetFlag(&IPCKeyFlags, KeyholeFlagPresent, true);
    Keyhole_SetFlag(&IPCKeyFlags, KeyholeFlagDataTypethreadIsExecutableAsIPC, true);

    process_t process;
    Sys_GetProcessKey(&process);

    Sys_Createthread(process, (uintptr_t)UISDHandler, PriviledgeService, NULL, &threadIPC);
    Sys_Keyhole_CloneModify(threadIPC, &IPCthreadKey, NULL, IPCKeyFlags);

    UISDControllers = (controller_t**)calloc(sizeof(controller_t) * UISDMaxController);
    return IPCthreadKey;

}

KResult UISDCreate(enum ControllerTypeEnum Controller, ksmem_t DataKey) {
    if(!UISDControllers[Controller]){
        enum MemoryFieldType Type;
        size_t Size = NULL;
        process_t Target = NULL;
        uint64_t Flags = NULL;
        if(Sys_Keyhole_Verify(DataKey, DataTypeSharedMemory, &Target, &Flags) != KSUCCESS) return KKEYVIOLATION;
        if(Sys_GetInfoMemoryField(DataKey, (uint64_t*)&Type, &Size) == KSUCCESS){
            if(Type == MemoryFieldTypeSendSpaceRO && Size == sizeof(controller_t)){
                UISDControllers[Controller] = (controller_t*)malloc(Size);
                Sys_AcceptMemoryField(proc, DataKey, (uintptr_t*)&UISDControllers[Controller]);
                if(Sys_Keyhole_Verify(UISDControllers[Controller]->Data, DataTypeSharedMemory, &Target, &Flags) != KSUCCESS){
                    if(Sys_GetInfoMemoryField(UISDControllers[Controller]->Data, (uint64_t*)&Type, &Size) == KSUCCESS){
                        if(Type == MemoryFieldTypeSendSpaceRO){
                            return KSUCCESS;
                        }
                    }                    
                }

                Sys_FreeMemoryField(proc, DataKey, UISDControllers[Controller]);
                free(UISDControllers[Controller]);
                UISDControllers[Controller] = NULL;
            }
        }
        return KFAIL;
    }else{
        return KFAIL;
    }
}

KResult UISDGet(enum ControllerTypeEnum Controller, process_t self, uintptr_t address) {
    
    return KSUCCESS;
}

void UISDHandler(uint64_t IPCTask, enum ControllerTypeEnum Controller, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3) {
    uint64_t ReturnValue = NULL;
    switch (IPCTask) {
    case UISDCreateTask:
        ReturnValue = (uint64_t)UISDCreate(Controller, (ksmem_t)GP0);
        break;
    case UISDGetTask:
        ReturnValue = (uint64_t)UISDGet(Controller, (process_t)GP0, (uintptr_t)GP1);
        break;
    case UISDFreeTask:
        break;
    }
    SYS_Exit(NULL, ReturnValue);
}