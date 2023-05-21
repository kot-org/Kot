#ifndef _MEMORY_H
#define _MEMORY_H 1

#include <kot/sys.h>
#include <stdlib.h>
#include <kot/types.h>
#include <kot/atomic.h>

#if defined(__cplusplus)
extern "C" {
#endif

void memset(uintptr_t start, uint8_t value, size64_t size);
void memset16(uintptr_t start, uint16_t value, size64_t size);
void memset32(uintptr_t start, uint32_t value, size64_t size);
void memset64(uintptr_t start, uint64_t value, size64_t size);

int memcmp(const void *aptr, const void *bptr, size64_t size);
void memcpy(uintptr_t destination, uintptr_t source, size64_t size);
uintptr_t GetFreeAlignedSpace(size64_t size);
uintptr_t MapPhysical(uintptr_t physicalAddress, size64_t size);
void MapPhysicalToVirtual(uintptr_t virtualAddress, uintptr_t* physicalAddress, size64_t size);
uintptr_t GetPhysical(uintptr_t* physicalAddress, size64_t size);
void FreeAddress(uintptr_t virtualAddress, size64_t size);

#if defined(__cplusplus)
}
#endif

#endif