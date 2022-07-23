#include <kot/utils/vector.h>

vector_t* vector_create() {
    vector_t* vector = (vector_t*) malloc(sizeof(vector_t));
    vector->items = NULL;
    vector->length = 0;
    return vector;
}

void vector_push(vector_t* vector, void *item) {
    if (vector->items == NULL) {
        vector->items = (void**) malloc((uint64_t) sizeof(void*));
        vector->length = 1;
        vector->items[0] = item;
    } else {
        void** temp = (void**) malloc((size_t) vector->length+1 * sizeof(void*));
        for (uint64_t i = 0; i < vector->length; i++) {
            temp[i] = vector->items[i];
        }
        temp[vector->length] = item;
        free(&vector->items);
        vector->items = temp;
        vector->length++;
    }
}