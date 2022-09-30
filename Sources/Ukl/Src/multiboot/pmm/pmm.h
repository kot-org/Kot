#pragma once

#include <lib/math.h>
#include <multiboot/memory/memory.h>
#include <multiboot/core/multiboot.h>

#define PROTECT_ADDRESS              0x0 // protect the firts 16 bits address
#define PROTECT_SIZE                 0x10000

void Pmm_Init(struct multiboot_tag_mmap* Map);

uint64_t Pmm_GetMemorySize(struct multiboot_tag_mmap* Map);

void Pmm_InitBitmap(size64_t bitmapSize, uintptr_t bufferAddress);

uintptr_t Pmm_RequestPage();