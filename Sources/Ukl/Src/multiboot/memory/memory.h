#define PAGE_SIZE (uint64_t)0x1000

#pragma once
#include <multiboot/pmm/pmm.h>
#include <multiboot/vmm/vmm.h>

void InitializeMemory(uint32_t MbHeader, struct multiboot_tag_mmap* Map, uint64_t* Stack);

void memcpy(void* destination, void* source, size64_t size);

void memset(void* start, uint8_t value, size64_t size);
