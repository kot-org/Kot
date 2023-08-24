#include <stdint.h>
#include <stddef.h>
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
    if(i == kh_end((khash_t(str)*)map)){
        return NULL; 
    }else{
        return kh_value((khash_t(str)*)map, i);
    }
}

void hashmap_remove(hashmap_t* map, const void* key){
    khint_t i = kh_get(str, (khash_t(str)*)map, key);
    if(i != kh_end((khash_t(str)*)map)){
        kh_del(str, (khash_t(str)*)map, i);
    }
}

void hashmap_destroy(hashmap_t* map){
    kh_destroy(str, (khash_t(str)*)map);
}
