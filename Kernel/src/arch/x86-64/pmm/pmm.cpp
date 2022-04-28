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

    Pmm_ReservePages(0, PageCount);

    for (int i = 0; i < Map->entries; i++){
        if (Map->memmap[i].type == STIVALE2_MMAP_USABLE){ 
            Pmm_UnreservePages((void*)Map->memmap[i].base, Map->memmap[i].length / PAGE_SIZE);
        }
    }

    Pmm_LockPages(Pmm_PageBitmap.Buffer, DivideRoundUp(Pmm_PageBitmap.Size, PAGE_SIZE));
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
    memset(Pmm_PageBitmap.Buffer, 0, Pmm_PageBitmap.Size);
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