#pragma once

#include <kot/utils/vector.h>

#if defined(__cplusplus)
extern "C" {
#endif

kot_vector_t* map_create();
void map_set(kot_vector_t* map, char* key, uintptr_t item);
void map_remove(kot_vector_t* map, char* key);
int64_t map_indexof(kot_vector_t* map, char* key);
char* map_key(kot_vector_t* map, uint64_t index);
bool map_exist(kot_vector_t* map, char* key);
uintptr_t map_get(kot_vector_t* map, char* key);
uintptr_t map_geti(kot_vector_t* map, uint64_t index);

#if defined(__cplusplus)
}
#endif