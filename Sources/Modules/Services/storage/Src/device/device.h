#pragma once
#include <kot/math.h>
#include <core/main.h>
#include <partition/partition.h>
#include <kot/uisd/srvs/storage.h>
#include <kot/uisd/srvs/storage/device.h>

struct storage_callback_t{
    kot_thread_t MainThread;
    void* Data;
    size64_t Size;
    KResult Status;
};

struct storage_device_t{
    void* BufferRWBase;
    size_t BufferRWSize;
    kot_srv_storage_device_info_t Info;
    kot_thread_t CallbackRequestHandlerThread;
    kot_thread_t CallbackCreateSpaceHandlerThread;
    uint64_t Lock;

    KResult CreateSpace(uint64_t Start, size64_t Size, kot_srv_storage_space_info_t** SpaceInfo);

    KResult SendRequest(uint64_t Start, size64_t Size, bool IsWrite);

    uint64_t GetBufferStartingAddress(uint64_t Start);

    KResult ReadDevice(void* Buffer, uint64_t Start, size64_t Size);
    KResult WriteDevice(void* Buffer, uint64_t Start, size64_t Size);
};

KResult CallbackCreateSpaceHandler(KResult Status, storage_callback_t* CallbackData, kot_srv_storage_space_info_t* SpaceInfo);
KResult CallbackRequestHandler(KResult Status, storage_callback_t* CallbackData);

KResult AddDevice(kot_srv_storage_device_info_t* Info, storage_device_t** DevicePointer);
KResult RemoveDevice(storage_device_t* Device);