#include <kot/utils/map.h>

typedef struct {
    uint64_t key;
    uintptr_t item;
} map_item_t;

vector_t* map_create() {
    return vector_create(sizeof(map_item_t));
}

int64_t map_indexof(vector_t* map, uint64_t key) {
    if (map->length > 0) {
        for (int64_t i = 0; i < map->length; i++) {
            map_item_t* map_item = (map_item_t*) vector_get(map, i);
            if (map_item->key == key) {
                return i;
            }
        }
    }
    return -1; 
}

void map_set(vector_t* map, uint64_t key, uintptr_t item) {
    map_item_t* map_item = (map_item_t*) malloc(sizeof(map_item_t));
    map_item->key = key;
    map_item->item = item;
    int64_t indexof = map_indexof(map, key);
    if (indexof == -1) {
        vector_push(map, map_item);
    } else {
        vector_set(map, indexof, map_item);
    }
}

bool map_exist(vector_t* map, uint64_t key) {
    if (map_indexof(map, key) == -1) {
        return false;
    }
    return true;
}

void map_remove(vector_t* map, uint64_t key) {
    int64_t indexof = map_indexof(map, key);
    if (indexof != -1) {
        vector_remove(map, indexof);
    }
}

uintptr_t map_get(vector_t* map, uint64_t key) {
    int64_t indexof = map_indexof(map, key);
    if (indexof != -1) {
        return ((map_item_t*) vector_get(map, indexof))->item;
    }
    return NULL;
}