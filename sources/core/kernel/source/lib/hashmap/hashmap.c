#include <stdint.h>
#include <stddef.h>
#include <lib/log.h>
#include <lib/memory.h>
#include <lib/string.h>
#include <lib/hashmap.h>
#include <global/heap.h>

#include <lib/hashmap/khash.h>
 
KHASH_MAP_INIT_STR(str, void*)

hashmap_t* hashmap_create(size_t size){
    return (hashmap_t*)kh_init(str);
}

void hashmap_set(hashmap_t* map, const void* key, void* value){
    int ret;
    khint_t i = kh_put(str, (khash_t(str)*)map, key, &ret);
    kh_value((khash_t(str)*)map, i) = value;
}

void* hashmap_get(hashmap_t* map, const void* key){
    khint_t i = kh_get(str, (khash_t(str)*)map, key);
    if(i == kh_end((khash_t(str)*)map) || !kh_exist((khash_t(str)*)map, i)){
        return NULL; 
    }else{
        return kh_value((khash_t(str)*)map, i);
    }
}

void hashmap_remove(hashmap_t* map, const void* key){
    khint_t i = kh_get(str, (khash_t(str)*)map, key);
    if(i < kh_end((khash_t(str)*)map)){
        kh_del(str, (khash_t(str)*)map, i);
    }
}

void hashmap_destroy(hashmap_t* map){
    kh_destroy(str, (khash_t(str)*)map);
}

void* hashmap_get_key(hashmap_t* map, uint64_t iterator){
    if(iterator < kh_end((khash_t(str)*)map)){
        if(kh_exist((khash_t(str)*)map, (khint_t)iterator)){
            return (void*)kh_key((khash_t(str)*)map, (khint_t)iterator);
        }
    }
    return NULL;
}

void* hashmap_get_key_with_key(hashmap_t* map, const void* key){
    khint_t i = kh_get(str, (khash_t(str)*)map, key);
    if(i < kh_end((khash_t(str)*)map) || !kh_exist((khash_t(str)*)map, i)){
        return (void*)kh_key((khash_t(str)*)map, (khint_t)i);
    }

    return NULL;
}

uint64_t hashmap_get_start(hashmap_t* map){
    return (uint64_t)kh_begin((khash_t(str)*)map);
}

uint64_t hashmap_get_end(hashmap_t* map){
    return (uint64_t)kh_end((khash_t(str)*)map);
}