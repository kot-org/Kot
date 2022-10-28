#include <srv/srv.h>

struct SrvInfo_t* SrvInfo;

void InitializeSrv(struct KernelInfo* kernelInfo){
    uintptr_t address = getFreeAlignedSpace(sizeof(uisd_system_t));
    ksmem_t key = NULL;
    Sys_CreateMemoryField(proc, sizeof(uisd_system_t), &address, &key, MemoryFieldTypeShareSpaceRO);

    uisd_system_t* SystemSrv = (uisd_system_t*)address;
    SystemSrv->ControllerHeader.IsReadWrite = false;
    SystemSrv->ControllerHeader.Version = System_Srv_Version;
    SystemSrv->ControllerHeader.VendorID = Kot_VendorID;
    SystemSrv->ControllerHeader.Type = ControllerTypeEnum_System;
    SystemSrv->ControllerHeader.Process = ShareProcessKey(proc);

    /* Setup threads */

    /* GetFramebuffer */
    thread_t GetFrameBufferThread = NULL;
    Sys_Createthread(proc, (uintptr_t)&GetFrameBuffer, PriviledgeApp, NULL, &GetFrameBufferThread);
    SystemSrv->GetFramebuffer = MakeShareableThread(GetFrameBufferThread, PriviledgeService);

    /* ReadFileInitrd */
    thread_t ReadFileFromInitrdThread = NULL;
    Sys_Createthread(proc, (uintptr_t)&ReadFileFromInitrd, PriviledgeApp, NULL, &ReadFileFromInitrdThread);
    SystemSrv->ReadFileInitrd = MakeShareableThread(ReadFileFromInitrdThread, PriviledgeService);

    /* GetTableInRootSystemDescription */
    thread_t GetTableInRootSystemDescriptionThread = NULL;
    Sys_Createthread(proc, (uintptr_t)&GetTableInRootSystemDescription, PriviledgeApp, NULL, &GetTableInRootSystemDescriptionThread);
    SystemSrv->GetTableInRootSystemDescription = MakeShareableThread(GetTableInRootSystemDescriptionThread, PriviledgeDriver);

    /* GetSystemManagementBIOSTable */
    thread_t GetSystemManagementBIOSTableThread = NULL;
    Sys_Createthread(proc, (uintptr_t)&GetSystemManagementBIOSTable, PriviledgeApp, NULL, &GetSystemManagementBIOSTableThread);
    SystemSrv->GetSystemManagementBIOSTable = MakeShareableThread(GetSystemManagementBIOSTableThread, PriviledgeDriver);

    /* BindIRQLine */
    thread_t BindIRQLineThread = NULL;
    Sys_Createthread(proc, (uintptr_t)&BindIRQLine, PriviledgeApp, NULL, &BindIRQLineThread);
    SystemSrv->BindIRQLine = MakeShareableThread(BindIRQLineThread, PriviledgeDriver);
    
    /* UnbindIRQLine */
    thread_t UnbindIRQLineThread = NULL;
    Sys_Createthread(proc, (uintptr_t)&UnbindIRQLine, PriviledgeApp, NULL, &UnbindIRQLineThread);
    SystemSrv->UnbindIRQLine = MakeShareableThread(UnbindIRQLineThread, PriviledgeDriver);

    /* BindFreeIRQ */
    thread_t BindFreeIRQThread = NULL;
    Sys_Createthread(proc, (uintptr_t)&BindFreeIRQ, PriviledgeApp, NULL, &BindFreeIRQThread);
    SystemSrv->BindFreeIRQ = MakeShareableThread(BindFreeIRQThread, PriviledgeDriver);

    /* UnbindIRQ */
    thread_t UnbindIRQThread = NULL;
    Sys_Createthread(proc, (uintptr_t)&BindFreeIRQ, PriviledgeApp, NULL, &UnbindIRQThread);
    SystemSrv->UnbindIRQ = MakeShareableThread(UnbindIRQThread, PriviledgeDriver);

    /* Setup data */
    SrvInfo = (SrvInfo_t*)malloc(sizeof(SrvInfo_t));

    SrvInfo->Framebuffer = (srv_system_framebuffer_t*)malloc(sizeof(srv_system_framebuffer_t));
    SrvInfo->Framebuffer->Address = kernelInfo->Framebuffer.framebuffer_base;
    SrvInfo->Framebuffer->Width = kernelInfo->Framebuffer.framebuffer_width;
    SrvInfo->Framebuffer->Height = kernelInfo->Framebuffer.framebuffer_height;
    SrvInfo->Framebuffer->Pitch = kernelInfo->Framebuffer.framebuffer_pitch;
    SrvInfo->Framebuffer->Bpp = kernelInfo->Framebuffer.framebuffer_bpp;

    SrvInfo->Smbios = kernelInfo->Smbios;

    SrvInfo->Rsdp = kernelInfo->Rsdp;

    SrvInfo->IRQLineStart = kernelInfo->IRQLineStart;
    SrvInfo->IRQLineSize = kernelInfo->IRQLineSize;

    SrvInfo->IRQSize = kernelInfo->IRQSize;
    SrvInfo->IRQEvents = (event_t*)malloc(sizeof(event_t) * kernelInfo->IRQSize);
    memcpy(SrvInfo->IRQEvents, &kernelInfo->IRQEvents, sizeof(event_t) * kernelInfo->IRQSize);    
    SrvInfo->IsIRQEventsFree = IsIRQEventsFree;

    CreateControllerUISD(ControllerTypeEnum_System, key, true);
}

KResult GetFrameBuffer(thread_t Callback, uint64_t CallbackArg){
    ShareDataWithArguments_t data{
        .Data = SrvInfo->Framebuffer,
        .Size = sizeof(srv_system_framebuffer_t),
        .ParameterPosition = 0x2, 
    };

    arguments_t arguments{
        .arg[0] = KSUCCESS,         /* Status */
        .arg[1] = CallbackArg,      /* CallbackArg */
        .arg[2] = NULL,             /* GP0 */
        .arg[3] = NULL,             /* GP1 */
        .arg[4] = NULL,             /* GP2 */
        .arg[5] = NULL,             /* GP3 */
    };

    Sys_Execthread(Callback, &arguments, ExecutionTypeQueu, &data);
    Sys_Close(KSUCCESS);
}

KResult ReadFileFromInitrd(thread_t Callback, uint64_t CallbackArg, char* Name){
    if(Name != NULL){
        initrd::File* file = initrd::Find(Name);
        uintptr_t fileData = NULL; 
        if(file != NULL){
            fileData = initrd::Read(file);
        }else{
            arguments_t arguments{
                .arg[0] = KFAIL,            /* Status */
                .arg[1] = CallbackArg,      /* CallbackArg */
                .arg[2] = NULL,             /* Size */
                .arg[3] = NULL,             /* Data */
                .arg[4] = NULL,             /* GP2 */
                .arg[5] = NULL,             /* GP3 */
            };
            Sys_Execthread(Callback, &arguments, ExecutionTypeQueu, NULL);
            Sys_Close(KFAIL);
        }

        ShareDataWithArguments_t data{
            .Data = fileData,
            .Size = file->size,
            .ParameterPosition = 0x3, 
        };

        arguments_t arguments{
            .arg[0] = KSUCCESS,         /* Status */
            .arg[1] = CallbackArg,      /* CallbackArg */
            .arg[2] = file->size,       /* Size */
            .arg[3] = NULL,             /* Data */
            .arg[4] = NULL,             /* GP2 */
            .arg[5] = NULL,             /* GP3 */
        };
        
        Sys_Execthread(Callback, &arguments, ExecutionTypeQueu, &data);
        Sys_Close(KSUCCESS);
    }else{
        arguments_t arguments{
            .arg[0] = KFAIL,            /* Status */
            .arg[1] = CallbackArg,      /* CallbackArg */
            .arg[2] = NULL,             /* Size */
            .arg[3] = NULL,             /* Data */
            .arg[4] = NULL,             /* GP2 */
            .arg[5] = NULL,             /* GP3 */
        };
        Sys_Execthread(Callback, &arguments, ExecutionTypeQueu, NULL);
        Sys_Close(KFAIL);        
    }
}

KResult GetTableInRootSystemDescription(thread_t Callback, uint64_t CallbackArg, char* Name){
    if(Name != NULL){
        uint64_t tableIndex = FindTableIndex(Name);
        uintptr_t tableAddress = NULL;
        size64_t tableSize = NULL;

        KResult status = KFAIL;

        if(tableIndex != NULL){
            status = KSUCCESS;
            tableAddress = GetTablePhysicalAddress(tableIndex);
            tableSize = GetTableSize(tableIndex);
        }

        arguments_t arguments{
            .arg[0] = status,                           /* Status */
            .arg[1] = CallbackArg,                      /* CallbackArg */
            .arg[2] = (uint64_t)tableAddress,           /* TableAddress */
            .arg[3] = (uint64_t)tableSize,              /* TableSize */
            .arg[4] = NULL,                             /* GP2 */
            .arg[5] = NULL,                             /* GP3 */
        };

        Sys_Execthread(Callback, &arguments, ExecutionTypeQueu, NULL);
        Sys_Close(KSUCCESS);
    }else{
        arguments_t arguments{
            .arg[0] = KFAIL,            /* Status */
            .arg[1] = CallbackArg,      /* CallbackArg */
            .arg[2] = NULL,             /* Size */
            .arg[3] = NULL,             /* Data */
            .arg[4] = NULL,             /* GP2 */
            .arg[5] = NULL,             /* GP3 */
        };
        Sys_Execthread(Callback, &arguments, ExecutionTypeQueu, NULL);
        Sys_Close(KFAIL);
    }
}

KResult GetSystemManagementBIOSTable(thread_t Callback, uint64_t CallbackArg){
    arguments_t arguments{
        .arg[0] = KSUCCESS,                     /* Status */
        .arg[1] = CallbackArg,                  /* CallbackArg */
        .arg[2] = (uint64_t)SrvInfo->Smbios,    /* Smbios physical address */
        .arg[3] = NULL,                         /* GP1 */
        .arg[4] = NULL,                         /* GP2 */
        .arg[5] = NULL,                         /* GP3 */
    };

    Sys_Execthread(Callback, &arguments, ExecutionTypeQueu, NULL);
    Sys_Close(KSUCCESS);
}

KResult BindIRQLine(thread_t Callback, uint64_t CallbackArg, uint8_t IRQLineNumber, thread_t Target, bool IgnoreMissedEvents){
    KResult Status = KFAIL;
    if(IRQLineNumber < SrvInfo->IRQLineSize){
        uint8_t vector = SrvInfo->IRQLineStart + IRQLineNumber;
        Status = Sys_Event_Bind(SrvInfo->IRQEvents[vector], Target, IgnoreMissedEvents);
    }
    
    arguments_t arguments{
        .arg[0] = Status,            /* Status */
        .arg[1] = CallbackArg,      /* CallbackArg */
        .arg[2] = NULL,             /* GP0 */
        .arg[3] = NULL,             /* GP1 */
        .arg[4] = NULL,             /* GP2 */
        .arg[5] = NULL,             /* GP3 */
    };

    Sys_Execthread(Callback, &arguments, ExecutionTypeQueu, NULL);
    Sys_Close(KSUCCESS);
}

KResult UnbindIRQLine(thread_t Callback, uint64_t CallbackArg, uint8_t IRQLineNumber, thread_t Target){
    KResult Status = KFAIL;
    if(IRQLineNumber < SrvInfo->IRQLineSize){
        uint8_t vector = SrvInfo->IRQLineStart + IRQLineNumber;
        Status = Sys_Event_Unbind(SrvInfo->IRQEvents[vector], Target);
    }
    
    arguments_t arguments{
        .arg[0] = Status,            /* Status */
        .arg[1] = CallbackArg,      /* CallbackArg */
        .arg[2] = NULL,             /* GP0 */
        .arg[3] = NULL,             /* GP1 */
        .arg[4] = NULL,             /* GP2 */
        .arg[5] = NULL,             /* GP3 */
    };

    Sys_Execthread(Callback, &arguments, ExecutionTypeQueu, NULL);
    Sys_Close(KSUCCESS);
}

KResult BindFreeIRQ(thread_t Callback, uint64_t CallbackArg, thread_t Target, bool IgnoreMissedEvents){
    event_t Vector = NULL;
    for(size64_t i = 0; i < SrvInfo->IRQSize; i++){
        if(SrvInfo->IsIRQEventsFree[i]){
            Vector = SrvInfo->IsIRQEventsFree[i];
            break;
        }
    }
    KResult Status = KFAIL;
    if(Vector != NULL){
        Status = Sys_Event_Bind(SrvInfo->IRQEvents[Vector], Target, IgnoreMissedEvents);
    }
    arguments_t arguments{
        .arg[0] = KSUCCESS,         /* Status */
        .arg[1] = CallbackArg,      /* CallbackArg */
        .arg[2] = Vector,              /* IRQNumber */
        .arg[3] = NULL,             /* GP1 */
        .arg[4] = NULL,             /* GP2 */
        .arg[5] = NULL,             /* GP3 */
    };

    Sys_Execthread(Callback, &arguments, ExecutionTypeQueu, NULL);
    Sys_Close(KSUCCESS);
}

KResult UnbindIRQ(thread_t Callback, uint64_t CallbackArg, thread_t Target, uint8_t Vector){
    KResult Status = KFAIL;
    if(Vector > SrvInfo->IRQLineStart + SrvInfo->IRQLineSize && Vector < SrvInfo->IRQSize){
        Status = Sys_Event_Unbind(SrvInfo->IRQEvents[Vector], Target);
    }
    arguments_t arguments{
        .arg[0] = KSUCCESS,         /* Status */
        .arg[1] = CallbackArg,      /* CallbackArg */
        .arg[2] = NULL,             /* GP0 */
        .arg[3] = NULL,             /* GP1 */
        .arg[4] = NULL,             /* GP2 */
        .arg[5] = NULL,             /* GP3 */
    };

    Sys_Execthread(Callback, &arguments, ExecutionTypeQueu, NULL);
    Sys_Close(KSUCCESS);
}