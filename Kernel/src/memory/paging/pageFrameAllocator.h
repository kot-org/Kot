#pragma once
#include "../../misc/bitmap/bitmap.h"
#include "../efiMemory.h"


class PageFrameAllocator {
    public:
        Bitmap PageBitmap;
        void ReadEFIMemoryMap(EFI_MEMORY_DESCRIPTOR* mMap, size_t mMapSize, size_t mMapDescSize);
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