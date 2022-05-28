#include <memory/memory.h>
#include <lib/stdio/cstr.h>
#include <misc/bitmap/bitmap.h>
#include <arch/arch.h>


memoryInfo_t memoryInfo;
Bitmap Pmm_PageBitmap;

bool Initialized = false;

void Pmm_Init(stivale2_struct_tag_memmap* Map){
    if (Initialized) return;

    Initialized = true;

    uintptr_t BitmapSegment = NULL;

    uint64_t memorySize = Pmm_GetMemorySize(Map);
    uint64_t PageCount = 0;
    uint64_t LastBase = 0;
    for (int i = 0; i < Map->entries; i++){
        if(Map->memmap[i].base > LastBase){
            LastBase = Map->memmap[i].base;
            PageCount = DivideRoundUp(Map->memmap[i].base + Map->memmap[i].length, PAGE_SIZE);
        }
    }

    uint64_t bitmapSize = DivideRoundUp(PageCount, 8);

    for (int i = 0; i < Map->entries; i++){
        if (Map->memmap[i].type == STIVALE2_MMAP_USABLE){
            if (Map->memmap[i].length > bitmapSize){
                BitmapSegment = (uintptr_t)Map->memmap[i].base;
                break;
            }
        }
    }

    memoryInfo.freeMemory = memorySize;
    memoryInfo.totalMemory = memorySize;


    Pmm_InitBitmap(bitmapSize, BitmapSegment);

    Pmm_ReservePages(0, PageCount);

    for (uint64_t i = 0; i < Map->entries; i++){
        if (Map->memmap[i].type == STIVALE2_MMAP_USABLE){ 
            Pmm_UnreservePages((uintptr_t)Map->memmap[i].base, Map->memmap[i].length / PAGE_SIZE);
        }
    }
    Pmm_LockPages(BitmapSegment, DivideRoundUp(Pmm_PageBitmap.Size, PAGE_SIZE));
}

uint64_t Pmm_GetMemorySize(stivale2_struct_tag_memmap* Map){
    static uint64_t memorySizeBytes = 0;
    if (memorySizeBytes > 0) return memorySizeBytes;

    for (uint64_t i = 0; i < Map->entries; i++){
        memorySizeBytes += Map->memmap[i].length;
    }

    return memorySizeBytes;
}

void Pmm_InitBitmap(size_t bitmapSize, uintptr_t bufferAddress){
    Pmm_PageBitmap.Size = bitmapSize;
    Pmm_PageBitmap.Buffer = (uint8_t*)bufferAddress;
    memset(Pmm_PageBitmap.Buffer, 0xff, Pmm_PageBitmap.Size);
}

uint64_t Pmm_PageBitmapIndex = 0;
uintptr_t Pmm_RequestPage(){
    for (; Pmm_PageBitmapIndex < Pmm_PageBitmap.Size * 8; Pmm_PageBitmapIndex++){
        if(!Pmm_PageBitmap.Get(Pmm_PageBitmapIndex)){
            uint64_t page = Pmm_PageBitmapIndex;
            Pmm_LockPage((uintptr_t)(Pmm_PageBitmapIndex * PAGE_SIZE));
            return (uintptr_t)(Pmm_PageBitmapIndex * PAGE_SIZE);
        }
    }
    
    return NULL; // Page Frame Swap to file
}

uintptr_t Pmm_RequestPages(uint64_t pages){
	while(Pmm_PageBitmapIndex < Pmm_PageBitmap.Size) {
		for(size_t j = 0; j < pages; j++) {
			if(Pmm_PageBitmap[Pmm_PageBitmapIndex + j] == true) {
				Pmm_PageBitmapIndex += j + 1;
				goto not_free;
			}
		}
		goto exit;
		not_free:
			continue;
		exit: {
			uintptr_t page = (uintptr_t)(Pmm_PageBitmapIndex * PAGE_SIZE);	// transform the index into the physical page address
			Pmm_PageBitmapIndex += pages;
			Pmm_LockPages(page, pages);
			return page;
		}
	}
	return NULL;
}

void Pmm_FreePage(uintptr_t address){
    uint64_t index = (uint64_t)address / PAGE_SIZE;
    if (Pmm_PageBitmap.Get(index) == false) return;
    if (Pmm_PageBitmap.Set(index, false)){
        memoryInfo.freeMemory += PAGE_SIZE;
        memoryInfo.usedMemory -= PAGE_SIZE;
        if (Pmm_PageBitmapIndex > index) Pmm_PageBitmapIndex = index;
    }
}

void Pmm_FreePages(uintptr_t address, uint64_t pageCount){
    for (int t = 0; t < pageCount; t++){
        Pmm_FreePage((uintptr_t)((uint64_t)address + (t * PAGE_SIZE)));
    }
}

void Pmm_LockPage(uintptr_t address){
    uint64_t index = (uint64_t)address / PAGE_SIZE;
    if (Pmm_PageBitmap.Get(index)) return;
    if (Pmm_PageBitmap.Set(index, true)){
        memoryInfo.freeMemory -= PAGE_SIZE;
        memoryInfo.usedMemory += PAGE_SIZE;
    }
}

void Pmm_LockPages(uintptr_t address, uint64_t pageCount){
    for (uint64_t t = 0; t < pageCount; t++){
        Pmm_LockPage((uintptr_t)((uint64_t)address + (t * PAGE_SIZE)));
    }
}

void Pmm_UnreservePage(uintptr_t address){
    uint64_t index = (uint64_t)address / PAGE_SIZE;
    if (Pmm_PageBitmap.Get(index) == false) return;
    if (Pmm_PageBitmap.Set(index, false)){
        memoryInfo.freeMemory += PAGE_SIZE;
        memoryInfo.reservedMemory -= PAGE_SIZE;
        if (Pmm_PageBitmapIndex > index) Pmm_PageBitmapIndex = index;
    }
}

void Pmm_UnreservePages(uintptr_t address, uint64_t pageCount){
    for (uint64_t t = 0; t < pageCount; t++){
        Pmm_UnreservePage((uintptr_t)((uint64_t)address + (t * PAGE_SIZE)));
    }
}

void Pmm_ReservePage(uintptr_t address){
    uint64_t index = (uint64_t)address / PAGE_SIZE;
    if (Pmm_PageBitmap.Get(index) == true) return;
    if (Pmm_PageBitmap.Set(index, true)){
        memoryInfo.freeMemory -= PAGE_SIZE;
        memoryInfo.reservedMemory += PAGE_SIZE;
    }
}

void Pmm_ReservePages(uintptr_t address, uint64_t pageCount){
    for (uint64_t t = 0; t < pageCount; t++){
        Pmm_ReservePage((uintptr_t)((uint64_t)address + (t * PAGE_SIZE)));
    }
}

uint64_t Pmm_GetTotalRAM(){
    return memoryInfo.totalMemory;
}

uint64_t Pmm_GetFreeRAM(){
    return memoryInfo.freeMemory;
}
uint64_t Pmm_GetUsedRAM(){
    return memoryInfo.usedMemory;
}
uint64_t Pmm_GetReservedRAM(){
    return memoryInfo.reservedMemory;
}