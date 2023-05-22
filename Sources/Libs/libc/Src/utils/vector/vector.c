#include "../vector.h"

kot_vector_t* vector_create() {
    kot_vector_t* vector = (kot_vector_t*) malloc(sizeof(kot_vector_t));
    vector->items = NULL;
    vector->length = 0;
    return vector;
}

kot_vector_t* vector_clone(kot_vector_t* vector) {
    void** new_vector = (void**) malloc((size64_t)(vector->length * 8));
    memcpy(new_vector, vector->items, vector->length * 8);
    return new_vector;
}

void vector_expand(kot_vector_t* vector, uint64_t len) {
    void** temp = (void**) malloc((size64_t)((vector->length + len) * 8));
    memcpy(temp, vector->items, vector->length * 8);
    free(vector->items);
    vector->items = temp;
    vector->length+=len;
}

uint64_t vector_push(kot_vector_t* vector, void* item) {
    if (vector->items == NULL) {
        vector->items = (void**) malloc(8);
        vector->length = 1;
        *(void**)(vector->items) = item;
    } else {
        vector_expand(vector, 1);
        *(void**)(vector->items + vector->length - 1) = item;
    }
    return vector->length - 1;
}

void* vector_get(kot_vector_t* vector, uint64_t index) {
    return *(void**)(vector->items + index);
}

void vector_set(kot_vector_t* vector, uint64_t index, void* item) {
    if (index < vector->length) {
        *(void**)(vector->items + index) = item;
    }
}

void vector_clear(kot_vector_t* vector) {
    if (vector->items != NULL) {
        free(vector->items);
        vector->items = NULL;
        vector->length = 0;
    }
}

void vector_remove(kot_vector_t* vector, uint64_t index) {
    if (vector->items != NULL && index < vector->length) {
        if (vector->length == 1) {
            vector_clear(vector);
        } else if (vector->length != 0) {
            void** temp = (void**) malloc((size64_t)((vector->length - 1) * 8));
            if (index != 0) { memcpy(temp, vector->items, index * 8); }
            memcpy(temp, vector->items + index * 8, ((vector->length - 1) * 8) - index * 8);
            free(vector->items);
            vector->items = temp;
            vector->length--;
        }
    }
}