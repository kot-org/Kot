#include "../vector.h"

vector_t* vector_create(size_t size) {
    vector_t* vector = (vector_t*) malloc(sizeof(vector_t));
    vector->items = NULL;
    vector->length = 0;
    vector->size = size;
    return vector;
}

void vector_expand(vector_t* vector, uint64_t len) {
    uintptr_t* temp = (uintptr_t*) malloc((size_t)((vector->length + len) * vector->size));
    memcpy(temp, vector->items, vector->length * vector->size);
    free(vector->items);
    vector->items = temp;
    vector->length+=len;
}

void vector_push(vector_t* vector, uintptr_t item) {
    if (vector->items == NULL) {
        vector->items = (uintptr_t*) malloc(vector->size);
        vector->length = 1;
        *(vector->items) = item;
    } else {
        vector_expand(vector, 1);
        *(vector->items + vector->length - 1) = item;
    }
}

uintptr_t vector_get(vector_t* vector, uint64_t index) {
    return *(vector->items + index);
}

void vector_set(vector_t* vector, uint64_t index, uintptr_t item) {
    if (index < vector->length) {
        *(vector->items + index) = item;
    }
}

void vector_clear(vector_t* vector) {
    if (vector->items != NULL) {
        free(vector->items);
        vector->items = NULL;
        vector->length = 0;
    }
}

void vector_remove(vector_t* vector, uint64_t index) {
    if (vector->items != NULL && index < vector->length) {
        if (vector->length == 1) {
            vector_clear(vector);
        } else if (vector->length != 0) {
            uintptr_t* temp = (uintptr_t*) malloc((size_t)((vector->length - 1) * vector->size));
            if (index != 0) { memcpy(temp, vector->items, index * vector->size); }
            memcpy(temp, vector->items + index, ((vector->length - 1) * vector->size) - index * vector->size);
            free(vector->items);
            vector->items = temp;
            vector->length--;
        }
    }
}