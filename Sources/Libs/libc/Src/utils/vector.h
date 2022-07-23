#include <kot/heap.h>

typedef struct {
    void **items;
    uint64_t length;
} vector_t;

#if defined(__cplusplus)
extern "C" {
#endif

vector_t* vector_create();
void vector_push(vector_t* vector, void *item);

#if defined(__cplusplus)
}
#endif