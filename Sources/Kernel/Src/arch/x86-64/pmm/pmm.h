#pragma once
#include <misc/bitmap/bitmap.h>

extern Bitmap Pmm_PageBitmap;
void Pmm_Init(struct ukl_memmap_t* Map);

uint64_t Pmm_GetMemorySize(struct ukl_memmap_t* Map);
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
uintptr_t Pmm_RequestPage();
uintptr_t Pmm_RequestPages(uint64_t pages);
uint64_t Pmm_GetTotalRAM();
uint64_t Pmm_GetFreeRAM();
uint64_t Pmm_GetUsedRAM();
uint64_t Pmm_GetReservedRAM();

void Pmm_InitBitmap(size64_t bitmapSize, uintptr_t bufferAddress);


struct memoryInfo_t{
    uint64_t totalPageMemory;
    uint64_t totalUsablePageMemory;
    uint64_t freePageMemory;
    uint64_t reservedPageMemory;
    uint64_t usedPageMemory;    
}__attribute__((packed));

struct freelistinfoend_t{
    struct freelistinfoend_t* End;
    struct freelistinfo_t* Start;
}__attribute__((packed));

struct freelistinfo_t{
    struct freelistinfoend_t Header;
    uint64_t IndexStart;
    uint64_t IndexEnd;
    uint64_t PageSize;
    struct freelistinfo_t* Last;
    struct freelistinfo_t* Next;
}__attribute__((packed));

extern memoryInfo_t Pmm_MemoryInfo;