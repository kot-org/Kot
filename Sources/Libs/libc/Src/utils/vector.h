#pragma once

#include <kot/heap.h>

typedef struct {
    uintptr_t* items;
    uint64_t length;
} vector_t;

#if defined(__cplusplus)
extern "C" {
#endif

vector_t* vector_create();
uint64_t vector_push(vector_t* vector, uintptr_t item);
void vector_remove(vector_t* vector, uint64_t index);
void vector_set(vector_t* vector, uint64_t index, uintptr_t item);
void vector_expand(vector_t* vector, uint64_t len);
uintptr_t vector_get(vector_t* vector, uint64_t index);
void vector_clear(vector_t* vector);
vector_t* vector_clone(vector_t* vector);

#if defined(__cplusplus)
}
#endif