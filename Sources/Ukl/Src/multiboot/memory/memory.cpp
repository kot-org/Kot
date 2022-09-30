#include <multiboot/memory/memory.h>

void InitializeMemory(struct multiboot_tag_mmap* Map){
    Pmm_Init(Map);
    vmm_Init(Map);
}

void memcpy(uintptr_t destination, uintptr_t source, size64_t size){
    for (uint64_t i = 0; i < size; i++){
        *(uint8_t*)((uint64_t)destination + i) = *(uint8_t*)((uint64_t)source + i);
    } 
}

void memset(uintptr_t start, uint8_t value, size64_t size){
    for (uint64_t i = 0; i < size; i++){
        *(uint8_t*)((uint64_t)start + i) = value;
    }
}