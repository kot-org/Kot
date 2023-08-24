#ifndef _VECTOR_H
#define _VECTOR_H 1

#include <stdint.h>

typedef struct {
    void** items;
    uint64_t length;
}vector_t;


vector_t* vector_create(void);
uint64_t vector_push(vector_t* vector, void* item);
void vector_remove(vector_t* vector, uint64_t index);
void vector_set(vector_t* vector, uint64_t index, void* item);
void vector_expand(vector_t* vector, uint64_t len);
void* vector_get(vector_t* vector, uint64_t index);
void vector_clear(vector_t* vector);
vector_t* vector_clone(vector_t* vector);

#endif