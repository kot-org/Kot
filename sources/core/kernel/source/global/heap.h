#ifndef _GLOBAL_HEAP_H
#define _GLOBAL_HEAP_H 1

#include <stdint.h>
#include <stddef.h>

void heap_init(void* heap_address_high, void* heap_address_low);

void* malloc(size_t size);
void* calloc(size_t number, size_t size);
void* realloc(void* ptr, size_t size);
void free(void* ptr);

#endif // _GLOBAL_HEAP_H
