#pragma once

#include <lib/types.h>

typedef struct {
    void** items;
    uint64_t length;
} kot_vector_t;


kot_vector_t* kot_vector_create();
uint64_t kot_vector_push(kot_vector_t* vector, void* item);
void kot_vector_remove(kot_vector_t* vector, uint64_t index);
void kot_vector_set(kot_vector_t* vector, uint64_t index, void* item);
void kot_vector_expand(kot_vector_t* vector, uint64_t len);
void* kot_vector_get(kot_vector_t* vector, uint64_t index);
void kot_vector_clear(kot_vector_t* vector);
kot_vector_t* kot_vector_clone(kot_vector_t* vector);