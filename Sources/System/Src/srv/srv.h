#pragma once
#include <core/main.h>
#include <kot/uisd/srvs/system.h>

#define System_Srv_Version 0x1

void InitializeSrv(struct KernelInfo* kernelInfo);

struct SrvInfo{
    srv_system_framebuffer_t* Framebuffer;
}__attribute__((packed));