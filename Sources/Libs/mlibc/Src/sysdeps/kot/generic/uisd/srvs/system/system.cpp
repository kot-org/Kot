#include <kot/uisd/srvs/system.h>
#include <stdlib.h>
#include <string.h>

namespace Kot{
    kot_thread_t srv_system_callback_thread = NULL;
    uisd_system_t* SystemData = NULL;
    kot_process_t ShareableProcessUISDSystem;

    void Srv_System_Initialize(){
        SystemData = (uisd_system_t*)FindControllerUISD(ControllerTypeEnum_System);
        if(SystemData != NULL){
            kot_process_t Proc = Sys_GetProcess();
            ShareableProcessUISDSystem = ShareProcessKey(Proc);

            kot_thread_t SystemthreadKeyCallback = NULL;
            Sys_CreateThread(Proc, (uintptr_t)&Srv_System_Callback, PriviledgeMax, NULL, &SystemthreadKeyCallback);
            InitializeThread(SystemthreadKeyCallback);
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

    /* LoadExecutable */
    KResult Srv_System_LoadExecutable_Callback(KResult Status, struct srv_system_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3){
        if(Status == KSUCCESS){
            Callback->Data = GP0;
            Callback->Size = (size64_t)sizeof(kot_thread_t);
        }
        return Status;
    }

    struct srv_system_callback_t* Srv_System_LoadExecutable(uint64_t Priviledge, char* Path, bool IsAwait){
        if(!srv_system_callback_thread) Srv_System_Initialize();
        
        kot_thread_t self = Sys_Getthread();

        struct srv_system_callback_t* callback = (struct srv_system_callback_t*)malloc(sizeof(struct srv_system_callback_t));
        callback->Self = self;
        callback->Data = NULL;
        callback->Size = NULL;
        callback->IsAwait = IsAwait;
        callback->Status = KBUSY;
        callback->Handler = &Srv_System_LoadExecutable_Callback;

        struct kot_arguments_t parameters;
        parameters.arg[0] = srv_system_callback_thread;
        parameters.arg[1] = (uint64_t)callback;
        parameters.arg[2] = ShareableProcessUISDSystem;
        parameters.arg[3] = Priviledge;

        struct ShareDataWithArguments_t data;

        if(Path != NULL){
            data.Data = (uintptr_t)Path;
            data.Size = strlen(Path) + 1; // add '\0' char
            data.ParameterPosition = 0x4; 
        }
        

        KResult Status = Sys_ExecThread(SystemData->LoadExecutable, &parameters, ExecutionTypeQueu, &data);
        if(Status == KSUCCESS && IsAwait){
            Sys_Pause(false);
        }
        return callback;
    }

    /* GetFrameBufer */
    KResult Srv_System_GetFramebuffer_Callback(KResult Status, struct srv_system_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3){
        if(Status == KSUCCESS){
            Callback->Data = (uint64_t)malloc(sizeof(srv_system_framebuffer_t));
            memcpy((void*)Callback->Data, (void*)GP0, sizeof(srv_system_framebuffer_t));
            Callback->Size = (size64_t)sizeof(srv_system_framebuffer_t);
        }
        return Status;
    }

    struct srv_system_callback_t* Srv_System_GetFramebuffer(bool IsAwait){
        if(!srv_system_callback_thread) Srv_System_Initialize();
        
        kot_thread_t self = Sys_Getthread();

        struct srv_system_callback_t* callback = (struct srv_system_callback_t*)malloc(sizeof(struct srv_system_callback_t));
        callback->Self = self;
        callback->Data = NULL;
        callback->Size = NULL;
        callback->IsAwait = IsAwait;
        callback->Status = KBUSY;
        callback->Handler = &Srv_System_GetFramebuffer_Callback;

        struct kot_arguments_t parameters;
        parameters.arg[0] = srv_system_callback_thread;
        parameters.arg[1] = (uint64_t)callback;
        

        KResult Status = Sys_ExecThread(SystemData->GetFramebuffer, &parameters, ExecutionTypeQueu, NULL);
        if(Status == KSUCCESS && IsAwait){
            Sys_Pause(false);
        }
        return callback;
    }

    /* ReadFile */
    KResult Srv_System_ReadFileInitrd_Callback(KResult Status, struct srv_system_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3){
        if(Status == KSUCCESS){
            Callback->Data = (uint64_t)malloc((size64_t)GP0);
            memcpy((void*)Callback->Data, (void*)GP1, (size64_t)GP0);
            Callback->Size = (size64_t)GP0;
        }
        return Status;
    }

    struct srv_system_callback_t* Srv_System_ReadFileInitrd(char* Name,  bool IsAwait){
        if(!srv_system_callback_thread) Srv_System_Initialize();
        
        kot_thread_t self = Sys_Getthread();

        struct srv_system_callback_t* callback = (struct srv_system_callback_t*)malloc(sizeof(struct srv_system_callback_t));
        callback->Self = self;
        callback->Data = NULL;
        callback->Size = NULL;
        callback->IsAwait = IsAwait;
        callback->Status = KBUSY;
        callback->Handler = &Srv_System_ReadFileInitrd_Callback;

        struct ShareDataWithArguments_t data;
        if(Name != NULL){
            data.Data = (uintptr_t)Name;
            data.Size = strlen(Name) + 1; // add '\0' char
            data.ParameterPosition = 0x2; 
        }

        struct kot_arguments_t parameters;
        parameters.arg[0] = srv_system_callback_thread;
        parameters.arg[1] = (uint64_t)callback;

        KResult Status = Sys_ExecThread(SystemData->ReadFileInitrd, &parameters, ExecutionTypeQueu, &data);
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
        
        kot_thread_t self = Sys_Getthread();

        struct srv_system_callback_t* callback = (struct srv_system_callback_t*)malloc(sizeof(struct srv_system_callback_t));
        callback->Self = self;
        callback->Data = NULL;
        callback->Size = NULL;
        callback->IsAwait = IsAwait;
        callback->Status = KBUSY;
        callback->Handler = &Srv_System_GetTableInRootSystemDescription_Callback;

        struct ShareDataWithArguments_t data;
        if(Name != NULL){
            data.Data = (uintptr_t)Name;
            data.Size = strlen(Name) + 1; // add '\0' char
            data.ParameterPosition = 0x2; 
        }

        struct kot_arguments_t parameters;
        parameters.arg[0] = srv_system_callback_thread;
        parameters.arg[1] = (uint64_t)callback;

        KResult Status = Sys_ExecThread(SystemData->GetTableInRootSystemDescription, &parameters, ExecutionTypeQueu, &data);
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
        
        kot_thread_t self = Sys_Getthread();

        struct srv_system_callback_t* callback = (struct srv_system_callback_t*)malloc(sizeof(struct srv_system_callback_t));
        callback->Self = self;
        callback->Data = NULL;
        callback->Size = NULL;
        callback->IsAwait = IsAwait;
        callback->Status = KBUSY;
        callback->Handler = &Srv_System_GetSystemManagementBIOSTable_Callback;

        struct kot_arguments_t parameters;
        parameters.arg[0] = srv_system_callback_thread;
        parameters.arg[1] = (uint64_t)callback;
        

        KResult Status = Sys_ExecThread(SystemData->GetSystemManagementBIOSTable, &parameters, ExecutionTypeQueu, NULL);
        if(Status == KSUCCESS && IsAwait){
            Sys_Pause(false);
        }
        return callback;
    }

    /* BindIRQLine */
    KResult Srv_System_BindIRQLine_Callback(KResult Status, struct srv_system_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3){
        return Status;
    }

    struct srv_system_callback_t* Srv_System_BindIRQLine(uint8_t IRQLineNumber, kot_thread_t Target, bool IgnoreMissedEvents, bool IsAwait){
        if(!srv_system_callback_thread) Srv_System_Initialize();
        
        kot_thread_t self = Sys_Getthread();

        uint64_t TargetShareKey = NULL;
        Sys_Keyhole_CloneModify(Target, &TargetShareKey, SystemData->ControllerHeader.Process, KeyholeFlagPresent | KeyholeFlagDataTypeThreadIsEventable, PriviledgeApp);

        struct srv_system_callback_t* callback = (struct srv_system_callback_t*)malloc(sizeof(struct srv_system_callback_t));
        callback->Self = self;
        callback->Data = NULL;
        callback->Size = NULL;
        callback->IsAwait = IsAwait;
        callback->Status = KBUSY;
        callback->Handler = &Srv_System_BindIRQLine_Callback;

        struct kot_arguments_t parameters;
        parameters.arg[0] = srv_system_callback_thread;
        parameters.arg[1] = (uint64_t)callback;
        parameters.arg[2] = IRQLineNumber;
        parameters.arg[3] = TargetShareKey;
        parameters.arg[4] = IgnoreMissedEvents;
        

        KResult Status = Sys_ExecThread(SystemData->BindIRQLine, &parameters, ExecutionTypeQueu, NULL);
        if(Status == KSUCCESS && IsAwait){
            Sys_Pause(false);
        }
        return callback;
    }

    /* UnbindIRQLine */
    KResult Srv_System_UnbindIRQLine_Callback(KResult Status, struct srv_system_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3){
        return Status;
    }

    struct srv_system_callback_t* Srv_System_UnbindIRQLine(uint8_t IRQLineNumber, kot_thread_t Target, bool IsAwait){
        if(!srv_system_callback_thread) Srv_System_Initialize();
        
        kot_thread_t self = Sys_Getthread();

        uint64_t TargetShareKey = NULL;
        Sys_Keyhole_CloneModify(Target, &TargetShareKey, SystemData->ControllerHeader.Process, KeyholeFlagPresent | KeyholeFlagDataTypeThreadIsEventable, PriviledgeApp);

        struct srv_system_callback_t* callback = (struct srv_system_callback_t*)malloc(sizeof(struct srv_system_callback_t));
        callback->Self = self;
        callback->Data = NULL;
        callback->Size = NULL;
        callback->IsAwait = IsAwait;
        callback->Status = KBUSY;
        callback->Handler = &Srv_System_UnbindIRQLine_Callback;

        struct kot_arguments_t parameters;
        parameters.arg[0] = srv_system_callback_thread;
        parameters.arg[1] = (uint64_t)callback;
        parameters.arg[2] = IRQLineNumber;
        parameters.arg[3] = TargetShareKey;
        

        KResult Status = Sys_ExecThread(SystemData->UnbindIRQLine, &parameters, ExecutionTypeQueu, NULL);
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

    struct srv_system_callback_t* Srv_System_BindFreeIRQ(kot_thread_t Target, bool IgnoreMissedEvents, bool IsAwait){
        if(!srv_system_callback_thread) Srv_System_Initialize();
        
        kot_thread_t self = Sys_Getthread();

        uint64_t TargetShareKey = NULL;
        Sys_Keyhole_CloneModify(Target, &TargetShareKey, SystemData->ControllerHeader.Process, KeyholeFlagPresent | KeyholeFlagDataTypeThreadIsEventable, PriviledgeApp);

        struct srv_system_callback_t* callback = (struct srv_system_callback_t*)malloc(sizeof(struct srv_system_callback_t));
        callback->Self = self;
        callback->Data = NULL;
        callback->Size = NULL;
        callback->IsAwait = IsAwait;
        callback->Status = KBUSY;
        callback->Handler = &Srv_System_BindFreeIRQ_Callback;

        struct kot_arguments_t parameters;
        parameters.arg[0] = srv_system_callback_thread;
        parameters.arg[1] = (uint64_t)callback;
        parameters.arg[2] = TargetShareKey;
        parameters.arg[3] = IgnoreMissedEvents;
        

        KResult Status = Sys_ExecThread(SystemData->BindFreeIRQ, &parameters, ExecutionTypeQueu, NULL);
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

    struct srv_system_callback_t* Srv_System_UnbindIRQ(uint8_t Vector, kot_thread_t Target, bool IsAwait){
        if(!srv_system_callback_thread) Srv_System_Initialize();
        
        kot_thread_t self = Sys_Getthread();

        uint64_t TargetShareKey = NULL;
        Sys_Keyhole_CloneModify(Target, &TargetShareKey, SystemData->ControllerHeader.Process, KeyholeFlagPresent | KeyholeFlagDataTypeThreadIsEventable, PriviledgeApp);

        struct srv_system_callback_t* callback = (struct srv_system_callback_t*)malloc(sizeof(struct srv_system_callback_t));
        callback->Self = self;
        callback->Data = NULL;
        callback->Size = NULL;
        callback->IsAwait = IsAwait;
        callback->Status = KBUSY;
        callback->Handler = &Srv_System_BindFreeIRQ_Callback;

        struct kot_arguments_t parameters;
        parameters.arg[0] = srv_system_callback_thread;
        parameters.arg[1] = (uint64_t)callback;
        parameters.arg[2] = TargetShareKey;
        parameters.arg[3] = Vector;
        

        KResult Status = Sys_ExecThread(SystemData->UnbindIRQ, &parameters, ExecutionTypeQueu, NULL);
        if(Status == KSUCCESS && IsAwait){
            Sys_Pause(false);
        }
        return callback;
    }

}