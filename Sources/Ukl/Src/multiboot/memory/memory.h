#define PAGE_SIZE (uint64_t)0x1000

#pragma once
#include <multiboot/pmm/pmm.h>
#include <multiboot/vmm/vmm.h>

void InitializeMemory(struct multiboot_tag_mmap* Map);
void memset(uintptr_t start, uint8_t value, size64_t size);
