#ifndef _MEMORY_H
#define _MEMORY_H 1

#include <kot/sys.h>
#include <kot/types.h>
#include <kot/atomic.h>

#if defined(__cplusplus)
extern "C" {
#endif


void kot_memset(void* start, uint8_t value, size64_t size);
void kot_memset16(void* start, uint16_t value, size64_t size);
void kot_memset32(void* start, uint32_t value, size64_t size);
void kot_memset64(void* start, uint64_t value, size64_t size);

void* kot_GetFreeAlignedSpace(size64_t size);
void* kot_MapPhysical(void* physicalAddress, size64_t size);
void kot_MapPhysicalToVirtual(void* virtualAddress, void** physicalAddress, size64_t size);
void* kot_GetPhysical(void** physicalAddress, size64_t size);
void kot_FreeAddress(void* virtualAddress, size64_t size);


#if defined(__cplusplus)
} 
#endif

#endif