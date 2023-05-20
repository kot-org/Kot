#ifndef _MEMORY_H
#define _MEMORY_H 1

#include <kot/sys.h>
#include <kot/types.h>
#include <kot/atomic.h>

uintptr_t kot_GetFreeAlignedSpace(size64_t size);
uintptr_t kot_MapPhysical(uintptr_t physicalAddress, size64_t size);
void kot_MapPhysicalToVirtual(uintptr_t virtualAddress, uintptr_t* physicalAddress, size64_t size);
uintptr_t kot_GetPhysical(uintptr_t* physicalAddress, size64_t size);
void kot_FreeAddress(uintptr_t virtualAddress, size64_t size);


#endif