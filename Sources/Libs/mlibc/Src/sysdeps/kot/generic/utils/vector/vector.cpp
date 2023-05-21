#include <kot/utils/vector.h>
#include <string.h>

kot_vector_t* kot_vector_create() {
    kot_vector_t* vector = (kot_vector_t*)malloc(sizeof(kot_vector_t));
    vector->items = NULL;
    vector->length = 0;
    return vector;
}

kot_vector_t* kot_vector_clone(kot_vector_t* vector) {
    uintptr_t* new_vector = (uintptr_t*)malloc((size64_t)(vector->length * 8));
    memcpy(new_vector, vector->items, vector->length * 8);
    return (kot_vector_t*)new_vector;
}

void kot_vector_expand(kot_vector_t* vector, uint64_t len) {
    uintptr_t* temp = (uintptr_t*)malloc((size64_t)((vector->length + len) * 8));
    memcpy(temp, vector->items, vector->length * 8);
    free(vector->items);
    vector->items = temp;
    vector->length+=len;
}

uint64_t kot_vector_push(kot_vector_t* vector, uintptr_t item) {
    if (vector->items == NULL) {
        vector->items = (uintptr_t*) malloc(8);
        vector->length = 1;
        *(uintptr_t*)(vector->items) = item;
    } else {
        kot_vector_expand(vector, 1);
        *(uintptr_t*)(vector->items + vector->length - 1) = item;
    }
    return vector->length - 1;
}

uintptr_t kot_vector_get(kot_vector_t* vector, uint64_t index) {
    return *(uintptr_t*)(vector->items + index);
}

void kot_vector_set(kot_vector_t* vector, uint64_t index, uintptr_t item) {
    if (index < vector->length) {
        *(uintptr_t*)(vector->items + index) = item;
    }
}

void kot_vector_clear(kot_vector_t* vector) {
    if (vector->items != NULL) {
        free(vector->items);
        vector->items = NULL;
        vector->length = 0;
    }
}

void kot_vector_remove(kot_vector_t* vector, uint64_t index) {
    if (vector->items != NULL && index < vector->length) {
        if (vector->length == 1) {
            kot_vector_clear(vector);
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