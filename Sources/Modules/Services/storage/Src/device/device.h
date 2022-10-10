#pragma once
#include <kot/math.h>
#include <core/main.h>
#include <partition/partition.h>
#include <kot/uisd/srvs/storage.h>

struct storage_callback_t{
    thread_t MainThread;
    uintptr_t Data;
    size64_t Size;
};

struct storage_device_t{
    uintptr_t BufferRWBase;
    size_t BufferRWSize;
    srv_storage_device_info_t Info;
    thread_t CallbackRequestHandlerThread;
    uint64_t Lock;

    KResult CreateSpace(uint64_t Start, size64_t Size, srv_storage_space_info_t** SpaceInfo);

    KResult SendRequest(uint64_t Start, size64_t Size, bool IsWrite);

    uint64_t GetBufferStartingAddress(uint64_t Start);

    KResult ReadDevice(uintptr_t Buffer, uint64_t Start, size64_t Size);
    KResult WriteDevice(uintptr_t Buffer, uint64_t Start, size64_t Size);
};

KResult CallbackCreateSpaceHandler(KResult Statu, storage_callback_t* CallbackData, srv_storage_space_info_t* SpaceInfo);
KResult CallbackRequestHandler(KResult Statu, thread_t MainThread);

KResult AddDevice(srv_storage_device_info_t* Info, storage_device_t** DevicePointer);
KResult RemoveDevice(storage_device_t* Device);