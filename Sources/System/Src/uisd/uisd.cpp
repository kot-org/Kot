#include <uisd/uisd.h>

kot_thread_t UISDHandlerThread;

controller_info_t** UISDControllers;

extern size64_t kot_ControllerTypeSize[ControllerCount];

kot_thread_t UISDInitialize(kot_process_t* process) {
    kot_thread_t UISDthreadKey;

    kot_process_t proc = kot_Sys_GetProcess();

    kot_Sys_CreateThread(proc, (void*)UISDHandler, PriviledgeService, NULL, &UISDHandlerThread);
    kot_Sys_Keyhole_CloneModify(UISDHandlerThread, &UISDthreadKey, NULL, KeyholeFlagPresent | KeyholeFlagDataTypeThreadIsExecutableWithQueue, PriviledgeApp);

    UISDControllers = (controller_info_t**)calloc(UISDMaxController, sizeof(controller_info_t*));

    kot_Sys_Keyhole_CloneModify(proc, process, NULL, KeyholeFlagPresent, PriviledgeApp);

    return UISDthreadKey;

}

void UISDAddToQueu(enum kot_uisd_controller_type_enum Controller, kot_thread_t Callback, uint64_t Callbackarg, kot_process_t Self, void* Address){
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
    KResult Status = kot_Sys_AcceptMemoryField(Callback->Self, UISDControllers[Callback->Controller]->DataKey, &Callback->Address);
    kot_arguments_t parameters{
        .arg[0] = UISDGetTask,
        .arg[1] = (uint64_t)Status,
        .arg[2] = Callback->Callbackarg,
        .arg[3] = (uint64_t)Callback->Address,
    };
    kot_Sys_ExecThread(Callback->Callback, &parameters, ExecutionTypeQueu, NULL);        
}

void UISDAcceptAll(enum kot_uisd_controller_type_enum Controller){
    for(uint64_t i = 0; i < UISDControllers[Controller]->NumberOfWaitingTasks; i++){
        callbackget_info_t* callback = (callbackget_info_t*)UISDControllers[Controller]->WaitingTasks->pop64();
        UISDAccept(callback);
        free(callback);
    }
}

KResult UISDCreate(enum kot_uisd_controller_type_enum Controller, kot_thread_t Callback, uint64_t Callbackarg, kot_key_mem_t DataKey){
    KResult Status = KFAIL;
    if(UISDControllers[Controller] == NULL || (UISDControllers[Controller] != NULL && !UISDControllers[Controller]->IsLoad)){
        enum kot_MemoryFieldType Type;
        size64_t Size = NULL;
        kot_process_t Target = NULL;
        uint64_t Flags = NULL;
        uint64_t Priviledge = NULL;
        if(kot_Sys_Keyhole_Verify(DataKey, DataTypeSharedMemory, &Target, &Flags, &Priviledge) != KSUCCESS) return KKEYVIOLATION;
        if(kot_Sys_GetInfoMemoryField(DataKey, (uint64_t*)&Type, &Size) == KSUCCESS){
            if(Type == MemoryFieldTypeShareSpaceRW || Type == MemoryFieldTypeShareSpaceRO){
                if(Size == kot_ControllerTypeSize[Controller]){
                    if(!UISDControllers[Controller]){
                        UISDControllers[Controller] = (controller_info_t*)malloc(sizeof(controller_info_t));
                        UISDControllers[Controller]->NumberOfWaitingTasks = NULL;
                    }
                    UISDControllers[Controller]->DataKey = DataKey;
                    UISDControllers[Controller]->Data = kot_GetFreeAlignedSpace(Size);
                    if(kot_Sys_AcceptMemoryField(proc, DataKey, (void**)&UISDControllers[Controller]->Data)){
                        UISDControllers[Controller]->IsLoad = true;
                        UISDAcceptAll(Controller);
                        Status = KSUCCESS;
                    }                    
                }
            }
        }
    }   
    return UISDCallbackStatu(UISDCreateTask, Callback, Callbackarg, Status);
}

KResult UISDGet(enum kot_uisd_controller_type_enum Controller, kot_thread_t Callback, uint64_t Callbackarg, kot_process_t Self, void* Address){
    kot_process_t Target = NULL;
    uint64_t Flags = NULL;
    uint64_t Priviledge = NULL;
    if(kot_Sys_Keyhole_Verify(Self, DataTypeProcess, &Target, &Flags, &Priviledge) != KSUCCESS) return UISDCallbackStatu(UISDGetTask, Callback, Callbackarg, KFAIL);
    if(!(Flags & KeyholeFlagDataTypeProcessMemoryAccessible)) return UISDCallbackStatu(UISDGetTask, Callback, Callbackarg, KFAIL);
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

KResult UISDCallbackStatu(uint64_t IPCTask, kot_thread_t Callback, uint64_t Callbackarg, KResult Status){
    kot_arguments_t parameters{
        .arg[0] = IPCTask,
        .arg[1] = (uint64_t)Status,
        .arg[2] = Callbackarg,
    };
    kot_Sys_ExecThread(Callback, &parameters, ExecutionTypeQueu, NULL);  
    return Status;
}

void UISDHandler(uint64_t IPCTask, enum kot_uisd_controller_type_enum Controller, kot_thread_t Callback, uint64_t Callbackarg, uint64_t GP0, uint64_t GP1) {
    KResult Status = KFAIL;
    if(Controller < ControllerCount && IPCTask <= 0x2){
        switch (IPCTask) {
        case UISDCreateTask:
            Status = (KResult)UISDCreate(Controller, Callback, Callbackarg, (kot_key_mem_t)GP0);
            break;
        case UISDGetTask:
            Status = (KResult)UISDGet(Controller, Callback, Callbackarg, (kot_process_t)GP0, (void*)GP1);
            break;
        default: /* TODO add free task */
            UISDCallbackStatu(IPCTask, Callback, Callbackarg, Status);
            break;
        }
    }
    kot_Sys_Close(Status);
}