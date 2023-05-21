#ifndef _SRV_STORAGE_DEVICE_H
#define _SRV_STORAGE_DEVICE_H 1

#include <kot/sys.h>
#include <kot/uisd.h>
#include <kot/types.h>
#include <kot/uisd/srvs/storage.h>

#if defined(__cplusplus)
extern "C" {
#endif

#define STORAGE_SINGLE_REQUEST      0x0
#define STORAGE_MULTIPLE_REQUESTS   0x1

struct kot_srv_storage_device_t{
    uintptr_t BufferRWBase;
    size_t BufferRWSize;
    struct kot_srv_storage_space_info_t SpaceInfo;
    kot_thread_t CallbackRequestHandlerThread;
    kot_thread_t CallbackCreateSpaceHandlerThread;
    uint64_t Lock;    
};

struct kot_srv_storage_device_callback_t{
    KResult Status;
    kot_thread_t MainThread;
    uintptr_t Data;
    size64_t Size;
};

typedef struct{
    uint64_t Start;
    size64_t Size;
    uint64_t BufferOffset;
}kot_srv_storage_request_t;

typedef struct{
    uint64_t RequestsCount;
    size64_t TotalSize;
    bool IsWrite;
    // If is write
    kot_ksmem_t MemoryKey;
    // Endif
    kot_srv_storage_request_t Requests[];
}kot_srv_storage_multiple_requests_t;

KResult Srv_StorageInitializeDeviceAccess(struct kot_srv_storage_space_info_t* StorageSpace, struct kot_srv_storage_device_t** StorageDevice);
KResult Srv_CallbackCreateSpaceHandler(KResult Status, struct kot_srv_storage_device_callback_t* CallbackData, struct kot_srv_storage_space_info_t* SpaceInfo);


KResult Srv_CallbackCreateSpaceHandler(KResult Status, struct kot_srv_storage_device_callback_t* CallbackData, struct kot_srv_storage_space_info_t* SpaceInfo);
KResult Srv_CallbackRequestHandler(KResult Status, struct kot_srv_storage_device_callback_t* CallbackData, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3);
KResult Srv_SendRequest(struct kot_srv_storage_device_t* StorageDevice, uint64_t Start, size64_t Size, bool IsWrite);
struct kot_srv_storage_device_callback_t* Srv_SendMultipleRequests(struct kot_srv_storage_device_t* StorageDevice, kot_srv_storage_multiple_requests_t* Requests);
uint64_t Srv_GetBufferStartingAddress(struct kot_srv_storage_device_t* StorageDevice, uint64_t Start);
KResult Srv_ReadDevice(struct kot_srv_storage_device_t* StorageDevice, uintptr_t Buffer, uint64_t Start, size64_t Size);
KResult Srv_WriteDevice(struct kot_srv_storage_device_t* StorageDevice, uintptr_t Buffer, uint64_t Start, size64_t Size);

#if defined(__cplusplus)
} 
#endif

#endif