#include <kot/memory.h>

extern "C" {

uint64_t kot_MemoryLock;

void* kot_GetFreeAlignedSpace(size64_t size){
    atomicAcquire(&kot_MemoryLock, 0);
    if(size % KotSpecificData.MMapPageSize){
        size -= size % KotSpecificData.MMapPageSize;
        size += KotSpecificData.MMapPageSize;
    }
    KotSpecificData.FreeMemorySpace = (void*)((uintptr_t)KotSpecificData.FreeMemorySpace - size);
    void* ReturnValue = KotSpecificData.FreeMemorySpace;
    atomicUnlock(&kot_MemoryLock, 0);
    return ReturnValue;
}

void* kot_MapPhysical(void* physicalAddress, size64_t size){
    void* ReturnValue = kot_GetFreeAlignedSpace(size);
    kot_Sys_Map(kot_Sys_GetProcess(), &ReturnValue, AllocationTypePhysical, &physicalAddress, &size, false);
    ReturnValue = (void*)(((uintptr_t)ReturnValue) | ((uintptr_t)physicalAddress & 0xFFF));
    return ReturnValue;
}

void kot_MapPhysicalToVirtual(void* virtualAddress, void** physicalAddress, size64_t size){
    kot_Sys_Map(kot_Sys_GetProcess(), &virtualAddress, AllocationTypePhysicalContiguous, physicalAddress, &size, false);
}

void* kot_GetPhysical(void** physicalAddress, size64_t size){
    void* ReturnValue = kot_GetFreeAlignedSpace(size);
    kot_Sys_Map(kot_Sys_GetProcess(), &ReturnValue, AllocationTypePhysicalContiguous, physicalAddress, &size, false);
    return ReturnValue;
}

void kot_FreeAddress(void* virtualAddress, size64_t size){
    kot_Sys_Unmap(kot_Sys_GetProcess(), virtualAddress, size);
}

void kot_memset(void* start, uint8_t value, size64_t size){
    for (uint64_t i = 0; i < size; i += sizeof(uint8_t)){
        *(uint8_t*)((uint64_t)start + i) = value;
    }
}       

void kot_memset16(void* start, uint16_t value, size64_t size){
    for (uint64_t i = 0; i < size; i += sizeof(uint16_t)){
        *(uint16_t*)((uint64_t)start + i) = value;
    }
}

void kot_memset32(void* start, uint32_t value, size64_t size){
    for (uint64_t i = 0; i < size; i += sizeof(uint32_t)){
        *(uint32_t*)((uint64_t)start + i) = value;
    }
}

void kot_memset64(void* start, uint64_t value, size64_t size){
    for (uint64_t i = 0; i < size; i += sizeof(uint64_t)){
        *(uint64_t*)((uint64_t)start + i) = value;
    }
}

}