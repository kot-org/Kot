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
    kot_key_mem_t BufferKey;
    uintptr_t BufferVirtual;
}__attribute__((packed));

void SrvAddDevice(class Device* Device);
void SrvRemoveDevice(thread_t Callback, uint64_t CallbackArg);

void SrvCreateProtectedSpace(thread_t Callback, uint64_t CallbackArg, uint64_t Start, uint64_t Size);

void SrvRequestHandler(thread_t Callback, uint64_t CallbackArg, uint64_t RequestType, uint64_t GP0, uint64_t GP1, uint64_t GP2);