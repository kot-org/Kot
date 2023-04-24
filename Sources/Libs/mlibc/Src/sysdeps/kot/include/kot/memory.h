#ifndef _MEMORY_H
#define _MEMORY_H 1

#include <kot/sys.h>
#include <kot/types.h>
#include <kot/atomic.h>

namespace Kot{
    uintptr_t GetFreeAlignedSpace(size64_t size);
    uintptr_t MapPhysical(uintptr_t physicalAddress, size64_t size);
    void MapPhysicalToVirtual(uintptr_t virtualAddress, uintptr_t* physicalAddress, size64_t size);
    uintptr_t GetPhysical(uintptr_t* physicalAddress, size64_t size);
    void FreeAddress(uintptr_t virtualAddress, size64_t size);
}


#endif