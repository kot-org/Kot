#include <kot/uisd/srvs/system.h>

thread_t srv_system_callback_thread = NULL;
uisd_system_t* SystemData = NULL;

void Srv_System_Initialize(){
    SystemData = (uisd_system_t*)FindControllerUISD(ControllerTypeEnum_System);
    if(SystemData != NULL){
        process_t Proc = Sys_GetProcess();

        thread_t SystemthreadKeyCallback = NULL;
        Sys_Createthread(Proc, &Srv_System_Callback, PriviledgeApp, &SystemthreadKeyCallback);
        srv_system_callback_thread = MakeShareableThreadToProcess(SystemthreadKeyCallback, SystemData->ControllerHeader.Process);
    }else{
        Sys_Close(KFAIL);
    }
}

void Srv_System_Callback(KResult Status, struct srv_system_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3){
    Callback->Status = Callback->Handler(Status, Callback, GP0, GP1, GP2, GP3);
    if(Callback->IsAwait){
        Sys_Unpause(Callback->Self);
    }
    Sys_Close(KSUCCESS);
}

/* GetFrameBufer */
KResult Srv_System_GetFrameBuffer_Callback(KResult Status, struct srv_system_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3){
    if(Status == KSUCCESS){
        Callback->Data = malloc(sizeof(srv_system_framebuffer_t));
        memcpy(Callback->Data, (uintptr_t)GP0, sizeof(srv_system_framebuffer_t));
        Callback->Size = (size64_t)sizeof(srv_system_framebuffer_t);
    }
    return Status;
}

struct srv_system_callback_t* Srv_System_GetFrameBuffer(bool IsAwait){
    if(!srv_system_callback_thread) Srv_System_Initialize();
    
    thread_t self = Sys_Getthread();

    struct srv_system_callback_t* callback = (struct srv_system_callback_t*)malloc(sizeof(struct srv_system_callback_t));
    callback->Self = self;
    callback->Data = NULL;
    callback->Size = NULL;
    callback->IsAwait = IsAwait;
    callback->Status = KBUSY;
    callback->Handler = &Srv_System_GetFrameBuffer_Callback;

    struct arguments_t parameters;
    parameters.arg[0] = srv_system_callback_thread;
    parameters.arg[1] = callback;
    

    KResult Status = Sys_Execthread(SystemData->GetFramebuffer, &parameters, ExecutionTypeQueu, NULL);
    if(Status == KSUCCESS && IsAwait){
        Sys_Pause(false);
    }
    return callback;
}

/* ReadFile */
KResult Srv_System_ReadFileInitrd_Callback(KResult Status, struct srv_system_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3){
    if(Status == KSUCCESS){
        Callback->Data = malloc((size64_t)GP0);
        memcpy(Callback->Data, (uintptr_t)GP1, (size64_t)GP0);
        Callback->Size = (size64_t)GP0;
    }
    return Status;
}

struct srv_system_callback_t* Srv_System_ReadFileInitrd(char* Name,  bool IsAwait){
    if(!srv_system_callback_thread) Srv_System_Initialize();
    
    thread_t self = Sys_Getthread();

    struct srv_system_callback_t* callback = (struct srv_system_callback_t*)malloc(sizeof(struct srv_system_callback_t));
    callback->Self = self;
    callback->Data = NULL;
    callback->Size = NULL;
    callback->IsAwait = IsAwait;
    callback->Status = KBUSY;
    callback->Handler = &Srv_System_ReadFileInitrd_Callback;

    struct ShareDataWithArguments_t data;
    if(Name != NULL){
        data.Data = Name;
        data.Size = strlen(Name) + 1; // add '\0' char
        data.ParameterPosition = 0x2; 
    }

    struct arguments_t parameters;
    parameters.arg[0] = srv_system_callback_thread;
    parameters.arg[1] = callback;

    KResult Status = Sys_Execthread(SystemData->ReadFileInitrd, &parameters, ExecutionTypeQueu, &data);
    if(Status == KSUCCESS && IsAwait){
        Sys_Pause(false);
    }
    return callback;
}

/* GetTableInRootSystemDescription */
KResult Srv_System_GetTableInRootSystemDescription_Callback(KResult Status, struct srv_system_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3){
    if(Status == KSUCCESS){
        Callback->Data = (uintptr_t)MapPhysical(GP0, GP1);
        Callback->Size = (size64_t)GP1;
    }
    return Status;
}

struct srv_system_callback_t* Srv_System_GetTableInRootSystemDescription(char* Name, bool IsAwait){
    if(!srv_system_callback_thread) Srv_System_Initialize();
    
    thread_t self = Sys_Getthread();

    struct srv_system_callback_t* callback = (struct srv_system_callback_t*)malloc(sizeof(struct srv_system_callback_t));
    callback->Self = self;
    callback->Data = NULL;
    callback->Size = NULL;
    callback->IsAwait = IsAwait;
    callback->Status = KBUSY;
    callback->Handler = &Srv_System_GetTableInRootSystemDescription_Callback;

    struct ShareDataWithArguments_t data;
    if(Name != NULL){
        data.Data = Name;
        data.Size = strlen(Name) + 1; // add '\0' char
        data.ParameterPosition = 0x2; 
    }

    struct arguments_t parameters;
    parameters.arg[0] = srv_system_callback_thread;
    parameters.arg[1] = callback;

    KResult Status = Sys_Execthread(SystemData->GetTableInRootSystemDescription, &parameters, ExecutionTypeQueu, &data);
    if(Status == KSUCCESS && IsAwait){
        Sys_Pause(false);
    }
    return callback;
}

/* GetSystemManagementBIOSTable */
KResult Srv_System_GetSystemManagementBIOSTable_Callback(KResult Status, struct srv_system_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3){
    if(Status == KSUCCESS){
        Callback->Data = GP0;
        Callback->Size = sizeof(uint64_t);
    }
    return Status;
}

struct srv_system_callback_t* Srv_System_GetSystemManagementBIOSTable(bool IsAwait){
    if(!srv_system_callback_thread) Srv_System_Initialize();
    
    thread_t self = Sys_Getthread();

    struct srv_system_callback_t* callback = (struct srv_system_callback_t*)malloc(sizeof(struct srv_system_callback_t));
    callback->Self = self;
    callback->Data = NULL;
    callback->Size = NULL;
    callback->IsAwait = IsAwait;
    callback->Status = KBUSY;
    callback->Handler = &Srv_System_GetSystemManagementBIOSTable_Callback;

    struct arguments_t parameters;
    parameters.arg[0] = srv_system_callback_thread;
    parameters.arg[1] = callback;
    

    KResult Status = Sys_Execthread(SystemData->GetSystemManagementBIOSTable, &parameters, ExecutionTypeQueu, NULL);
    if(Status == KSUCCESS && IsAwait){
        Sys_Pause(false);
    }
    return callback;
}

/* BindIRQLine */
KResult Srv_System_BindIRQLine_Callback(KResult Status, struct srv_system_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3){
    return Status;
}

struct srv_system_callback_t* Srv_System_BindIRQLine(uint8_t IRQLineNumber, thread_t Target, bool IgnoreMissedEvents, bool IsAwait){
    if(!srv_system_callback_thread) Srv_System_Initialize();
    
    thread_t self = Sys_Getthread();

    uint64_t TargetShareKey = NULL;
    uint64_t UISDKeyFlags = NULL;
    Keyhole_SetFlag(&UISDKeyFlags, KeyholeFlagPresent, true);
    Keyhole_SetFlag(&UISDKeyFlags, KeyholeFlagDataTypeThreadIsEventable, true);
    Sys_Keyhole_CloneModify(Target, &TargetShareKey, SystemData->ControllerHeader.Process, UISDKeyFlags, PriviledgeApp);

    struct srv_system_callback_t* callback = (struct srv_system_callback_t*)malloc(sizeof(struct srv_system_callback_t));
    callback->Self = self;
    callback->Data = NULL;
    callback->Size = NULL;
    callback->IsAwait = IsAwait;
    callback->Status = KBUSY;
    callback->Handler = &Srv_System_BindIRQLine_Callback;

    struct arguments_t parameters;
    parameters.arg[0] = srv_system_callback_thread;
    parameters.arg[1] = callback;
    parameters.arg[2] = IRQLineNumber;
    parameters.arg[3] = TargetShareKey;
    parameters.arg[4] = IgnoreMissedEvents;
    

    KResult Status = Sys_Execthread(SystemData->BindIRQLine, &parameters, ExecutionTypeQueu, NULL);
    if(Status == KSUCCESS && IsAwait){
        Sys_Pause(false);
    }
    return callback;
}

/* UnbindIRQLine */
KResult Srv_System_UnbindIRQLine_Callback(KResult Status, struct srv_system_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3){
    return Status;
}

struct srv_system_callback_t* Srv_System_UnbindIRQLine(uint8_t IRQLineNumber, thread_t Target, bool IsAwait){
    if(!srv_system_callback_thread) Srv_System_Initialize();
    
    thread_t self = Sys_Getthread();

    uint64_t TargetShareKey = NULL;
    uint64_t UISDKeyFlags = NULL;
    Keyhole_SetFlag(&UISDKeyFlags, KeyholeFlagPresent, true);
    Keyhole_SetFlag(&UISDKeyFlags, KeyholeFlagDataTypeThreadIsEventable, true);
    Sys_Keyhole_CloneModify(Target, &TargetShareKey, SystemData->ControllerHeader.Process, UISDKeyFlags, PriviledgeApp);

    struct srv_system_callback_t* callback = (struct srv_system_callback_t*)malloc(sizeof(struct srv_system_callback_t));
    callback->Self = self;
    callback->Data = NULL;
    callback->Size = NULL;
    callback->IsAwait = IsAwait;
    callback->Status = KBUSY;
    callback->Handler = &Srv_System_UnbindIRQLine_Callback;

    struct arguments_t parameters;
    parameters.arg[0] = srv_system_callback_thread;
    parameters.arg[1] = callback;
    parameters.arg[2] = IRQLineNumber;
    parameters.arg[3] = TargetShareKey;
    

    KResult Status = Sys_Execthread(SystemData->UnbindIRQLine, &parameters, ExecutionTypeQueu, NULL);
    if(Status == KSUCCESS && IsAwait){
        Sys_Pause(false);
    }
    return callback;
}

/* BindFreeIRQ */
KResult Srv_System_BindFreeIRQ_Callback(KResult Status, struct srv_system_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3){
    if(Status == KSUCCESS) {
        Callback->Data = GP0;
        Callback->Size = sizeof(uint64_t);
    }
    return Status;
}

struct srv_system_callback_t* Srv_System_BindFreeIRQ(uint8_t IRQLineNumber, thread_t Target, bool IgnoreMissedEvents, bool IsAwait){
    if(!srv_system_callback_thread) Srv_System_Initialize();
    
    thread_t self = Sys_Getthread();

    uint64_t TargetShareKey = NULL;
    uint64_t UISDKeyFlags = NULL;
    Keyhole_SetFlag(&UISDKeyFlags, KeyholeFlagPresent, true);
    Keyhole_SetFlag(&UISDKeyFlags, KeyholeFlagDataTypeThreadIsEventable, true);
    Sys_Keyhole_CloneModify(Target, &TargetShareKey, SystemData->ControllerHeader.Process, UISDKeyFlags, PriviledgeApp);

    struct srv_system_callback_t* callback = (struct srv_system_callback_t*)malloc(sizeof(struct srv_system_callback_t));
    callback->Self = self;
    callback->Data = NULL;
    callback->Size = NULL;
    callback->IsAwait = IsAwait;
    callback->Status = KBUSY;
    callback->Handler = &Srv_System_BindFreeIRQ_Callback;

    struct arguments_t parameters;
    parameters.arg[0] = srv_system_callback_thread;
    parameters.arg[1] = callback;
    parameters.arg[2] = IRQLineNumber;
    parameters.arg[3] = TargetShareKey;
    parameters.arg[4] = IgnoreMissedEvents;
    

    KResult Status = Sys_Execthread(SystemData->BindFreeIRQ, &parameters, ExecutionTypeQueu, NULL);
    if(Status == KSUCCESS && IsAwait){
        Sys_Pause(false);
    }
    return callback;
}

/* UnbindIRQ */
KResult Srv_System_UnbindIRQ_Callback(KResult Status, struct srv_system_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3){
    if(Status == KSUCCESS) {
        Callback->Data = GP0;
        Callback->Size = sizeof(uint64_t);
    }
    return Status;
}

struct srv_system_callback_t* Srv_System_UnbindIRQ(uint8_t IRQLineNumber, thread_t Target, bool IsAwait){
    if(!srv_system_callback_thread) Srv_System_Initialize();
    
    thread_t self = Sys_Getthread();

    uint64_t TargetShareKey = NULL;
    uint64_t UISDKeyFlags = NULL;
    Keyhole_SetFlag(&UISDKeyFlags, KeyholeFlagPresent, true);
    Keyhole_SetFlag(&UISDKeyFlags, KeyholeFlagDataTypeThreadIsEventable, true);
    Sys_Keyhole_CloneModify(Target, &TargetShareKey, SystemData->ControllerHeader.Process, UISDKeyFlags, PriviledgeApp);

    struct srv_system_callback_t* callback = (struct srv_system_callback_t*)malloc(sizeof(struct srv_system_callback_t));
    callback->Self = self;
    callback->Data = NULL;
    callback->Size = NULL;
    callback->IsAwait = IsAwait;
    callback->Status = KBUSY;
    callback->Handler = &Srv_System_BindFreeIRQ_Callback;

    struct arguments_t parameters;
    parameters.arg[0] = srv_system_callback_thread;
    parameters.arg[1] = callback;
    parameters.arg[2] = IRQLineNumber;
    parameters.arg[3] = TargetShareKey;
    

    KResult Status = Sys_Execthread(SystemData->UnbindIRQ, &parameters, ExecutionTypeQueu, NULL);
    if(Status == KSUCCESS && IsAwait){
        Sys_Pause(false);
    }
    return callback;
}
