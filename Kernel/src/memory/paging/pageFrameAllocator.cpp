#include <memory/memory.h>
#include <lib/stdio/cstr.h>
#include <misc/bitmap/bitmap.h>
#include <memory/paging/PageFrameAllocator.h>


memoryInfo_t memoryInfo;

bool Initialized = false;
PageFrameAllocator globalAllocator;

void PageFrameAllocator::ReadMemoryMap(stivale2_struct_tag_memmap* Map){
    if (Initialized) return;

    Initialized = true;

    void* BitmapSegment = NULL;

    uint64_t memorySize = GetMemorySize(Map);
    uint64_t bitmapSize = memorySize / PAGE / 8 + 1;

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


    InitBitmap(bitmapSize, BitmapSegment);

    ReservePages(0, memorySize / PAGE + 1);

    for (int i = 0; i < Map->entries; i++){
        if (Map->memmap[i].type == STIVALE2_MMAP_USABLE && Map->memmap[i].length >= PAGE){ 
            UnreservePages((void*)Map->memmap[i].base, Map->memmap[i].length / PAGE);
        }
    }

    ReservePages(0, 0x100); // reserve between 0 and 0x100000
    LockPages(PageBitmap.Buffer, PageBitmap.Size / PAGE + 1);
}

uint64_t PageFrameAllocator::GetMemorySize(stivale2_struct_tag_memmap* Map){
    static uint64_t memorySizeBytes = 0;
    if (memorySizeBytes > 0) return memorySizeBytes;

    for (int i = 0; i < Map->entries; i++){
        memorySizeBytes += Map->memmap[i].length;
    }

    return memorySizeBytes;
}

void PageFrameAllocator::InitBitmap(size_t bitmapSize, void* bufferAddress){
    PageBitmap.Size = bitmapSize;
    PageBitmap.Buffer = (uint8_t*)bufferAddress;
    for (int i = 0; i < bitmapSize; i++){
        *(uint8_t*)(PageBitmap.Buffer + i) = 0;
    }
}

uint64_t pageBitmapIndex = 0;
void* PageFrameAllocator::RequestPage(){
    for (; pageBitmapIndex < PageBitmap.Size * 8; pageBitmapIndex++){
        if(!PageBitmap.Get(pageBitmapIndex)){
            uint64_t page = pageBitmapIndex;
            LockPage((void*)(pageBitmapIndex * 0x1000));
            return (void*)(pageBitmapIndex * 0x1000);
        }
    }
    
    return NULL; // Page Frame Swap to file
}

void* PageFrameAllocator::RequestPages(uint64_t pages){
	while(pageBitmapIndex < PageBitmap.Size) {
		for(size_t j = 0; j < pages; j++) {
			if(PageBitmap[pageBitmapIndex + j] == true) {
				pageBitmapIndex += j + 1;
				goto not_free;
			}
		}
		goto exit;
		not_free:
			continue;
		exit: {
			void* page = (void*)(pageBitmapIndex * 4096);	// transform the index into the physical page address
			pageBitmapIndex += pages;
			LockPages(page, pages);
			return page;
		}
	}
	return NULL;
}

void PageFrameAllocator::FreePage(void* address){
    uint64_t index = (uint64_t)address / 4096;
    if (PageBitmap.Get(index) == false) return;
    if (PageBitmap.Set(index, false)){
        memoryInfo.freeMemory += 4096;
        memoryInfo.usedMemory -= 4096;
        if (pageBitmapIndex > index) pageBitmapIndex = index;
    }
}

void PageFrameAllocator::FreePages(void* address, uint64_t pageCount){
    for (int t = 0; t < pageCount; t++){
        FreePage((void*)((uint64_t)address + (t * 4096)));
    }
}

void PageFrameAllocator::LockPage(void* address){
    uint64_t index = (uint64_t)address / 4096;
    if (PageBitmap.Get(index) == true) return;
    if (PageBitmap.Set(index, true)){
        memoryInfo.freeMemory -= 4096;
        memoryInfo.usedMemory += 4096;
    }
}

void PageFrameAllocator::LockPages(void* address, uint64_t pageCount){
    for (int t = 0; t < pageCount; t++){
        LockPage((void*)((uint64_t)address + (t * 4096)));
    }
}

void PageFrameAllocator::UnreservePage(void* address){
    uint64_t index = (uint64_t)address / 4096;
    if (PageBitmap.Get(index) == false) return;
    if (PageBitmap.Set(index, false)){
        memoryInfo.freeMemory += 4096;
        memoryInfo.reservedMemory -= 4096;
        if (pageBitmapIndex > index) pageBitmapIndex = index;
    }
}

void PageFrameAllocator::UnreservePages(void* address, uint64_t pageCount){
    for (int t = 0; t < pageCount; t++){
        UnreservePage((void*)((uint64_t)address + (t * 4096)));
    }
}

void PageFrameAllocator::ReservePage(void* address){
    uint64_t index = (uint64_t)address / 4096;
    if (PageBitmap.Get(index) == true) return;
    if (PageBitmap.Set(index, true)){
        memoryInfo.freeMemory -= 4096;
        memoryInfo.reservedMemory += 4096;
    }
}

void PageFrameAllocator::ReservePages(void* address, uint64_t pageCount){
    for (int t = 0; t < pageCount; t++){
        ReservePage((void*)((uint64_t)address + (t * 4096)));
    }
}

uint64_t PageFrameAllocator::GetTotalRAM(){
    return memoryInfo.totalMemory;
}

uint64_t PageFrameAllocator::GetFreeRAM(){
    return memoryInfo.freeMemory;
}
uint64_t PageFrameAllocator::GetUsedRAM(){
    return memoryInfo.usedMemory;
}
uint64_t PageFrameAllocator::GetReservedRAM(){
    return memoryInfo.reservedMemory;
}