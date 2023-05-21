#ifndef KOT_UTILS_MAP_H
#define KOT_UTILS_MAP_H 1

#include <kot/utils/vector.h>

#if defined(__cplusplus)
extern "C" {
#endif

kot_vector_t* kot_map_create();
void kot_map_set(kot_vector_t* map, char* key, uintptr_t item);
void kot_map_remove(kot_vector_t* map, char* key);
int64_t kot_map_indexof(kot_vector_t* map, char* key);
char* kot_map_key(kot_vector_t* map, uint64_t index);
bool kot_map_exist(kot_vector_t* map, char* key);
uintptr_t kot_map_get(kot_vector_t* map, char* key);
uintptr_t kot_map_geti(kot_vector_t* map, uint64_t index);

#if defined(__cplusplus)
}
#endif

#endif