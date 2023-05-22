#pragma once

#include <stdlib.h>

typedef struct {
    void** items;
    uint64_t length;
} kot_vector_t;

#if defined(__cplusplus)
extern "C" {
#endif

kot_vector_t* vector_create();
uint64_t vector_push(kot_vector_t* vector, void* item);
void vector_remove(kot_vector_t* vector, uint64_t index);
void vector_set(kot_vector_t* vector, uint64_t index, void* item);
void vector_expand(kot_vector_t* vector, uint64_t len);
void* vector_get(kot_vector_t* vector, uint64_t index);
void vector_clear(kot_vector_t* vector);
kot_vector_t* vector_clone(kot_vector_t* vector);

#if defined(__cplusplus)
}
#endif