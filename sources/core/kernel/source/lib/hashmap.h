#ifndef _HASHMAP_H
#define _HASHMAP_H 1

#include <stddef.h>

typedef void* hashmap_t;

hashmap_t* hashmap_create(size_t size);

void hashmap_set(hashmap_t* map, const void* key, void* value);

void* hashmap_get(hashmap_t* map, const void* key);

void hashmap_remove(hashmap_t* map, const void* key);

void hashmap_destroy(hashmap_t* map);

void* hashmap_get_key(hashmap_t* map, uint64_t iterator);

void* hashmap_get_key_with_key(hashmap_t* map, const void* key);

uint64_t hashmap_get_start(hashmap_t* map);

uint64_t hashmap_get_end(hashmap_t* map);
#endif // _HASHMAP_H
