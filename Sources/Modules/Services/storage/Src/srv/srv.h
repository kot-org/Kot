#pragma once

#include <kot/uisd.h>
#include <core/main.h>
#include <device/device.h>

#define Storage_Srv_Version 0x1

KResult InitialiseSrv();

KResult AddDeviceSrv(thread_t ReadWriteThread, uint64_t ReadWriteArg, ksmem_t BufferRW, uint64_t BufferRWAlignement, storage_device_info_t* Info);
KResult RemoveDeviceSrv(uint64_t Index);