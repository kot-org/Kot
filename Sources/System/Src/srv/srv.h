#pragma once
#include <core/main.h>
#include <kot/uisd/srvs/system.h>

#define System_Srv_Version 0x1

void InitializeSrv(struct KernelInfo* kernelInfo);

KResult GetFrameBuffer(thread_t Callback, uint64_t CallbackArg);
KResult ReadFileFromInitrd(thread_t Callback, uint64_t CallbackArg, char* Name);

struct SrvInfo_t{
    srv_system_framebuffer_t* Framebuffer;
}__attribute__((packed));