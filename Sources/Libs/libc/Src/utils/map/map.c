#include <kot/utils/map.h>
#include <string.h>

typedef struct {
    char* key;
    void* item;
} map_item_t;

kot_vector_t* map_create() {
    return vector_create(sizeof(map_item_t));
}

char* map_key(kot_vector_t* map, uint64_t index) {
    map_item_t* map_item = (map_item_t*) vector_get(map, index);
    if (map_item == NULL) return NULL;
    return map_item->key;
}

int64_t map_indexof(kot_vector_t* map, char* key) {
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

void map_set(kot_vector_t* map, char* key, void* item) {
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

bool map_exist(kot_vector_t* map, char* key) {
    if (map_indexof(map, key) == -1) {
        return false;
    }
    return true;
}

void map_remove(kot_vector_t* map, char* key) {
    int64_t indexof = map_indexof(map, key);
    if (indexof != -1) {
        vector_remove(map, indexof);
    }
}

void* map_get(kot_vector_t* map, char* key) {
    int64_t indexof = map_indexof(map, key);
    if (indexof != -1) {
        return ((map_item_t*) vector_get(map, indexof))->item;
    }
    return NULL;
}

void* map_geti(kot_vector_t* map, uint64_t index) {
    return ((map_item_t*) vector_get(map, index))->item;
}