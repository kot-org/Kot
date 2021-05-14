#pragma once

#include "../uefi/EfiMemory.h"
#include "../Bitmap.h"
#include "../../bios/multiboot.h"
#include "memory/Memory.h"
#include <stdint.h>

class PageFrameAllocator {
public:
    void ReadEFIMemoryMap(EFI_MEMORY_DESCRIPTOR* mMap, size_t mMapSize, size_t mMapDescSize);
    void ReadMultibootMap(multiboot_info_t* mb_info);
    void FreePage(void* address);
    void FreePages(void* address, uint64_t pageCount);
    void LockPage(void* address);
    void LockPages(void* address, uint64_t pageCount);
    void* RequestPage();
    void* RequestPages(uint64_t pageCount);

    uint64_t GetFreeRAM() const { return _freeMemory; }
    uint64_t GetUsedRAM() const { return _usedMemory; }
    uint64_t GetReservedRAM() const { return _reservedMemory; }

    static PageFrameAllocator* SharedAllocator();

private:
    void ReservePage(void* address);
    void ReservePages(void* address, uint64_t pageCount);
    void UnreservePage(void* address);
    void UnreservePages(void* address, uint64_t pageCount);

    Bitmap _pageBitmap;
    uint64_t _freeMemory;
    uint64_t _reservedMemory{0};
    uint64_t _usedMemory{0};
    uint64_t _pageBitmapIndex{0};
    bool _initialized {false};
};