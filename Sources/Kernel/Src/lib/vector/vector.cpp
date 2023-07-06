#include <lib/vector.h>
#include <memory/memory.h>

kot_vector_t* kot_vector_create() {
    kot_vector_t* vector = (kot_vector_t*)kmalloc(sizeof(kot_vector_t));
    vector->items = NULL;
    vector->length = 0;
    return vector;
}

kot_vector_t* kot_vector_clone(kot_vector_t* vector) {
    void** new_vector = (void**)kmalloc((size64_t)(vector->length * 8));
    memcpy(new_vector, vector->items, vector->length * 8);
    return (kot_vector_t*)new_vector;
}

void kot_vector_expand(kot_vector_t* vector, uint64_t len) {
    void** temp = (void**)kmalloc((size64_t)((vector->length + len) * 8));
    memcpy(temp, vector->items, vector->length * 8);
    kfree(vector->items);
    vector->items = temp;
    vector->length+=len;
}

uint64_t kot_vector_push(kot_vector_t* vector, void* item) {
    if (vector->items == NULL) {
        vector->items = (void**) kmalloc(8);
        vector->length = 1;
        *(void**)(vector->items) = item;
    } else {
        kot_vector_expand(vector, 1);
        *(void**)(vector->items + vector->length - 1) = item;
    }
    return vector->length - 1;
}

void* kot_vector_get(kot_vector_t* vector, uint64_t index) {
    return *(void**)(vector->items + index);
}

void kot_vector_set(kot_vector_t* vector, uint64_t index, void* item) {
    if (index < vector->length) {
        *(void**)(vector->items + index) = item;
    }
}

void kot_vector_clear(kot_vector_t* vector) {
    if (vector->items != NULL) {
        kfree(vector->items);
        vector->items = NULL;
        vector->length = 0;
    }
}

void kot_vector_remove(kot_vector_t* vector, uint64_t index) {
    if (vector->items != NULL && index < vector->length) {
        if (vector->length == 1) {
            kot_vector_clear(vector);
        } else if (vector->length != 0) {
            void** temp = (void**) kmalloc((size64_t)((vector->length - 1) * 8));
            if (index != 0) { memcpy(temp, vector->items, index * 8); }
            memcpy(temp, vector->items + index * 8, ((vector->length - 1) * 8) - index * 8);
            kfree(vector->items);
            vector->items = temp;
            vector->length--;
        }
    }
}