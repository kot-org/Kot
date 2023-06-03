#include <multiboot/memory/memory.h>

void InitializeMemory(uint32_t MbHeader, struct multiboot_tag_mmap* Map, uint64_t* Stack){
    Pmm_Init(MbHeader, Map);
    vmm_Init(Map, Stack);
}

void memcpy(void* destination, void* source, size64_t size){
    for (uint64_t i = 0; i < size; i++){
        *(uint8_t*)((uint64_t)destination + i) = *(uint8_t*)((uint64_t)source + i);
    }
}

void memset(void* start, uint8_t value, size64_t size){
    for (uint64_t i = 0; i < size; i++){
        *(uint8_t*)((uint64_t)start + i) = value;
    }
}