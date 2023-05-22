#pragma once

#include <kot/uisd.h>
#include <core/main.h>
#include <device/device.h>
#include <stdio.h>

#define Storage_Srv_Version 0x1

KResult InitialiseSrv();

KResult AddDeviceSrv(kot_thread_t Callback, uint64_t CallbackArg, struct srv_storage_device_info_t* Info);
KResult RemoveDeviceSrv(kot_thread_t Callback, uint64_t CallbackArg, struct storage_device_t* Device);

KResult NotifyOnNewPartitionByGUIDTypeSrv(kot_thread_t Callback, uint64_t CallbackArg, kot_thread_t ThreadToNotify, kot_process_t ProcessToNotify, kot_GUID_t* PartitionTypeGUID);