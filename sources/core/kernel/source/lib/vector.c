#include <stdint.h>
#include <stddef.h>
#include <lib/assert.h>
#include <lib/lock.h>
#include <lib/memory.h>
#include <lib/vector.h>
#include <global/heap.h>

vector_t* vector_create(void) {
    vector_t* vector = (vector_t*)malloc(sizeof(vector_t));
    vector->items = NULL;
    vector->length = 0;
    vector->lock = (spinlock_t){};
    return vector;
}

vector_t* vector_clone(vector_t* vector) {
    assert(!spinlock_acquire(&vector->lock));
    void** new_vector = (void**)malloc((size_t)(vector->length * 8));
    memcpy(new_vector, vector->items, vector->length * 8);
    spinlock_release(&vector->lock);
    return (vector_t*)new_vector;
}

void vector_expand(vector_t* vector, uint64_t len) {
    assert(!spinlock_acquire(&vector->lock));
    void** temp = (void**)malloc((size_t)((vector->length + len) * 8));
    memcpy(temp, vector->items, vector->length * 8);
    free(vector->items);
    vector->items = temp;
    vector->length+=len;
    spinlock_release(&vector->lock);
}

uint64_t vector_push(vector_t* vector, void* item) {
    if (vector->items == NULL) {
        assert(!spinlock_acquire(&vector->lock));
        vector->items = (void**) malloc(8);
        vector->length = 1;
        *(void**)(vector->items) = item;
        spinlock_release(&vector->lock);
    } else {
        vector_expand(vector, 1);
        assert(!spinlock_acquire(&vector->lock));
        *(void**)(vector->items + vector->length - 1) = item;
        spinlock_release(&vector->lock);
    }
    return vector->length - 1;
}

void* vector_get(vector_t* vector, uint64_t index) {
    return *(void**)(vector->items + index);
}

void vector_set(vector_t* vector, uint64_t index, void* item) {
    if (index < vector->length) {
        *(void**)(vector->items + index) = item;
    }
}

void vector_clear(vector_t* vector) {
    if (vector->items != NULL) {
        assert(!spinlock_acquire(&vector->lock));
        free(vector->items);
        vector->items = NULL;
        vector->length = 0;
        spinlock_release(&vector->lock);
    }
}

void vector_remove(vector_t* vector, uint64_t index) {
    if (vector->items != NULL && index < vector->length) {
        if (vector->length == 1) {
            vector_clear(vector);
        } else if (vector->length != 0) {
            assert(!spinlock_acquire(&vector->lock));
            void** temp = (void**) malloc((size_t)((vector->length - 1) * 8));
            if (index != 0) { memcpy(temp, vector->items, index * 8); }
            memcpy(temp, vector->items + index * 8, ((vector->length - 1) * 8) - index * 8);
            free(vector->items);
            vector->items = temp;
            vector->length--;
            spinlock_release(&vector->lock);
        }
    }
}