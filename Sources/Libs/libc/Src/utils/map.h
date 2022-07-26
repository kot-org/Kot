#include <kot/utils/vector.h>

#if defined(__cplusplus)
extern "C" {
#endif

vector_t* map_create();
void map_set(vector_t* map, char* key, uintptr_t item);

#if defined(__cplusplus)
}
#endif