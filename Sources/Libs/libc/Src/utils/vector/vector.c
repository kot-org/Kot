#include <kot/utils/vector.h>

vector_t* vector_create(size_t size) {
    vector_t* vector = (vector_t*) malloc(sizeof(vector_t));
    vector->items = NULL;
    vector->length = 0;
    vector->size = size;
    return vector;
}

void vector_push(vector_t* vector, uintptr_t item) {
    if (vector->items == NULL) {
        vector->items = (uintptr_t*) malloc(vector->size);
        vector->length = 1;
        *(vector->items) = item;
    } else {
        uintptr_t* temp = (uintptr_t*) malloc((size_t)((vector->length + 1) * vector->size));
        memcpy(temp, vector->items, vector->length * vector->size);
        *(temp + vector->length) = item;
        free(vector->items);
        vector->items = temp;
        vector->length++;
    }
}

uintptr_t vector_get(vector_t* vector, uint64_t index) {
    return *(vector->items + index);
}

void vector_clear(vector_t* vector) {
    if (vector->items != NULL) {
        free(vector->items);
        vector->items = NULL;
        vector->length = 0;
    }
}