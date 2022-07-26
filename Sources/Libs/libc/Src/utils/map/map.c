#include <kot/utils/map.h>

typedef struct {
    char* key;
    uintptr_t item;
} map_item_t;

vector_t* map_create() {
    return vector_create(sizeof(map_item_t));
}

void map_exist(vector_t* map, char* key) {
    
}

void map_set(vector_t* map, char* key, uintptr_t item) {
    map_item_t* map_item = (map_item_t*) malloc(sizeof(map_item_t));

}