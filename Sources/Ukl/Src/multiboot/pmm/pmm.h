#pragma once

#include <math/math.h>
#include <bitmap/bitmap.h>
#include <multiboot/memory/memory.h>
#include <multiboot/core/multiboot.h>

#define PROTECT_ADDRESS              0x0 // protect the firts 16 bits address
#define PROTECT_SIZE                 0xffff

void Pmm_Init(struct multiboot_tag_mmap* Map);

uint64_t Pmm_GetMemorySize(struct multiboot_tag_mmap* Map);
void Pmm_FreePage(uintptr_t address);
void Pmm_FreePages(uintptr_t address, uint64_t pageCount);
void Pmm_LockPage(uintptr_t address);
void Pmm_LockPages(uintptr_t address, uint64_t pageCount);
void Pmm_ReservePage(uintptr_t address);
void Pmm_ReservePages(uintptr_t address, uint64_t pageCount);
void Pmm_UnreservePage(uintptr_t address);
void Pmm_UnreservePages(uintptr_t address, uint64_t pageCount);

void Pmm_FreePage_WI(uint64_t index);
void Pmm_FreePages_WI(uint64_t index, uint64_t pageCount);
void Pmm_LockPage_WI(uint64_t index);
void Pmm_LockPages_WI(uint64_t index, uint64_t pageCount);
void Pmm_ReservePage_WI(uint64_t index);
void Pmm_ReservePages_WI(uint64_t index, uint64_t pageCount);
void Pmm_UnreservePage_WI(uint64_t index);
void Pmm_UnreservePages_WI(uint64_t index, uint64_t pageCount);

void Pmm_InitBitmap(size64_t bitmapSize, uintptr_t bufferAddress);

uintptr_t Pmm_RequestPage();

struct memoryInfo_t{
    uint64_t totalPageMemory;
    uint64_t totalUsablePageMemory;
    uint64_t freePageMemory;
    uint64_t reservedPageMemory;
    uint64_t usedPageMemory;    
}__attribute__((packed));
