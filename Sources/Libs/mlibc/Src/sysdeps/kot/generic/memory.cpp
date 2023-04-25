#include <kot/memory.h>

namespace Kot{
    uint64_t MemoryLock;

    uintptr_t GetFreeAlignedSpace(size64_t size){
        atomicAcquire(&MemoryLock, 0);
        if(size % KotSpecificData.MMapPageSize){
            size -= size % KotSpecificData.MMapPageSize;
            size += KotSpecificData.MMapPageSize;
        }
        KotSpecificData.FreeMemorySpace -= size;
        uintptr_t ReturnValue = KotSpecificData.FreeMemorySpace;
        atomicUnlock(&MemoryLock, 0);
        return ReturnValue;
    }

    uintptr_t MapPhysical(uintptr_t physicalAddress, size64_t size){
        uint64_t ReturnValue = (uint64_t)GetFreeAlignedSpace(size);
        Sys_Map(Sys_GetProcess(), (uintptr_t*)&ReturnValue, AllocationTypePhysical, &physicalAddress, &size, false);
        ReturnValue |= (uint64_t)physicalAddress & 0xFFF;
        return ReturnValue;
    }

    void MapPhysicalToVirtual(uintptr_t virtualAddress, uintptr_t* physicalAddress, size64_t size){
        Sys_Map(Sys_GetProcess(), (uintptr_t*)&virtualAddress, AllocationTypePhysicalContiguous, physicalAddress, &size, false);
    }

    uintptr_t GetPhysical(uintptr_t* physicalAddress, size64_t size){
        uint64_t ReturnValue = (uint64_t)GetFreeAlignedSpace(size);
        Sys_Map(Sys_GetProcess(), (uintptr_t*)&ReturnValue, AllocationTypePhysicalContiguous, physicalAddress, &size, false);
        return ReturnValue;
    }

    void FreeAddress(uintptr_t virtualAddress, size64_t size){
        Sys_Unmap(Sys_GetProcess(), virtualAddress, size);
    }
}