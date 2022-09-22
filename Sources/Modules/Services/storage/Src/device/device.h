#pragma once
#include <core/main.h>
#include <kot/uisd/srvs/storage.h>

struct storage_device_t{
    uintptr_t BufferRWBase;
    size_t BufferRWSize;
    srv_storage_device_info_t Info;

    KResult ReadDevice(uint64_t Start, size64_t Size);
    KResult WriteDevice(uint64_t Start, size64_t Size);
};

KResult InitializeDeviceHandling();

KResult AddDevice(srv_storage_device_info_t* Info, uint64_t* DeviceIndex);
KResult RemoveDevice(uint64_t Index);