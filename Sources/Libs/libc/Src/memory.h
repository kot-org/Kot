#ifndef _MEMORY_H
#define _MEMORY_H 1

#include <kot/types.h>
#include <kot/heap.h>

#if defined(__cplusplus)
extern "C" {
#endif

void memset(uintptr_t start, uint8_t value, uint64_t num);
void memset16(uintptr_t start, uint16_t value, uint64_t num);
void memset32(uintptr_t start, uint32_t value, uint64_t num);
void memset64(uintptr_t start, uint64_t value, uint64_t num);

int memcmp(const void *aptr, const void *bptr, size_t n);
void memcpy(uintptr_t destination, uintptr_t source, uint64_t num);

#if defined(__cplusplus)
}
#endif

#endif