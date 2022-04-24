#ifndef _MEMORY_H
#define _MEMORY_H 1

#include <kot/types.h>

#if defined(__cplusplus)
extern "C" {
#endif

void memset(void* start, uint8_t value, uint64_t num);
int memcmp(const void *aptr, const void *bptr, size_t n);
void memcpy(void* destination, void* source, uint64_t num);

uint64_t SuperTest();

#if defined(__cplusplus)
}
#endif

#endif