#pragma once

#include <stdlib.h>

typedef struct {
    uintptr_t* items;
    uint64_t length;
} kot_vector_t;

#if defined(__cplusplus)
extern "C" {
#endif

kot_vector_t* vector_create();
uint64_t vector_push(kot_vector_t* vector, uintptr_t item);
void vector_remove(kot_vector_t* vector, uint64_t index);
void vector_set(kot_vector_t* vector, uint64_t index, uintptr_t item);
void vector_expand(kot_vector_t* vector, uint64_t len);
uintptr_t vector_get(kot_vector_t* vector, uint64_t index);
void vector_clear(kot_vector_t* vector);
kot_vector_t* vector_clone(kot_vector_t* vector);

#if defined(__cplusplus)
}
#endif