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

    void* BitmapSegment = NULL;

    uint64_t memorySize = Pmm_GetMemorySize(Map);
    uint64_t bitmapSize = memorySize / PAGE_SIZE / 8 + 1;

    for (int i = 0; i < Map->entries; i++){
        if (Map->memmap[i].type == STIVALE2_MMAP_USABLE){
            if (Map->memmap[i].length > bitmapSize)
            {
                BitmapSegment = (void*)Map->memmap[i].base;
                break;
            }
        }
    }

    memoryInfo.freeMemory = memorySize;
    memoryInfo.totalMemory = memorySize;


    Pmm_InitBitmap(bitmapSize, BitmapSegment);

    Pmm_ReservePages(0, memorySize / PAGE_SIZE + 1);

    for (int i = 0; i < Map->entries; i++){
        if (Map->memmap[i].type == STIVALE2_MMAP_USABLE){ 
            Pmm_UnreservePages((void*)Map->memmap[i].base, Map->memmap[i].length / PAGE_SIZE);
        }
    }

    Pmm_ReservePages(0, 0x100); // reserve between 0 and PAGE_SIZE00
    Pmm_LockPages(Pmm_PageBitmap.Buffer, Pmm_PageBitmap.Size / PAGE_SIZE + 1);
}

uint64_t Pmm_GetMemorySize(stivale2_struct_tag_memmap* Map){
    static uint64_t memorySizeBytes = 0;
    if (memorySizeBytes > 0) return memorySizeBytes;

    for (uint64_t i = 0; i < Map->entries; i++){
        memorySizeBytes += Map->memmap[i].length;
    }

    return memorySizeBytes;
}

void Pmm_InitBitmap(size_t bitmapSize, void* bufferAddress){
    Pmm_PageBitmap.Size = bitmapSize;
    Pmm_PageBitmap.Buffer = (uint8_t*)bufferAddress;
    for (int i = 0; i < bitmapSize; i++){
        *(uint8_t*)(Pmm_PageBitmap.Buffer + i) = 0;
    }
}

uint64_t Pmm_PageBitmapIndex = 0;
void* Pmm_RequestPage(){
    for (; Pmm_PageBitmapIndex < Pmm_PageBitmap.Size * 8; Pmm_PageBitmapIndex++){
        if(!Pmm_PageBitmap.Get(Pmm_PageBitmapIndex)){
            uint64_t page = Pmm_PageBitmapIndex;
            Pmm_LockPage((void*)(Pmm_PageBitmapIndex * PAGE_SIZE));
            return (void*)(Pmm_PageBitmapIndex * PAGE_SIZE);
        }
    }
    
    return NULL; // Page Frame Swap to file
}

void* Pmm_RequestPages(uint64_t pages){
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
			void* page = (void*)(Pmm_PageBitmapIndex * 4096);	// transform the index into the physical page address
			Pmm_PageBitmapIndex += pages;
			Pmm_LockPages(page, pages);
			return page;
		}
	}
	return NULL;
}

void Pmm_FreePage(void* address){
    uint64_t index = (uint64_t)address / 4096;
    if (Pmm_PageBitmap.Get(index) == false) return;
    if (Pmm_PageBitmap.Set(index, false)){
        memoryInfo.freeMemory += 4096;
        memoryInfo.usedMemory -= 4096;
        if (Pmm_PageBitmapIndex > index) Pmm_PageBitmapIndex = index;
    }
}

void Pmm_FreePages(void* address, uint64_t pageCount){
    for (int t = 0; t < pageCount; t++){
        Pmm_FreePage((void*)((uint64_t)address + (t * 4096)));
    }
}

void Pmm_LockPage(void* address){
    uint64_t index = (uint64_t)address / 4096;
    if (Pmm_PageBitmap.Get(index) == true) return;
    if (Pmm_PageBitmap.Set(index, true)){
        memoryInfo.freeMemory -= 4096;
        memoryInfo.usedMemory += 4096;
    }
}

void Pmm_LockPages(void* address, uint64_t pageCount){
    for (int t = 0; t < pageCount; t++){
        Pmm_LockPage((void*)((uint64_t)address + (t * 4096)));
    }
}

void Pmm_UnreservePage(void* address){
    uint64_t index = (uint64_t)address / 4096;
    if (Pmm_PageBitmap.Get(index) == false) return;
    if (Pmm_PageBitmap.Set(index, false)){
        memoryInfo.freeMemory += 4096;
        memoryInfo.reservedMemory -= 4096;
        if (Pmm_PageBitmapIndex > index) Pmm_PageBitmapIndex = index;
    }
}

void Pmm_UnreservePages(void* address, uint64_t pageCount){
    for (int t = 0; t < pageCount; t++){
        Pmm_UnreservePage((void*)((uint64_t)address + (t * 4096)));
    }
}

void Pmm_ReservePage(void* address){
    uint64_t index = (uint64_t)address / 4096;
    if (Pmm_PageBitmap.Get(index) == true) return;
    if (Pmm_PageBitmap.Set(index, true)){
        memoryInfo.freeMemory -= 4096;
        memoryInfo.reservedMemory += 4096;
    }
}

void Pmm_ReservePages(void* address, uint64_t pageCount){
    for (int t = 0; t < pageCount; t++){
        Pmm_ReservePage((void*)((uint64_t)address + (t * 4096)));
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