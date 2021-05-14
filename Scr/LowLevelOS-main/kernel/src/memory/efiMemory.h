#pragma once
#include <stdint.h>
#include <stddef.h>

struct EFI_MEMORY_DESCRIPTOR {
    uint32_t type;
    void* physAddr;
    void* virtAddr; 
    uint64_t numPages;
    uint64_t attribs;
};

extern const char* EFI_MEMORY_TYPE_STRINGS[];

uint64_t GetMemorySize(EFI_MEMORY_DESCRIPTOR* nMap, uint64_t nMapEntries, uint64_t nMapDescSize);
void memset(void* start, uint64_t value, uint64_t num);

void memcpy(void* destination, void* source, uint64_t num);
void ssememcpy(void* dst, void* src, size_t size);

void slowmemset(void* start, uint8_t value, uint64_t num);
void asmmemcpy(void *d, const void *s, size_t n);