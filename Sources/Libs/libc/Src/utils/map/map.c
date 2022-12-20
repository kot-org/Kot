#include <kot/utils/map.h>
#include <kot/cstring.h>

typedef struct {
    char* key;
    uintptr_t item;
} map_item_t;

vector_t* map_create() {
    return vector_create(sizeof(map_item_t));
}

char* map_key(vector_t* map, uint64_t index) {
    map_item_t* map_item = (map_item_t*) vector_get(map, index);
    if (map_item == NULL) return NULL;
    return map_item->key;
}

int64_t map_indexof(vector_t* map, char* key) {
    if (map->length > 0) {
        for (int64_t i = 0; i < map->length; i++) {
            map_item_t* map_item = (map_item_t*) vector_get(map, i);
            if (strcmp(map_item->key, key)) {
                return i;
            }
        }
    }
    return -1; 
}

void map_set(vector_t* map, char* key, uintptr_t item) {
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

bool map_exist(vector_t* map, char* key) {
    if (map_indexof(map, key) == -1) {
        return false;
    }
    return true;
}

void map_remove(vector_t* map, char* key) {
    int64_t indexof = map_indexof(map, key);
    if (indexof != -1) {
        vector_remove(map, indexof);
    }
}

uintptr_t map_get(vector_t* map, char* key) {
    int64_t indexof = map_indexof(map, key);
    if (indexof != -1) {
        return ((map_item_t*) vector_get(map, indexof))->item;
    }
    return NULL;
}

uintptr_t map_geti(vector_t* map, uint64_t index) {
    return ((map_item_t*) vector_get(map, index))->item;
}