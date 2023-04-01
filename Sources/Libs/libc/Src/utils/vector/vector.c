#include "../vector.h"

vector_t* vector_create() {
    vector_t* vector = (vector_t*) malloc(sizeof(vector_t));
    vector->items = NULL;
    vector->length = 0;
    return vector;
}

vector_t* vector_clone(vector_t* vector) {
    uintptr_t* new_vector = (uintptr_t*) malloc((size64_t)(vector->length * 8));
    memcpy(new_vector, vector->items, vector->length * 8);
    return new_vector;
}

void vector_expand(vector_t* vector, uint64_t len) {
    uintptr_t* temp = (uintptr_t*) malloc((size64_t)((vector->length + len) * 8));
    memcpy(temp, vector->items, vector->length * 8);
    free(vector->items);
    vector->items = temp;
    vector->length+=len;
}

uint64_t vector_push(vector_t* vector, uintptr_t item) {
    if (vector->items == NULL) {
        vector->items = (uintptr_t*) malloc(8);
        vector->length = 1;
        *(uintptr_t*)(vector->items) = item;
    } else {
        vector_expand(vector, 1);
        *(uintptr_t*)(vector->items + vector->length - 1) = item;
    }
    return vector->length - 1;
}

uintptr_t vector_get(vector_t* vector, uint64_t index) {
    return *(uintptr_t*)(vector->items + index);
}

void vector_set(vector_t* vector, uint64_t index, uintptr_t item) {
    if (index < vector->length) {
        *(uintptr_t*)(vector->items + index) = item;
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
            uintptr_t* temp = (uintptr_t*) malloc((size64_t)((vector->length - 1) * 8));
            if (index != 0) { memcpy(temp, vector->items, index * 8); }
            memcpy(temp, vector->items + index * 8, ((vector->length - 1) * 8) - index * 8);
            free(vector->items);
            vector->items = temp;
            vector->length--;
        }
    }
}