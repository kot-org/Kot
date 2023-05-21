#include <kot/memory.h>

extern "C" {

uint64_t kot_MemoryLock;

uintptr_t kot_GetFreeAlignedSpace(size64_t size){
    atomicAcquire(&kot_MemoryLock, 0);
    if(size % KotSpecificData.MMapPageSize){
        size -= size % KotSpecificData.MMapPageSize;
        size += KotSpecificData.MMapPageSize;
    }
    KotSpecificData.FreeMemorySpace -= size;
    uintptr_t ReturnValue = KotSpecificData.FreeMemorySpace;
    atomicUnlock(&kot_MemoryLock, 0);
    return ReturnValue;
}

uintptr_t kot_MapPhysical(uintptr_t physicalAddress, size64_t size){
    uint64_t ReturnValue = (uint64_t)kot_GetFreeAlignedSpace(size);
    kot_Sys_Map(kot_Sys_GetProcess(), (uintptr_t*)&ReturnValue, AllocationTypePhysical, &physicalAddress, &size, false);
    ReturnValue |= (uint64_t)physicalAddress & 0xFFF;
    return ReturnValue;
}

void kot_MapPhysicalToVirtual(uintptr_t virtualAddress, uintptr_t* physicalAddress, size64_t size){
    kot_Sys_Map(kot_Sys_GetProcess(), (uintptr_t*)&virtualAddress, AllocationTypePhysicalContiguous, physicalAddress, &size, false);
}

uintptr_t kot_GetPhysical(uintptr_t* physicalAddress, size64_t size){
    uint64_t ReturnValue = (uint64_t)kot_GetFreeAlignedSpace(size);
    kot_Sys_Map(kot_Sys_GetProcess(), (uintptr_t*)&ReturnValue, AllocationTypePhysicalContiguous, physicalAddress, &size, false);
    return ReturnValue;
}

void kot_FreeAddress(uintptr_t virtualAddress, size64_t size){
    kot_Sys_Unmap(kot_Sys_GetProcess(), virtualAddress, size);
}

}