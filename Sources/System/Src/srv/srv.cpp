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
    Sys_Createthread(proc, (uintptr_t)&GetFrameBuffer, PriviledgeApp, &GetFrameBufferThread);
    SystemSrv->GetFramebuffer = MakeShareableThread(GetFrameBufferThread, PriviledgeService);

    /* ReadFileInitrd */
    thread_t ReadFileFromInitrdThread = NULL;
    Sys_Createthread(proc, (uintptr_t)&ReadFileFromInitrd, PriviledgeApp, &ReadFileFromInitrdThread);
    SystemSrv->ReadFileInitrd = MakeShareableThread(ReadFileFromInitrdThread, PriviledgeService);

    /* GetTableInRootSystemDescription */
    thread_t GetTableInRootSystemDescriptionThread = NULL;
    Sys_Createthread(proc, (uintptr_t)&GetTableInRootSystemDescription, PriviledgeApp, &GetTableInRootSystemDescriptionThread);
    SystemSrv->GetTableInRootSystemDescription = MakeShareableThread(GetTableInRootSystemDescriptionThread, PriviledgeDriver);

    /* GetSystemManagementBIOSTable */
    thread_t GetSystemManagementBIOSTableThread = NULL;
    Sys_Createthread(proc, (uintptr_t)&GetSystemManagementBIOSTable, PriviledgeApp, &GetSystemManagementBIOSTableThread);
    SystemSrv->GetSystemManagementBIOSTable = MakeShareableThread(GetSystemManagementBIOSTableThread, PriviledgeDriver);

    /* BindIRQLine */
    thread_t BindIRQLineThread = NULL;
    Sys_Createthread(proc, (uintptr_t)&BindIRQLine, PriviledgeApp, &BindIRQLineThread);
    SystemSrv->BindIRQLine = MakeShareableThread(BindIRQLineThread, PriviledgeDriver);

    /* BindFreeIRQ */
    thread_t BindFreeIRQThread = NULL;
    Sys_Createthread(proc, (uintptr_t)&BindFreeIRQ, PriviledgeApp, &BindFreeIRQThread);
    SystemSrv->BindFreeIRQ = MakeShareableThread(BindFreeIRQThread, PriviledgeDriver);

    /* Setup data */
    SrvInfo = (SrvInfo_t*)malloc(sizeof(SrvInfo_t));

    SrvInfo->Framebuffer = (srv_system_framebuffer_t*)malloc(sizeof(srv_system_framebuffer_t));
    SrvInfo->Framebuffer->Address = kernelInfo->Framebuffer.framebuffer_addr;
    SrvInfo->Framebuffer->Width = kernelInfo->Framebuffer.framebuffer_width;
    SrvInfo->Framebuffer->Height = kernelInfo->Framebuffer.framebuffer_height;
    SrvInfo->Framebuffer->Pitch = kernelInfo->Framebuffer.framebuffer_pitch;
    SrvInfo->Framebuffer->Bpp = kernelInfo->Framebuffer.framebuffer_bpp;

    SrvInfo->Smbios = kernelInfo->Smbios;

    SrvInfo->Rsdp = kernelInfo->Rsdp;

    SrvInfo->IRQLineStart = kernelInfo->IRQLineStart;
    SrvInfo->IRQLineSize = kernelInfo->IRQLineSize;

    SrvInfo->IRQSize = kernelInfo->IRQSize;
    SrvInfo->IRQEvents = kernelInfo->IRQEvents;

    CreateControllerUISD(ControllerTypeEnum_System, key, true);
}

KResult GetFrameBuffer(thread_t Callback, uint64_t CallbackArg){
    ShareDataWithArguments_t data{
        .Data = SrvInfo->Framebuffer,
        .Size = sizeof(srv_system_framebuffer_t),
        .ParameterPosition = 0x2, 
    };

    arguments_t arguments{
        .arg[0] = KSUCCESS,         /* Statu */
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
    initrd::File* file = initrd::Find(Name);
    uintptr_t fileData = NULL; 
    if(file != NULL){
        fileData = initrd::Read(file);
    }else{
        arguments_t arguments{
            .arg[0] = KFAIL,            /* Statu */
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
        .arg[0] = KSUCCESS,         /* Statu */
        .arg[1] = CallbackArg,      /* CallbackArg */
        .arg[2] = file->size,       /* Size */
        .arg[3] = NULL,             /* Data */
        .arg[4] = NULL,             /* GP2 */
        .arg[5] = NULL,             /* GP3 */
    };
    
    Sys_Execthread(Callback, &arguments, ExecutionTypeQueu, &data);
    Sys_Close(KSUCCESS);
}

KResult GetTableInRootSystemDescription(thread_t Callback, uint64_t CallbackArg, char* Name){
    uintptr_t physicalTableAddress = FindTable(Name);

    KResult status = KFAIL;

    if(physicalTableAddress != NULL){
        status = KSUCCESS;
    }

    arguments_t arguments{
        .arg[0] = status,                           /* Statu */
        .arg[1] = CallbackArg,                      /* CallbackArg */
        .arg[2] = (uint64_t)physicalTableAddress,   /* PhysicalTableAddress */
        .arg[3] = NULL,                             /* GP1 */
        .arg[4] = NULL,                             /* GP2 */
        .arg[5] = NULL,                             /* GP3 */
    };

    Sys_Execthread(Callback, &arguments, ExecutionTypeQueu, NULL);
    Sys_Close(KSUCCESS);
}

KResult GetSystemManagementBIOSTable(thread_t Callback, uint64_t CallbackArg){
    arguments_t arguments{
        .arg[0] = KSUCCESS,                     /* Statu */
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
    KResult Statu = KFAIL;
    if(IRQLineNumber < SrvInfo->IRQLineSize){
        uint8_t vector = SrvInfo->IRQLineStart + IRQLineNumber;
        Statu = Sys_Event_Bind(SrvInfo->IRQEvents[vector], Target, IgnoreMissedEvents);
    }
    
    arguments_t arguments{
        .arg[0] = Statu,            /* Statu */
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
    event_t IRQ = NULL;
    for(size64_t i = 0; i < SrvInfo->IRQSize; i++){
        if(SrvInfo->IsIRQEventsFree[i]){
            IRQ = SrvInfo->IsIRQEventsFree[i];
            break;
        }
    }
    KResult Statu = KFAIL;
    if(IRQ != NULL){
        Statu = Sys_Event_Bind(IRQ, Target, IgnoreMissedEvents);
    }
    arguments_t arguments{
        .arg[0] = KSUCCESS,         /* Statu */
        .arg[1] = CallbackArg,      /* CallbackArg */
        .arg[2] = IRQ,              /* IRQNumber */
        .arg[3] = NULL,             /* GP1 */
        .arg[4] = NULL,             /* GP2 */
        .arg[5] = NULL,             /* GP3 */
    };

    Sys_Execthread(Callback, &arguments, ExecutionTypeQueu, NULL);
    Sys_Close(KSUCCESS);
}