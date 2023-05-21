#include <kot/utils/map.h>
#include <string.h>

typedef struct{
    char* key;
    uintptr_t item;
}kot_map_item_t;

kot_vector_t* kot_map_create(){
    return kot_vector_create();
}

char* kot_map_key(kot_vector_t* map, uint64_t index) {
    kot_map_item_t* map_item = (kot_map_item_t*)kot_vector_get(map, index);
    if (map_item == NULL) return NULL;
    return map_item->key;
}

int64_t kot_map_indexof(kot_vector_t* map, char* key) {
    if (map->length > 0) {
        for (int64_t i = 0; i < map->length; i++) {
            kot_map_item_t* map_item = (kot_map_item_t*)kot_vector_get(map, i);
            if(strcmp(map_item->key, key)){
                return i;
            }
        }
    }
    return -1; 
}

void kot_map_set(kot_vector_t* map, char* key, uintptr_t item) {
    kot_map_item_t* map_item = (kot_map_item_t*) malloc(sizeof(kot_map_item_t));
    map_item->key = key;
    map_item->item = item;
    int64_t indexof = kot_map_indexof(map, key);
    if (indexof == -1) {
        kot_vector_push(map, (uintptr_t)map_item);
    } else {
        kot_vector_set(map, indexof, (uintptr_t)map_item);
    }
}

bool kot_map_exist(kot_vector_t* map, char* key) {
    if(kot_map_indexof(map, key) == -1){
        return false;
    }
    return true;
}

void kot_map_remove(kot_vector_t* map, char* key) {
    int64_t indexof = kot_map_indexof(map, key);
    if(indexof != -1){
        kot_vector_remove(map, indexof);
    }
}

uintptr_t kot_map_get(kot_vector_t* map, char* key) {
    int64_t indexof = kot_map_indexof(map, key);
    if (indexof != -1) {
        return ((kot_map_item_t*)kot_vector_get(map, indexof))->item;
    }
    return NULL;
}

uintptr_t kot_map_geti(kot_vector_t* map, uint64_t index) {
    return ((kot_map_item_t*)kot_vector_get(map, index))->item;
}