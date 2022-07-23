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
        vector->items = (uintptr_t*) calloc(vector->size);
        vector->length = 1;
        vector->items[0] = item;
    } else {
        uintptr_t* temp = (uintptr_t*) calloc((size_t) vector->length+1 * sizeof(vector->size));
        for (uint64_t i = 0; i < vector->length; i++) {
            temp[i] = vector->items[i];
        }
        temp[vector->length] = item;
        free(vector->items);
        vector->items = temp;
        vector->length++;
    }
}