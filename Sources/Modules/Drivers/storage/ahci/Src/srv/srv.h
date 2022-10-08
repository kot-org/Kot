#pragma once 

#include <core/main.h>
#include <port/port.h>
#include <kot/types.h>

struct Space_t{
    class Device* StorageDevice;
    struct HBACommandTable_t* CommandAddressTable;
    uint64_t Start;     // Start in byte
    uint64_t Size;      // Size in byte

    /* Buffer */
    ksmem_t BufferKey;
    uintptr_t BufferVirtual;
}__attribute__((packed));

void SrvAddDevice(class Device* Device);
void SrvRemoveDevice(Device* Device);

void SrvReadWriteHandler(thread_t Callback, uint64_t CallbackArg, uint64_t Start, size64_t Size, bool IsWrite);
KResult SrvRead(struct Space_t* Space, class Device* Device, uint64_t Start, size64_t Size);
KResult SrvWrite(struct Space_t* Space, class Device* Device, uint64_t Start, size64_t Size);