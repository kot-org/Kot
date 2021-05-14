#include "PageFrameAllocator.h"
#include "../graphics/BasicRenderer.h"
#include "stalloc.h"
#include "KernelUtil.h"
#include "string.h"
#include "../../../bios/multiboot.h"

enum memmap_types_t
{
    MEMTYPE_FREE = 1,
    MEMTYPE_RESERVED,
    MEMTYPE_RECLAIM,
    MEMTYPE_NVRAM
};

typedef struct memory_region
{
    uint32_t size;
    uint64_t base_address;
    uint64_t length;
    uint32_t type;
} __attribute__((packed)) mem_region_t;

PageFrameAllocator sAllocator;
PageFrameAllocator* PageFrameAllocator::SharedAllocator() {
     return &sAllocator;
}

void PageFrameAllocator::ReadEFIMemoryMap(EFI_MEMORY_DESCRIPTOR* mMap, size_t mMapSize, size_t mMapDescSize) {
    if(_initialized) {
        return;
    }

    _initialized = true;
    uint64_t mMapEntries = mMapSize / mMapDescSize;
    uint64_t memorySize = GetMemorySize(mMap, mMapEntries, mMapDescSize);
    _freeMemory = memorySize;
    uint64_t bitmapSize = memorySize / 4096 / 8 + 1;
    void* bitmapAddr = stalloc(bitmapSize);
    _pageBitmap = Bitmap(bitmapSize, (uint8_t *)bitmapAddr);
    _pageBitmap.Clear();
    uint64_t last_address = (uint64_t)stalloc(1);
    stalloc_disable();

    ReservePages(0, memorySize / 4096 + 1);
    for(int i = 0; i < mMapEntries; i++) {
        EFI_MEMORY_DESCRIPTOR* desc = (EFI_MEMORY_DESCRIPTOR *)((uint64_t)mMap + i * mMapDescSize);
        if(desc->type == 7) { // type = EfiConventionalMemory
            UnreservePages(desc->physAddr, desc->numPages);
        }
    }

    ReservePages(0, 256);

    uint64_t ke = (uint64_t)&_KernelEnd & ~0xfff;
    uint64_t se = (last_address + 0xfff) & ~0xfff;
    ReservePages((void *)ke, (se - ke) / 0x1000);
}

void PageFrameAllocator::ReadMultibootMap(multiboot_info_t* mb_info) {
    uint64_t* mem_ptr = (uint64_t *)(uint64_t)mb_info->memory_map_addr;
    uint64_t memory_size = mb_info->memory_high + mb_info->memory_low;
    uint64_t total_blocks = (memory_size * 1024) / 0x1000;
    uint64_t bitmap_size = total_blocks / 8;
    void* mem_bitmap = stalloc(bitmap_size);
    _pageBitmap = Bitmap(bitmap_size, (uint8_t *)mem_bitmap);
    _pageBitmap.Clear();
    uint64_t last_address = (uint64_t)stalloc(1);
    stalloc_disable();


    ReservePages(0, memory_size / 4096 + 1);
    for(uint64_t itr = (uint64_t)mem_ptr; itr < ((uint64_t)mem_ptr + mb_info->memory_map_length); itr += sizeof(mem_region_t)) {
        mem_region_t* mem_region = (mem_region_t *)itr;
        if(mem_region->type == MEMTYPE_FREE) {
            if(mem_region->length >= 0x1000) {
                UnreservePages((void *)mem_region->base_address, mem_region->length / 0x1000);
            }
        }
    }

    ReservePages(0, 256);

    uint64_t ks = (uint64_t)&_KernelStart & ~0xfff;
    uint64_t se = (last_address + 0xfff) & ~0xfff;
    ReservePages((void *)ks, (se - ks + 0xfff) / 0x1000);
}

void* PageFrameAllocator::RequestPage() {
    for(; _pageBitmapIndex < _pageBitmap.GetSize() * 8; _pageBitmapIndex++) {
        if(!_pageBitmap[_pageBitmapIndex]) {
            LockPage((void *)(_pageBitmapIndex * 4096));
            return (void *)(_pageBitmapIndex * 4096);
        }
    }

    return NULL; // Page Frame Swap
}

void* PageFrameAllocator::RequestPages(uint64_t pageCount) {
    uint64_t remaining = pageCount;
    uint64_t start = _pageBitmapIndex;
    for(; _pageBitmapIndex < _pageBitmap.GetSize() * 8; _pageBitmapIndex++) {
        if(!_pageBitmap[_pageBitmapIndex]) {
            if(--remaining == 0) {
                LockPages((void *)(start * 4096), pageCount);
                return (void *)(start * 4096);
            }
        } else {
            remaining = pageCount;
            start = _pageBitmapIndex + 1;
        }
    }

    return nullptr;
}

void PageFrameAllocator::FreePage(void* address) {
    uint64_t index = (uint64_t)address / 4096;
    if(!_pageBitmap[index]) {
        return;
    }

    if(_pageBitmap.Set(index, false)) {
        _freeMemory += 4096;
        _usedMemory -= 4096;
        if(_pageBitmapIndex > index) {
            _pageBitmapIndex = index;
        }
    }
}

void PageFrameAllocator::FreePages(void* address, uint64_t pageCount) {
    for(int t = 0; t < pageCount; t++) {
        FreePage((void *)((uint64_t)address + (t * 4096)));
    }
}

void PageFrameAllocator::LockPage(void* address) {
    uint64_t index = (uint64_t)address / 4096;
    if(_pageBitmap[index]) {
        return;
    }

    if(_pageBitmap.Set(index, true)) {
        _freeMemory -= 4096;
        _usedMemory += 4096;
    }
}

void PageFrameAllocator::LockPages(void* address, uint64_t pageCount) {
    for(int t = 0; t < pageCount; t++) {
        LockPage((void *)((uint64_t)address + (t * 4096)));
    }
}

void PageFrameAllocator::UnreservePage(void* address) {
    uint64_t index = (uint64_t)address / 4096;
    if(!_pageBitmap[index]) {
        return;
    }

    if(_pageBitmap.Set(index, false)) {
        _freeMemory += 4096;
        _reservedMemory -= 4096;
        if(_pageBitmapIndex > index) {
            _pageBitmapIndex = index;
        }
    }
}

void PageFrameAllocator::UnreservePages(void* address, uint64_t pageCount) {
    for(int t = 0; t < pageCount; t++) {
        UnreservePage((void *)((uint64_t)address + (t * 4096)));
    }
}

void PageFrameAllocator::ReservePage(void* address) {
    uint64_t index = (uint64_t)address / 4096;
    if(_pageBitmap[index]) {
        return;
    }

    if(_pageBitmap.Set(index, true)) {
        _freeMemory -= 4096;
        _reservedMemory += 4096;
    }
}

void PageFrameAllocator::ReservePages(void* address, uint64_t pageCount) {
    for(int t = 0; t < pageCount; t++) {
        ReservePage((void *)((uint64_t)address + (t * 4096)));
    }
}