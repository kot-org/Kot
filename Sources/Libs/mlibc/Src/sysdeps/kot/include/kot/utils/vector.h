#ifndef KOT_UTILS_VECTOR_H
#define KOT_UTILS_VECTOR_H 1

#include <kot/types.h>
#include <stdlib.h>
#include <stdint.h>

#if defined(__cplusplus)
extern "C" {
#endif

typedef struct {
    uintptr_t* items;
    uint64_t length;
} kot_vector_t;


kot_vector_t* kot_vector_create();
uint64_t kot_vector_push(kot_vector_t* vector, uintptr_t item);
void kot_vector_remove(kot_vector_t* vector, uint64_t index);
void kot_vector_set(kot_vector_t* vector, uint64_t index, uintptr_t item);
void kot_vector_expand(kot_vector_t* vector, uint64_t len);
uintptr_t kot_vector_get(kot_vector_t* vector, uint64_t index);
void kot_vector_clear(kot_vector_t* vector);
kot_vector_t* kot_vector_clone(kot_vector_t* vector);

#if defined(__cplusplus)
}
#endif

#endif