#pragma once
#include <core/main.h>


struct storage_device_info_t{
    uint64_t DeviceSize;
    uint16_t SerialNumber[10];
    uint8_t DriveModelNumber[40];
};
struct storage_device_t{
    thread_t ReadWriteThread;
    uint64_t ReadWriteArg;
    ksmem_t BufferRWKey;
    uintptr_t BufferRWBase;
    size_t BufferRWSize;
    uint64_t BufferRWAlignement;
    storage_device_info_t Info;
};

KResult InitializeDeviceHandling();

uint64_t AddDevice(thread_t ReadWriteThread, uint64_t ReadWriteArg, ksmem_t BufferRW, uint64_t BufferRWAlignement, storage_device_info_t* Info);
void RemoveDevice(uint64_t Index);