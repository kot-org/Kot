#ifndef _SRV_STORAGE_H
#define _SRV_STORAGE_H 1

#include <kot/sys.h>
#include <kot/uisd.h>
#include <kot/types.h>

#if defined(__cplusplus)
extern "C" {
#endif

#define SerialNumberSize        0x14
#define DriveModelNumberSize    0x28

typedef KResult (*StorageCallbackHandler)(KResult Status, struct srv_storage_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3);

struct srv_storage_space_info_t{
    thread_t CreateProtectedDeviceSpaceThread;
    thread_t ReadWriteDeviceThread;
    ksmem_t BufferRWKey;
    uint64_t BufferRWAlignement;
    uint64_t BufferRWUsableSize;
    uint64_t SpaceSize;
};

struct srv_storage_device_info_t{
    struct srv_storage_space_info_t MainSpace;
    uint64_t DeviceSize;
    uint8_t SerialNumber[SerialNumberSize];
    uint8_t DriveModelNumber[DriveModelNumberSize];
    process_t DriverProc;
};

struct srv_storage_callback_t{
    thread_t Self;
    uint64_t Data;
    size64_t Size;
    bool IsAwait;
    KResult Status;
    StorageCallbackHandler Handler;
};

void Srv_Storage_Initialize();

void Srv_Storage_Callback(KResult Status, struct srv_storage_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3);

struct srv_storage_callback_t* Srv_Storage_AddDevice(struct srv_storage_device_info_t* Info, bool IsAwait);
struct srv_storage_callback_t* Srv_Storage_RemoveDevice(uint64_t Index, bool IsAwait);

#if defined(__cplusplus)
}
#endif

#endif