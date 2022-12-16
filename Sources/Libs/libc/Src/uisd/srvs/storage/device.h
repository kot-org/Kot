#ifndef _SRV_STORAGE_DEVICE_H
#define _SRV_STORAGE_DEVICE_H 1

#include <kot/sys.h>
#include <kot/uisd.h>
#include <kot/types.h>
#include <kot/uisd/srvs/storage.h>

#if defined(__cplusplus)
extern "C" {
#endif

struct srv_storage_device_t{
    uintptr_t BufferRWBase;
    size_t BufferRWSize;
    struct srv_storage_space_info_t SpaceInfo;
    thread_t CallbackRequestHandlerThread;
    thread_t CallbackCreateSpaceHandlerThread;
    uint64_t Lock;    
};

struct srv_storage_device_callback_t{
    thread_t MainThread;
    uintptr_t Data;
    size64_t Size;
};

KResult Srv_StorageInitializeDeviceAccess(struct srv_storage_space_info_t* StorageSpace, struct srv_storage_device_t** StorageDevice);
KResult Srv_CallbackCreateSpaceHandler(KResult Status, struct srv_storage_device_callback_t* CallbackData, struct srv_storage_space_info_t* SpaceInfo);


KResult Srv_CallbackCreateSpaceHandler(KResult Status, struct srv_storage_device_callback_t* CallbackData, struct srv_storage_space_info_t* SpaceInfo);
KResult Srv_CallbackRequestHandler(KResult Status, thread_t MainThread);
KResult Srv_SendRequest(struct srv_storage_device_t* StorageDevice, uint64_t Start, size64_t Size, bool IsWrite);
uint64_t Srv_GetBufferStartingAddress(struct srv_storage_device_t* StorageDevice, uint64_t Start);
KResult Srv_ReadDevice(struct srv_storage_device_t* StorageDevice, uintptr_t Buffer, uint64_t Start, size64_t Size);
KResult Srv_WriteDevice(struct srv_storage_device_t* StorageDevice, uintptr_t Buffer, uint64_t Start, size64_t Size);

#if defined(__cplusplus)
}
#endif

#endif