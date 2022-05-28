#pragma once
#include <misc/bitmap/bitmap.h>

extern Bitmap Pmm_PageBitmap;
void Pmm_Init(struct stivale2_struct_tag_memmap* Map);
uint64_t Pmm_GetMemorySize(struct stivale2_struct_tag_memmap* Map);
void Pmm_FreePage(uintptr_t address);
void Pmm_FreePages(uintptr_t address, uint64_t pageCount);
void Pmm_LockPage(uintptr_t address);
void Pmm_LockPages(uintptr_t address, uint64_t pageCount);
uintptr_t Pmm_RequestPage();
uintptr_t Pmm_RequestPages(uint64_t pages);
uint64_t Pmm_GetTotalRAM();
uint64_t Pmm_GetFreeRAM();
uint64_t Pmm_GetUsedRAM();
uint64_t Pmm_GetReservedRAM();

void Pmm_InitBitmap(size_t bitmapSize, uintptr_t bufferAddress);

void Pmm_ReservePage(uintptr_t address);
void Pmm_ReservePages(uintptr_t address, uint64_t pageCount);
void Pmm_UnreservePage(uintptr_t address);
void Pmm_UnreservePages(uintptr_t address, uint64_t pageCount);

struct memoryInfo_t{
    uint64_t totalMemory;
    uint64_t freeMemory;
    uint64_t reservedMemory;
    uint64_t usedMemory;    
}__attribute__((packed));

extern memoryInfo_t memoryInfo;