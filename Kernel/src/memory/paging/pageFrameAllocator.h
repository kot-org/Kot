#pragma once
#include <main/kernelInit.h>
#include <misc/bitmap/bitmap.h>

#define PAGE 0x1000

class PageFrameAllocator {
    public:
        Bitmap PageBitmap;
        void ReadMemoryMap(struct stivale2_struct_tag_memmap* Map);
        uint64_t GetMemorySize(struct stivale2_struct_tag_memmap* Map);
        void FreePage(void* address);
        void FreePages(void* address, uint64_t pageCount);
        void LockPage(void* address);
        void LockPages(void* address, uint64_t pageCount);
        void* RequestPage();
        void* RequestPages(uint64_t pages);
        uint64_t GetTotalRAM();
        uint64_t GetFreeRAM();
        uint64_t GetUsedRAM();
        uint64_t GetReservedRAM();


    private:
        void InitBitmap(size_t bitmapSize, void* bufferAddress);
        void ReservePage(void* address);
        void ReservePages(void* address, uint64_t pageCount);
        void UnreservePage(void* address);
        void UnreservePages(void* address, uint64_t pageCount);

};

struct memoryInfo_t{
    uint64_t totalMemory;
    uint64_t freeMemory;
    uint64_t reservedMemory;
    uint64_t usedMemory;    
}__attribute__((packed));

extern PageFrameAllocator globalAllocator;
extern memoryInfo_t memoryInfo;