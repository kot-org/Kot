#include <kot/heap.h>

typedef struct {
    void **items;
    size_t size;
    uint64_t length;
} vector_t;

#if defined(__cplusplus)
extern "C" {
#endif

vector_t* vector_create(size_t size);
void vector_push(vector_t* vector, void *item);
void vector_clear(vector_t* vector);

#if defined(__cplusplus)
}
#endif