#pragma once
#include <libgen.h>
#include <core/main.h>
#include <kot/uisd/srvs/system.h>

#define System_Srv_Version 0x1

void InitializeSrv(struct KernelInfo* kernelInfo);

KResult LoadExecutable(kot_thread_t Callback, uint64_t CallbackArg, kot_process_t Process, uint64_t Priviledge, char* Path);
KResult GetFramebuffer(kot_thread_t Callback, uint64_t CallbackArg);
KResult ReadFileFromInitrd(kot_thread_t Callback, uint64_t CallbackArg, char* Name);
KResult GetTableInRootSystemDescription(kot_thread_t Callback, uint64_t CallbackArg, char* Name);
KResult GetSystemManagementBIOSTable(kot_thread_t Callback, uint64_t CallbackArg);
KResult BindIRQLine(kot_thread_t Callback, uint64_t CallbackArg, uint8_t IRQLineNumber, kot_thread_t Target, bool IgnoreMissedEvents);
KResult UnbindIRQLine(kot_thread_t Callback, uint64_t CallbackArg, uint8_t IRQLineNumber, kot_thread_t Target);
KResult BindFreeIRQ(kot_thread_t Callback, uint64_t CallbackArg, kot_thread_t Target, bool IgnoreMissedEvents);
KResult UnbindIRQ(kot_thread_t Callback, uint64_t CallbackArg, kot_thread_t Target, uint8_t Vector);

struct SrvInfo_t{
    kot_srv_system_framebuffer_t* Framebuffer;

    uintptr_t Smbios;

    uintptr_t Rsdp;

    uint8_t IRQLineStart;
    uint8_t IRQLineSize;
    
    size64_t IRQSize;
    kot_event_t* IRQEvents; 
    bool* IsIRQEventsFree;
}__attribute__((packed));