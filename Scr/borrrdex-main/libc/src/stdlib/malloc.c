#include "stdlib.h"
#include "stdint.h"

#define HEAP_START 0x90000000
#define HEAP_START_SZ 8 // Pages

typedef struct free_block {
  size_t size;
  struct free_block *next;
} free_block_t;

free_block_t *free_list = NULL;

void* malloc(size_t sz) {
    if(!free_list) {
        free_list = (free_block_t *)HEAP_START;
        free_list->size = HEAP_START_SZ * 0x1000;
        free_list->next = NULL;
    }

    if(sz == 0) {
        return NULL;
    }

    sz = (sz + sizeof(free_block_t) + 4) & ~3ULL;
    free_block_t** prev = &free_list;
    for(free_block_t* block = free_list; block; prev = &(block->next), block = block->next) {
        if((block->size - sz - sizeof(size_t)) >= sizeof(free_block_t) + sizeof(size_t)) {
            block->size -= sz + sizeof(size_t);
            free_block_t* new_block = (free_block_t *)(((uint8_t *)block) + block->size);
            new_block->size = sz + sizeof(size_t);
            return ((uint8_t *)new_block)+sizeof(size_t);
        }
    }
}