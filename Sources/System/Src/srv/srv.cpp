#include <srv/srv.h>

struct SrvInfo_t* SrvInfo;
kot_process_t ShareProcess;

void InitializeSrv(struct KernelInfo* kernelInfo){
    void* address = kot_GetFreeAlignedSpace(sizeof(kot_uisd_system_t));
    kot_key_mem_t key = NULL;
    kot_Sys_CreateMemoryField(proc, sizeof(kot_uisd_system_t), &address, &key, MemoryFieldTypeShareSpaceRO);

    ShareProcess = kot_ShareProcessKey(proc);

    kot_uisd_system_t* SystemSrv = (kot_uisd_system_t*)address;
    SystemSrv->ControllerHeader.IsReadWrite = false;
    SystemSrv->ControllerHeader.Version = System_Srv_Version;
    SystemSrv->ControllerHeader.VendorID = Kot_VendorID;
    SystemSrv->ControllerHeader.Type = ControllerTypeEnum_System;
    SystemSrv->ControllerHeader.Process = ShareProcess;

    /* Setup threads */

    /* LoadExecutable */
    kot_thread_t LoadExecutableThread = NULL;
    kot_Sys_CreateThread(proc, (void*)&LoadExecutable, PriviledgeApp, NULL, &LoadExecutableThread);
    SystemSrv->LoadExecutable = kot_MakeShareableThread(LoadExecutableThread, PriviledgeApp);

    /* LoadExecutableToProcess */
    kot_thread_t LoadExecutableToProcessThread = NULL;
    kot_Sys_CreateThread(proc, (void*)&LoadExecutableToProcess, PriviledgeApp, NULL, &LoadExecutableToProcessThread);
    SystemSrv->LoadExecutableToProcess = kot_MakeShareableThread(LoadExecutableToProcessThread, PriviledgeApp);

    /* GetFramebuffer */
    kot_thread_t GetFramebufferThread = NULL;
    kot_Sys_CreateThread(proc, (void*)&GetFramebuffer, PriviledgeApp, NULL, &GetFramebufferThread);
    SystemSrv->GetFramebuffer = kot_MakeShareableThread(GetFramebufferThread, PriviledgeService);

    /* ReadFileInitrd */
    kot_thread_t ReadFileFromInitrdThread = NULL;
    kot_Sys_CreateThread(proc, (void*)&ReadFileFromInitrd, PriviledgeApp, NULL, &ReadFileFromInitrdThread);
    SystemSrv->ReadFileInitrd = kot_MakeShareableThread(ReadFileFromInitrdThread, PriviledgeService);

    /* GetTableInRootSystemDescription */
    kot_thread_t GetTableInRootSystemDescriptionThread = NULL;
    kot_Sys_CreateThread(proc, (void*)&GetTableInRootSystemDescription, PriviledgeApp, NULL, &GetTableInRootSystemDescriptionThread);
    SystemSrv->GetTableInRootSystemDescription = kot_MakeShareableThread(GetTableInRootSystemDescriptionThread, PriviledgeDriver);

    /* GetSystemManagementBIOSTable */
    kot_thread_t GetSystemManagementBIOSTableThread = NULL;
    kot_Sys_CreateThread(proc, (void*)&GetSystemManagementBIOSTable, PriviledgeApp, NULL, &GetSystemManagementBIOSTableThread);
    SystemSrv->GetSystemManagementBIOSTable = kot_MakeShareableThread(GetSystemManagementBIOSTableThread, PriviledgeDriver);

    /* BindIRQLine */
    kot_thread_t BindIRQLineThread = NULL;
    kot_Sys_CreateThread(proc, (void*)&BindIRQLine, PriviledgeApp, NULL, &BindIRQLineThread);
    SystemSrv->BindIRQLine = kot_MakeShareableThread(BindIRQLineThread, PriviledgeDriver);
    
    /* UnbindIRQLine */
    kot_thread_t UnbindIRQLineThread = NULL;
    kot_Sys_CreateThread(proc, (void*)&UnbindIRQLine, PriviledgeApp, NULL, &UnbindIRQLineThread);
    SystemSrv->UnbindIRQLine = kot_MakeShareableThread(UnbindIRQLineThread, PriviledgeDriver);

    /* BindFreeIRQ */
    kot_thread_t BindFreeIRQThread = NULL;
    kot_Sys_CreateThread(proc, (void*)&BindFreeIRQ, PriviledgeApp, NULL, &BindFreeIRQThread);
    SystemSrv->BindFreeIRQ = kot_MakeShareableThread(BindFreeIRQThread, PriviledgeDriver);

    /* UnbindIRQ */
    kot_thread_t UnbindIRQThread = NULL;
    kot_Sys_CreateThread(proc, (void*)&BindFreeIRQ, PriviledgeApp, NULL, &UnbindIRQThread);
    SystemSrv->UnbindIRQ = kot_MakeShareableThread(UnbindIRQThread, PriviledgeDriver);

    /* Setup data */
    SrvInfo = (SrvInfo_t*)malloc(sizeof(SrvInfo_t));

    SrvInfo->Framebuffer = (kot_srv_system_framebuffer_t*)malloc(sizeof(kot_srv_system_framebuffer_t));
    SrvInfo->Framebuffer->Address = kernelInfo->Framebuffer.framebuffer_base;
    SrvInfo->Framebuffer->Width = kernelInfo->Framebuffer.framebuffer_Width;
    SrvInfo->Framebuffer->Height = kernelInfo->Framebuffer.framebuffer_Height;
    SrvInfo->Framebuffer->Pitch = kernelInfo->Framebuffer.framebuffer_Pitch;
    SrvInfo->Framebuffer->Bpp = kernelInfo->Framebuffer.framebuffer_Bpp;

    SrvInfo->Smbios = kernelInfo->Smbios;

    SrvInfo->Rsdp = kernelInfo->Rsdp;

    SrvInfo->IRQLineStart = kernelInfo->IRQLineStart;
    SrvInfo->IRQLineSize = kernelInfo->IRQLineSize;

    SrvInfo->IRQSize = kernelInfo->IRQSize;
    SrvInfo->IRQEvents = (kot_event_t*)malloc(sizeof(kot_event_t) * kernelInfo->IRQSize);
    memcpy(SrvInfo->IRQEvents, &kernelInfo->IRQEvents, sizeof(kot_event_t) * kernelInfo->IRQSize);    
    SrvInfo->IsIRQEventsFree = IsIRQEventsFree;

    kot_CreateControllerUISD(ControllerTypeEnum_System, key, true);
}

KResult LoadExecutable(kot_thread_t Callback, uint64_t CallbackArg, kot_process_t Process, uint64_t Priviledge, char* Path){
    // Load filesystem handler
    if(!KotSpecificData.VFSHandler){
        kot_srv_storage_callback_t* Callback = kot_Srv_Storage_VFSLoginApp(ShareProcess, FS_AUTHORIZATION_HIGH, Storage_Permissions_Admin | Storage_Permissions_Read | Storage_Permissions_Write | Storage_Permissions_Create, "d0:", true);
        KotSpecificData.VFSHandler = Callback->Data;
        free(Callback);
    }

    KResult Status = KFAIL;
    kot_thread_t ThreadOutput;
    if(Priviledge >= kot_Sys_GetPriviledgeThreadLauncher()){
        kot_thread_t Thread;
        FILE* ExecutableFile = fopen(Path, "r");
        if(ExecutableFile){
            fseek(ExecutableFile, 0, SEEK_END);
            size_t ExecutableFileSize = ftell(ExecutableFile);
            fseek(ExecutableFile, 0, SEEK_SET);

            void* BufferExecutable = malloc(ExecutableFileSize);
            fread(BufferExecutable, ExecutableFileSize, 1, ExecutableFile);
            Status = ELF::loadElf((void*)BufferExecutable, NULL, (enum kot_Priviledge)Priviledge, NULL, &Thread, dirname(Path), true);
            free(BufferExecutable);
            ThreadOutput = kot_MakeShareableThreadToProcess(Thread, Process);
        }
    }



    kot_arguments_t arguments{
        .arg[0] = Status,           /* Status */
        .arg[1] = CallbackArg,      /* CallbackArg */
        .arg[2] = ThreadOutput,     /* ThreadOutput */
        .arg[3] = NULL,             /* GP1 */
        .arg[4] = NULL,             /* GP2 */
        .arg[5] = NULL,             /* GP3 */
    };

    kot_Sys_ExecThread(Callback, &arguments, ExecutionTypeQueu, NULL);
    kot_Sys_Close(KSUCCESS);
}

KResult LoadExecutableToProcess(kot_thread_t Callback, uint64_t CallbackArg, kot_process_t Process, void* Data, size64_t Size){
    // Load filesystem handler
    if(!KotSpecificData.VFSHandler){
        kot_srv_storage_callback_t* Callback = kot_Srv_Storage_VFSLoginApp(ShareProcess, FS_AUTHORIZATION_HIGH, Storage_Permissions_Admin | Storage_Permissions_Read | Storage_Permissions_Write | Storage_Permissions_Create, "d0:", true);
        KotSpecificData.VFSHandler = Callback->Data;
        free(Callback);
    }

    KResult Status = KFAIL;
    char* Path = (char*)((uintptr_t)Data + Size);
    kot_thread_t Thread;
    FILE* ExecutableFile = fopen(Path, "r");
    if(ExecutableFile){
        fseek(ExecutableFile, 0, SEEK_END);
        size_t ExecutableFileSize = ftell(ExecutableFile);
        fseek(ExecutableFile, 0, SEEK_SET);

        void* BufferExecutable = malloc(ExecutableFileSize);
        fread(BufferExecutable, ExecutableFileSize, 1, ExecutableFile);
        Status = ELF::loadElf((void*)BufferExecutable, Process, (enum kot_Priviledge)0, NULL, &Thread, dirname(Path), true);
        free(BufferExecutable);

        kot_ShareDataWithArguments_t DataArguments{
            .Data = Data,
            .Size = Size,
            .ParameterPosition = 0x0,
        };

        kot_arguments_t arguments;
        kot_Sys_ExecThread(Thread, &arguments, ExecutionTypeQueu, &DataArguments) == KSUCCESS;
    }

    kot_Sys_Close(KSUCCESS);
}

KResult GetFramebuffer(kot_thread_t Callback, uint64_t CallbackArg){
    kot_ShareDataWithArguments_t data{
        .Data = (void*)SrvInfo->Framebuffer,
        .Size = sizeof(kot_srv_system_framebuffer_t),
        .ParameterPosition = 0x2, 
    };

    kot_arguments_t arguments{
        .arg[0] = KSUCCESS,         /* Status */
        .arg[1] = CallbackArg,      /* CallbackArg */
        .arg[2] = NULL,             /* GP0 */
        .arg[3] = NULL,             /* GP1 */
        .arg[4] = NULL,             /* GP2 */
        .arg[5] = NULL,             /* GP3 */
    };

    kot_Sys_ExecThread(Callback, &arguments, ExecutionTypeQueu, &data);
    kot_Sys_Close(KSUCCESS);
}

KResult ReadFileFromInitrd(kot_thread_t Callback, uint64_t CallbackArg, char* Name){
    if(Name != NULL){
        initrd::InitrdFile* file = initrd::Find(Name);
        void* fileData = NULL; 
        if(file != NULL){
            fileData = initrd::Read(file);
        }else{
            kot_arguments_t arguments{
                .arg[0] = KFAIL,            /* Status */
                .arg[1] = CallbackArg,      /* CallbackArg */
                .arg[2] = NULL,             /* Size */
                .arg[3] = NULL,             /* Data */
                .arg[4] = NULL,             /* GP2 */
                .arg[5] = NULL,             /* GP3 */
            };
            kot_Sys_ExecThread(Callback, &arguments, ExecutionTypeQueu, NULL);
            kot_Sys_Close(KFAIL);
        }

        kot_ShareDataWithArguments_t data{
            .Data = fileData,
            .Size = file->size,
            .ParameterPosition = 0x3, 
        };

        kot_arguments_t arguments{
            .arg[0] = KSUCCESS,         /* Status */
            .arg[1] = CallbackArg,      /* CallbackArg */
            .arg[2] = file->size,       /* Size */
            .arg[3] = NULL,             /* Data */
            .arg[4] = NULL,             /* GP2 */
            .arg[5] = NULL,             /* GP3 */
        };
        
        kot_Sys_ExecThread(Callback, &arguments, ExecutionTypeQueu, &data);
        kot_Sys_Close(KSUCCESS);
    }else{
        kot_arguments_t arguments{
            .arg[0] = KFAIL,            /* Status */
            .arg[1] = CallbackArg,      /* CallbackArg */
            .arg[2] = NULL,             /* Size */
            .arg[3] = NULL,             /* Data */
            .arg[4] = NULL,             /* GP2 */
            .arg[5] = NULL,             /* GP3 */
        };
        kot_Sys_ExecThread(Callback, &arguments, ExecutionTypeQueu, NULL);
        kot_Sys_Close(KFAIL);        
    }
}

KResult GetTableInRootSystemDescription(kot_thread_t Callback, uint64_t CallbackArg, char* Name){
    if(Name != NULL){
        uint64_t tableIndex = FindTableIndex(Name);
        void* tableAddress = NULL;
        size64_t tableSize = NULL;

        KResult status = KFAIL;

        if(tableIndex != NULL){
            status = KSUCCESS;
            tableAddress = GetTablePhysicalAddress(tableIndex);
            tableSize = GetTableSize(tableIndex);
        }

        kot_arguments_t arguments{
            .arg[0] = status,                           /* Status */
            .arg[1] = CallbackArg,                      /* CallbackArg */
            .arg[2] = (uint64_t)tableAddress,           /* TableAddress */
            .arg[3] = (uint64_t)tableSize,              /* TableSize */
            .arg[4] = NULL,                             /* GP2 */
            .arg[5] = NULL,                             /* GP3 */
        };

        kot_Sys_ExecThread(Callback, &arguments, ExecutionTypeQueu, NULL);
        kot_Sys_Close(KSUCCESS);
    }else{
        kot_arguments_t arguments{
            .arg[0] = KFAIL,            /* Status */
            .arg[1] = CallbackArg,      /* CallbackArg */
            .arg[2] = NULL,             /* Size */
            .arg[3] = NULL,             /* Data */
            .arg[4] = NULL,             /* GP2 */
            .arg[5] = NULL,             /* GP3 */
        };
        kot_Sys_ExecThread(Callback, &arguments, ExecutionTypeQueu, NULL);
        kot_Sys_Close(KFAIL);
    }
}

KResult GetSystemManagementBIOSTable(kot_thread_t Callback, uint64_t CallbackArg){
    kot_arguments_t arguments{
        .arg[0] = KSUCCESS,                     /* Status */
        .arg[1] = CallbackArg,                  /* CallbackArg */
        .arg[2] = (uint64_t)SrvInfo->Smbios,    /* Smbios physical address */
        .arg[3] = NULL,                         /* GP1 */
        .arg[4] = NULL,                         /* GP2 */
        .arg[5] = NULL,                         /* GP3 */
    };

    kot_Sys_ExecThread(Callback, &arguments, ExecutionTypeQueu, NULL);
    kot_Sys_Close(KSUCCESS);
}

KResult BindIRQLine(kot_thread_t Callback, uint64_t CallbackArg, uint8_t IRQLineNumber, kot_thread_t Target, bool IgnoreMissedEvents){
    KResult Status = KFAIL;
    if(IRQLineNumber < SrvInfo->IRQLineSize){
        uint8_t vector = SrvInfo->IRQLineStart + IRQLineNumber;
        Status = kot_Sys_Event_Bind(SrvInfo->IRQEvents[vector], Target, IgnoreMissedEvents);
    }
    
    kot_arguments_t arguments{
        .arg[0] = Status,            /* Status */
        .arg[1] = CallbackArg,      /* CallbackArg */
        .arg[2] = NULL,             /* GP0 */
        .arg[3] = NULL,             /* GP1 */
        .arg[4] = NULL,             /* GP2 */
        .arg[5] = NULL,             /* GP3 */
    };

    kot_Sys_ExecThread(Callback, &arguments, ExecutionTypeQueu, NULL);
    kot_Sys_Close(KSUCCESS);
}

KResult UnbindIRQLine(kot_thread_t Callback, uint64_t CallbackArg, uint8_t IRQLineNumber, kot_thread_t Target){
    KResult Status = KFAIL;
    if(IRQLineNumber < SrvInfo->IRQLineSize){
        uint8_t vector = SrvInfo->IRQLineStart + IRQLineNumber;
        Status = kot_Sys_Event_Unbind(SrvInfo->IRQEvents[vector], Target);
    }
    
    kot_arguments_t arguments{
        .arg[0] = Status,            /* Status */
        .arg[1] = CallbackArg,      /* CallbackArg */
        .arg[2] = NULL,             /* GP0 */
        .arg[3] = NULL,             /* GP1 */
        .arg[4] = NULL,             /* GP2 */
        .arg[5] = NULL,             /* GP3 */
    };

    kot_Sys_ExecThread(Callback, &arguments, ExecutionTypeQueu, NULL);
    kot_Sys_Close(KSUCCESS);
}

KResult BindFreeIRQ(kot_thread_t Callback, uint64_t CallbackArg, kot_thread_t Target, bool IgnoreMissedEvents){
    kot_event_t Vector = NULL;
    for(size64_t i = 0; i < SrvInfo->IRQSize; i++){
        if(SrvInfo->IsIRQEventsFree[i]){
            Vector = i;
            break;
        }
    }
    KResult Status = KFAIL;
    if(Vector != NULL){
        Status = kot_Sys_Event_Bind(SrvInfo->IRQEvents[Vector], Target, IgnoreMissedEvents);
    }
    kot_arguments_t arguments{
        .arg[0] = KSUCCESS,         /* Status */
        .arg[1] = CallbackArg,      /* CallbackArg */
        .arg[2] = Vector,           /* IRQNumber */
        .arg[3] = NULL,             /* GP1 */
        .arg[4] = NULL,             /* GP2 */
        .arg[5] = NULL,             /* GP3 */
    };

    kot_Sys_ExecThread(Callback, &arguments, ExecutionTypeQueu, NULL);
    kot_Sys_Close(KSUCCESS);
}

KResult UnbindIRQ(kot_thread_t Callback, uint64_t CallbackArg, kot_thread_t Target, uint8_t Vector){
    KResult Status = KFAIL;
    if(Vector > SrvInfo->IRQLineStart + SrvInfo->IRQLineSize && Vector < SrvInfo->IRQSize){
        Status = kot_Sys_Event_Unbind(SrvInfo->IRQEvents[Vector], Target);
    }
    kot_arguments_t arguments{
        .arg[0] = KSUCCESS,         /* Status */
        .arg[1] = CallbackArg,      /* CallbackArg */
        .arg[2] = NULL,             /* GP0 */
        .arg[3] = NULL,             /* GP1 */
        .arg[4] = NULL,             /* GP2 */
        .arg[5] = NULL,             /* GP3 */
    };

    kot_Sys_ExecThread(Callback, &arguments, ExecutionTypeQueu, NULL);
    kot_Sys_Close(KSUCCESS);
}