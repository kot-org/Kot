#pragma once

#include <kot/uisd.h>
#include <core/main.h>
#include <device/device.h>
#include <kot/uisd/srvs/storage.h>

#define Storage_Srv_Version 0x1

KResult InitialiseSrv();

KResult AddDeviceSrv(thread_t Callback, uint64_t CallbackArg, srv_storage_device_info_t* Info);
KResult RemoveDeviceSrv(thread_t Callback, uint64_t CallbackArg, storage_device_t* Device);