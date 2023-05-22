#ifndef _MEMORY_H
#define _MEMORY_H 1

#include <kot/sys.h>
#include <stdlib.h>
#include <kot/types.h>
#include <kot/atomic.h>

#if defined(__cplusplus)
extern "C" {
#endif

void memset(void* start, uint8_t value, size64_t size);
void memset16(void* start, uint16_t value, size64_t size);
void memset32(void* start, uint32_t value, size64_t size);
void memset64(void* start, uint64_t value, size64_t size);

int memcmp(const void *aptr, const void *bptr, size64_t size);
void memcpy(void* destination, void* source, size64_t size);
void* GetFreeAlignedSpace(size64_t size);
void* MapPhysical(void* physicalAddress, size64_t size);
void MapPhysicalToVirtual(void* virtualAddress, void** physicalAddress, size64_t size);
void* GetPhysical(void** physicalAddress, size64_t size);
void FreeAddress(void* virtualAddress, size64_t size);

#if defined(__cplusplus)
}
#endif

#endif