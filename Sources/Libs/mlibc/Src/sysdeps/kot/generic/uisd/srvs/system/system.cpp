#include <kot/uisd/srvs/system.h>
#include <bits/ensure.h>
#include <stdlib.h>
#include <string.h>

extern "C" {

kot_thread_t kot_srv_system_callback_thread = NULL;
kot_uisd_system_t* kot_SystemData = NULL;
kot_process_t kot_ShareableProcessUISDSystem;

void kot_Srv_System_Initialize(){
    kot_SystemData = (kot_uisd_system_t*)kot_FindControllerUISD(ControllerTypeEnum_System);
    if(kot_SystemData != NULL){
        kot_process_t Proc = kot_Sys_GetProcess();
        kot_ShareableProcessUISDSystem = kot_ShareProcessKey(Proc);

        kot_thread_t SystemthreadKeyCallback = NULL;
        kot_Sys_CreateThread(Proc, (void*)&kot_Srv_System_Callback, PriviledgeMax, NULL, &SystemthreadKeyCallback);
        kot_srv_system_callback_thread = kot_MakeShareableThreadToProcess(SystemthreadKeyCallback, kot_SystemData->ControllerHeader.Process);
    }else{
        kot_Sys_Close(KFAIL);
    }
}

void kot_Srv_System_Callback(KResult Status, struct kot_srv_system_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3){
    Callback->Status = Callback->Handler(Status, Callback, GP0, GP1, GP2, GP3);
    if(Callback->IsAwait){
        kot_Sys_Unpause(Callback->Self);
    }
    kot_Sys_Close(KSUCCESS);
}

/* LoadExecutable */
KResult Srv_System_LoadExecutable_Callback(KResult Status, struct kot_srv_system_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3){
    if(Status == KSUCCESS){
        Callback->Data = GP0;
        Callback->Size = (size64_t)sizeof(kot_thread_t);
    }
    return Status;
}

struct kot_srv_system_callback_t* kot_Srv_System_LoadExecutable(uint64_t Priviledge, char* Path, bool IsAwait){
    if(!kot_srv_system_callback_thread) kot_Srv_System_Initialize();
    
    kot_thread_t self = kot_Sys_GetThread();

    struct kot_srv_system_callback_t* callback = (struct kot_srv_system_callback_t*)malloc(sizeof(struct kot_srv_system_callback_t));
    callback->Self = self;
    callback->Data = NULL;
    callback->Size = NULL;
    callback->IsAwait = IsAwait;
    callback->Status = KBUSY;
    callback->Handler = &Srv_System_LoadExecutable_Callback;

    struct kot_arguments_t parameters;
    parameters.arg[0] = kot_srv_system_callback_thread;
    parameters.arg[1] = (uint64_t)callback;
    parameters.arg[2] = kot_ShareableProcessUISDSystem;
    parameters.arg[3] = Priviledge;

    struct kot_ShareDataWithArguments_t data;

    if(Path != NULL){
        data.Data = (void*)Path;
        data.Size = strlen(Path) + 1; // add '\0' char
        data.ParameterPosition = 0x4; 
    }
    

    KResult Status = kot_Sys_ExecThread(kot_SystemData->LoadExecutable, &parameters, ExecutionTypeQueu, &data);
    if(Status == KSUCCESS && IsAwait){
        kot_Sys_Pause(false);
    }
    return callback;
}

/* LoadExecutableToProcess */
__attribute__((__noreturn__)) void kot_Srv_System_LoadExecutableToProcess(char* Path, void* Data, size64_t Size, uint64_t DataPosition, kot_arguments_t* Arguments, uint64_t Flags){
    __ensure(Path != NULL);

    if(!kot_srv_system_callback_thread) kot_Srv_System_Initialize();

    struct kot_arguments_t parameters;
    parameters.arg[0] = NULL;
    parameters.arg[1] = NULL;
    parameters.arg[2] = kot_ProcessKeyForUISD;
    parameters.arg[4] = Size;


    struct kot_ShareDataWithArguments_t data;

    size64_t TotalSize = Size + sizeof(uint8_t) + sizeof(uint64_t) + sizeof(kot_thread_t) + sizeof(kot_arguments_t) + strlen(Path) + 1; // add '\0' char

    data.Data = malloc(TotalSize);
    data.Size = TotalSize;
    data.ParameterPosition = 0x3; 

    memcpy(data.Data, Data, Size);
    *(uint8_t*)((uintptr_t)data.Data + Size) = DataPosition;
    *(uint64_t*)((uintptr_t)data.Data + Size + sizeof(uint8_t)) = DataPosition;
    *(kot_thread_t*)((uintptr_t)data.Data + Size + sizeof(uint8_t) + sizeof(uint64_t)) = KotSpecificData.VFSHandler;
    if(Arguments){
        memcpy((void*)((uintptr_t)data.Data + Size + sizeof(uint8_t) + sizeof(uint64_t) + sizeof(kot_thread_t)), Arguments, sizeof(kot_arguments_t));
    }else{
        memset((void*)((uintptr_t)data.Data + Size + sizeof(uint8_t) + sizeof(uint64_t) + sizeof(kot_thread_t)), 0, sizeof(kot_arguments_t));
    }
    memcpy((void*)((uintptr_t)data.Data + Size + sizeof(uint8_t) + sizeof(uint64_t) + sizeof(kot_thread_t) + sizeof(kot_arguments_t)), Path, strlen(Path) + 1);
    
    __ensure(kot_Sys_ExecThread(kot_SystemData->LoadExecutableToProcess, &parameters, ExecutionTypeQueu | ExecutionTypeClose, &data) == KSUCCESS);
}

/* GetFrameBufer */
KResult Srv_System_GetFramebuffer_Callback(KResult Status, struct kot_srv_system_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3){
    if(Status == KSUCCESS){
        Callback->Data = (uint64_t)malloc(sizeof(kot_srv_system_framebuffer_t));
        memcpy((void*)Callback->Data, (void*)GP0, sizeof(kot_srv_system_framebuffer_t));
        Callback->Size = (size64_t)sizeof(kot_srv_system_framebuffer_t);
    }
    return Status;
}

struct kot_srv_system_callback_t* kot_Srv_System_GetFramebuffer(bool IsAwait){
    if(!kot_srv_system_callback_thread) kot_Srv_System_Initialize();
    
    kot_thread_t self = kot_Sys_GetThread();

    struct kot_srv_system_callback_t* callback = (struct kot_srv_system_callback_t*)malloc(sizeof(struct kot_srv_system_callback_t));
    callback->Self = self;
    callback->Data = NULL;
    callback->Size = NULL;
    callback->IsAwait = IsAwait;
    callback->Status = KBUSY;
    callback->Handler = &Srv_System_GetFramebuffer_Callback;

    struct kot_arguments_t parameters;
    parameters.arg[0] = kot_srv_system_callback_thread;
    parameters.arg[1] = (uint64_t)callback;
    

    KResult Status = kot_Sys_ExecThread(kot_SystemData->GetFramebuffer, &parameters, ExecutionTypeQueu, NULL);
    if(Status == KSUCCESS && IsAwait){
        kot_Sys_Pause(false);
    }
    return callback;
}

/* ReadFile */
KResult Srv_System_ReadFileInitrd_Callback(KResult Status, struct kot_srv_system_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3){
    if(Status == KSUCCESS){
        Callback->Data = (uint64_t)malloc((size64_t)GP0);
        memcpy((void*)Callback->Data, (void*)GP1, (size64_t)GP0);
        Callback->Size = (size64_t)GP0;
    }
    return Status;
}

struct kot_srv_system_callback_t* kot_Srv_System_ReadFileInitrd(char* Name,  bool IsAwait){
    if(!kot_srv_system_callback_thread) kot_Srv_System_Initialize();
    
    kot_thread_t self = kot_Sys_GetThread();

    struct kot_srv_system_callback_t* callback = (struct kot_srv_system_callback_t*)malloc(sizeof(struct kot_srv_system_callback_t));
    callback->Self = self;
    callback->Data = NULL;
    callback->Size = NULL;
    callback->IsAwait = IsAwait;
    callback->Status = KBUSY;
    callback->Handler = &Srv_System_ReadFileInitrd_Callback;

    struct kot_ShareDataWithArguments_t data;
    if(Name != NULL){
        data.Data = (void*)Name;
        data.Size = strlen(Name) + 1; // add '\0' char
        data.ParameterPosition = 0x2; 
    }

    struct kot_arguments_t parameters;
    parameters.arg[0] = kot_srv_system_callback_thread;
    parameters.arg[1] = (uint64_t)callback;

    KResult Status = kot_Sys_ExecThread(kot_SystemData->ReadFileInitrd, &parameters, ExecutionTypeQueu, &data);
    if(Status == KSUCCESS && IsAwait){
        kot_Sys_Pause(false);
    }
    return callback;
}

/* GetTableInRootSystemDescription */
KResult Srv_System_GetTableInRootSystemDescription_Callback(KResult Status, struct kot_srv_system_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3){
    if(Status == KSUCCESS){
        Callback->Data = (uintptr_t)kot_MapPhysical((void*)GP0, GP1);
        Callback->Size = (size64_t)GP1;
    }
    return Status;
}

struct kot_srv_system_callback_t* kot_Srv_System_GetTableInRootSystemDescription(char* Name, bool IsAwait){
    if(!kot_srv_system_callback_thread) kot_Srv_System_Initialize();
    
    kot_thread_t self = kot_Sys_GetThread();

    struct kot_srv_system_callback_t* callback = (struct kot_srv_system_callback_t*)malloc(sizeof(struct kot_srv_system_callback_t));
    callback->Self = self;
    callback->Data = NULL;
    callback->Size = NULL;
    callback->IsAwait = IsAwait;
    callback->Status = KBUSY;
    callback->Handler = &Srv_System_GetTableInRootSystemDescription_Callback;

    struct kot_ShareDataWithArguments_t data;
    if(Name != NULL){
        data.Data = (void*)Name;
        data.Size = strlen(Name) + 1; // add '\0' char
        data.ParameterPosition = 0x2; 
    }

    struct kot_arguments_t parameters;
    parameters.arg[0] = kot_srv_system_callback_thread;
    parameters.arg[1] = (uint64_t)callback;

    KResult Status = kot_Sys_ExecThread(kot_SystemData->GetTableInRootSystemDescription, &parameters, ExecutionTypeQueu, &data);
    if(Status == KSUCCESS && IsAwait){
        kot_Sys_Pause(false);
    }
    return callback;
}

/* GetSystemManagementBIOSTable */
KResult Srv_System_GetSystemManagementBIOSTable_Callback(KResult Status, struct kot_srv_system_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3){
    if(Status == KSUCCESS){
        Callback->Data = GP0;
        Callback->Size = sizeof(uint64_t);
    }
    return Status;
}

struct kot_srv_system_callback_t* kot_Srv_System_GetSystemManagementBIOSTable(bool IsAwait){
    if(!kot_srv_system_callback_thread) kot_Srv_System_Initialize();
    
    kot_thread_t self = kot_Sys_GetThread();

    struct kot_srv_system_callback_t* callback = (struct kot_srv_system_callback_t*)malloc(sizeof(struct kot_srv_system_callback_t));
    callback->Self = self;
    callback->Data = NULL;
    callback->Size = NULL;
    callback->IsAwait = IsAwait;
    callback->Status = KBUSY;
    callback->Handler = &Srv_System_GetSystemManagementBIOSTable_Callback;

    struct kot_arguments_t parameters;
    parameters.arg[0] = kot_srv_system_callback_thread;
    parameters.arg[1] = (uint64_t)callback;
    

    KResult Status = kot_Sys_ExecThread(kot_SystemData->GetSystemManagementBIOSTable, &parameters, ExecutionTypeQueu, NULL);
    if(Status == KSUCCESS && IsAwait){
        kot_Sys_Pause(false);
    }
    return callback;
}

/* BindIRQLine */
KResult Srv_System_BindIRQLine_Callback(KResult Status, struct kot_srv_system_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3){
    return Status;
}

struct kot_srv_system_callback_t* kot_Srv_System_BindIRQLine(uint8_t IRQLineNumber, kot_thread_t Target, bool IgnoreMissedEvents, bool IsAwait){
    if(!kot_srv_system_callback_thread) kot_Srv_System_Initialize();
    
    kot_thread_t self = kot_Sys_GetThread();

    uint64_t TargetShareKey = NULL;
    kot_Sys_Keyhole_CloneModify(Target, &TargetShareKey, kot_SystemData->ControllerHeader.Process, KeyholeFlagPresent | KeyholeFlagDataTypeThreadIsEventable, PriviledgeApp);

    struct kot_srv_system_callback_t* callback = (struct kot_srv_system_callback_t*)malloc(sizeof(struct kot_srv_system_callback_t));
    callback->Self = self;
    callback->Data = NULL;
    callback->Size = NULL;
    callback->IsAwait = IsAwait;
    callback->Status = KBUSY;
    callback->Handler = &Srv_System_BindIRQLine_Callback;

    struct kot_arguments_t parameters;
    parameters.arg[0] = kot_srv_system_callback_thread;
    parameters.arg[1] = (uint64_t)callback;
    parameters.arg[2] = IRQLineNumber;
    parameters.arg[3] = TargetShareKey;
    parameters.arg[4] = IgnoreMissedEvents;
    

    KResult Status = kot_Sys_ExecThread(kot_SystemData->BindIRQLine, &parameters, ExecutionTypeQueu, NULL);
    if(Status == KSUCCESS && IsAwait){
        kot_Sys_Pause(false);
    }
    return callback;
}

/* UnbindIRQLine */
KResult Srv_System_UnbindIRQLine_Callback(KResult Status, struct kot_srv_system_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3){
    return Status;
}

struct kot_srv_system_callback_t* kot_Srv_System_UnbindIRQLine(uint8_t IRQLineNumber, kot_thread_t Target, bool IsAwait){
    if(!kot_srv_system_callback_thread) kot_Srv_System_Initialize();
    
    kot_thread_t self = kot_Sys_GetThread();

    uint64_t TargetShareKey = NULL;
    kot_Sys_Keyhole_CloneModify(Target, &TargetShareKey, kot_SystemData->ControllerHeader.Process, KeyholeFlagPresent | KeyholeFlagDataTypeThreadIsEventable, PriviledgeApp);

    struct kot_srv_system_callback_t* callback = (struct kot_srv_system_callback_t*)malloc(sizeof(struct kot_srv_system_callback_t));
    callback->Self = self;
    callback->Data = NULL;
    callback->Size = NULL;
    callback->IsAwait = IsAwait;
    callback->Status = KBUSY;
    callback->Handler = &Srv_System_UnbindIRQLine_Callback;

    struct kot_arguments_t parameters;
    parameters.arg[0] = kot_srv_system_callback_thread;
    parameters.arg[1] = (uint64_t)callback;
    parameters.arg[2] = IRQLineNumber;
    parameters.arg[3] = TargetShareKey;
    

    KResult Status = kot_Sys_ExecThread(kot_SystemData->UnbindIRQLine, &parameters, ExecutionTypeQueu, NULL);
    if(Status == KSUCCESS && IsAwait){
        kot_Sys_Pause(false);
    }
    return callback;
}

/* BindFreeIRQ */
KResult Srv_System_BindFreeIRQ_Callback(KResult Status, struct kot_srv_system_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3){
    if(Status == KSUCCESS) {
        Callback->Data = GP0;
        Callback->Size = sizeof(uint64_t);
    }
    return Status;
}

struct kot_srv_system_callback_t* kot_Srv_System_BindFreeIRQ(kot_thread_t Target, bool IgnoreMissedEvents, bool IsAwait){
    if(!kot_srv_system_callback_thread) kot_Srv_System_Initialize();
    
    kot_thread_t self = kot_Sys_GetThread();

    uint64_t TargetShareKey = NULL;
    kot_Sys_Keyhole_CloneModify(Target, &TargetShareKey, kot_SystemData->ControllerHeader.Process, KeyholeFlagPresent | KeyholeFlagDataTypeThreadIsEventable, PriviledgeApp);

    struct kot_srv_system_callback_t* callback = (struct kot_srv_system_callback_t*)malloc(sizeof(struct kot_srv_system_callback_t));
    callback->Self = self;
    callback->Data = NULL;
    callback->Size = NULL;
    callback->IsAwait = IsAwait;
    callback->Status = KBUSY;
    callback->Handler = &Srv_System_BindFreeIRQ_Callback;

    struct kot_arguments_t parameters;
    parameters.arg[0] = kot_srv_system_callback_thread;
    parameters.arg[1] = (uint64_t)callback;
    parameters.arg[2] = TargetShareKey;
    parameters.arg[3] = IgnoreMissedEvents;
    

    KResult Status = kot_Sys_ExecThread(kot_SystemData->BindFreeIRQ, &parameters, ExecutionTypeQueu, NULL);
    if(Status == KSUCCESS && IsAwait){
        kot_Sys_Pause(false);
    }
    return callback;
}

/* UnbindIRQ */
KResult Srv_System_UnbindIRQ_Callback(KResult Status, struct kot_srv_system_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3){
    if(Status == KSUCCESS) {
        Callback->Data = GP0;
        Callback->Size = sizeof(uint64_t);
    }
    return Status;
}

struct kot_srv_system_callback_t* kot_Srv_System_UnbindIRQ(uint8_t Vector, kot_thread_t Target, bool IsAwait){
    if(!kot_srv_system_callback_thread) kot_Srv_System_Initialize();
    
    kot_thread_t self = kot_Sys_GetThread();

    uint64_t TargetShareKey = NULL;
    kot_Sys_Keyhole_CloneModify(Target, &TargetShareKey, kot_SystemData->ControllerHeader.Process, KeyholeFlagPresent | KeyholeFlagDataTypeThreadIsEventable, PriviledgeApp);

    struct kot_srv_system_callback_t* callback = (struct kot_srv_system_callback_t*)malloc(sizeof(struct kot_srv_system_callback_t));
    callback->Self = self;
    callback->Data = NULL;
    callback->Size = NULL;
    callback->IsAwait = IsAwait;
    callback->Status = KBUSY;
    callback->Handler = &Srv_System_BindFreeIRQ_Callback;

    struct kot_arguments_t parameters;
    parameters.arg[0] = kot_srv_system_callback_thread;
    parameters.arg[1] = (uint64_t)callback;
    parameters.arg[2] = TargetShareKey;
    parameters.arg[3] = Vector;
    

    KResult Status = kot_Sys_ExecThread(kot_SystemData->UnbindIRQ, &parameters, ExecutionTypeQueu, NULL);
    if(Status == KSUCCESS && IsAwait){
        kot_Sys_Pause(false);
    }
    return callback;
}

}