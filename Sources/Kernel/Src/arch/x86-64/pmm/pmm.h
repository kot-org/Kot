#pragma once
#include <misc/bitmap/bitmap.h>

extern Bitmap Pmm_Bitmap;
void Pmm_Init(struct ukl_memmory_info_t* Map);

uint64_t Pmm_GetMemorySize(struct ukl_memmory_info_t* Map);
void Pmm_FreePage(void* address);
void Pmm_FreePages(void* address, uint64_t pageCount);
void Pmm_LockPage(void* address);
void Pmm_LockPages(void* address, uint64_t pageCount);
void Pmm_ReservePage(void* address);
void Pmm_ReservePages(void* address, uint64_t pageCount);
void Pmm_UnreservePage(void* address);
void Pmm_UnreservePages(void* address, uint64_t pageCount);

void Pmm_FreePage_WI(uint64_t index);
void Pmm_FreePages_WI(uint64_t index, uint64_t pageCount);
void Pmm_LockPage_WI(uint64_t index);
void Pmm_LockPages_WI(uint64_t index, uint64_t pageCount);
void Pmm_ReservePage_WI(uint64_t index);
void Pmm_ReservePages_WI(uint64_t index, uint64_t pageCount);
void Pmm_UnreservePage_WI(uint64_t index);
void Pmm_UnreservePages_WI(uint64_t index, uint64_t pageCount);
void* Pmm_RequestPage();
void* Pmm_RequestPages(uint64_t pages);
uint64_t Pmm_GetTotalRAM();
uint64_t Pmm_GetFreeRAM();
uint64_t Pmm_GetUsedRAM();
uint64_t Pmm_GetReservedRAM();

void Pmm_InitBitmap(void* bufferAddress, size64_t bitmapSize);


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
    /* We put it on a header because if the segment is only one page long the end and start struct is at the same address */
    struct freelistinfoend_t Header;
    uint64_t IndexStart;
    uint64_t IndexEnd;
    uint64_t PageSize;
    struct freelistinfo_t* Last;
    struct freelistinfo_t* Next;
}__attribute__((packed));

extern memoryInfo_t Pmm_MemoryInfo;