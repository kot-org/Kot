#pragma once

#include <kot/utils/vector.h>

#if defined(__cplusplus)
extern "C" {
#endif

vector_t* map_create();
void map_set(vector_t* map, char* key, uintptr_t item);
void map_remove(vector_t* map, char* key);

bool map_exist(vector_t* map, char* key);
uintptr_t map_get(vector_t* map, char* key);

#if defined(__cplusplus)
}
#endif