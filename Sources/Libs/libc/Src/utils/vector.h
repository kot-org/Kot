#pragma once

#include <kot/heap.h>

typedef struct {
    uintptr_t* items;
    size_t size;
    uint64_t length;
} vector_t;

#if defined(__cplusplus)
extern "C" {
#endif

vector_t* vector_create(size_t size);
void vector_push(vector_t* vector, uintptr_t item);
void vector_remove(vector_t* vector, uint64_t index);
void vector_set(vector_t* vector, uint64_t index, uintptr_t item);
uintptr_t vector_get(vector_t* vector, uint64_t index);
void vector_clear(vector_t* vector);

#if defined(__cplusplus)
}
#endif