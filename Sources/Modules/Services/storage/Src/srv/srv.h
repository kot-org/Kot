#pragma once

#include <kot/uisd.h>
#include <core/main.h>
#include <device/device.h>
#include <kot/uisd/srvs/storage.h>

#define Storage_Srv_Version 0x1

KResult InitialiseSrv();

KResult AddDeviceSrv(thread_t Callback, uint64_t CallbackArg, struct srv_storage_device_info_t* Info);
KResult RemoveDeviceSrv(thread_t Callback, uint64_t CallbackArg, struct storage_device_t* Device);

KResult CountPartitionByGUIDTypeSrv(thread_t Callback, uint64_t CallbackArg, struct GUID_t* PartitionTypeGUID);
KResult MountPartitionSrv(thread_t Callback, uint64_t CallbackArg, uint64_t Index, struct GUID_t* PartitionTypeGUID, struct srv_storage_fs_server_functions_t* FSServerFunctions);
KResult UnmountPartitionSrv(thread_t Callback, uint64_t CallbackArg, uint64_t Index, struct GUID_t* PartitionTypeGUID);