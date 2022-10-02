#pragma once

#include <lib/math.h>
#include <multiboot/memory/memory.h>
#include <multiboot/core/multiboot.h>

void Pmm_Init(uint32_t MbHeader, struct multiboot_tag_mmap* Map);

uint64_t Pmm_GetMemorySize(struct multiboot_tag_mmap* Map);

void Pmm_InitBitmap(size64_t bitmapSize, uint64_t bufferAddress);

uint64_t Pmm_RequestPage();