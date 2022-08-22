#include <memory/memory.h>
#include <lib/stdio/cstr.h>
#include <misc/bitmap/bitmap.h>
#include <arch/arch.h>


memoryInfo_t memoryInfo;
Bitmap Pmm_PageBitmap;
uint64_t Pmm_Mutex;

bool Initialized = false;

void Pmm_Init(stivale2_struct_tag_memmap* Map){
    if (Initialized) return;

    Initialized = true;

    uintptr_t BitmapSegment = NULL;

    uint64_t memorySize = Pmm_GetMemorySize(Map);
    uint64_t PageCount = 0;
    uint64_t LastBase = 0;
    for (uint64_t i = 0; i < Map->entries; i++){
        if(Map->memmap[i].base > LastBase){
            LastBase = Map->memmap[i].base;
            PageCount = DivideRoundUp(Map->memmap[i].base + Map->memmap[i].length, PAGE_SIZE);
        }
    }

    uint64_t bitmapSize = DivideRoundUp(PageCount, 8);

    for (uint64_t i = 0; i < Map->entries; i++){
        if (Map->memmap[i].type == STIVALE2_MMAP_USABLE){
            uint64_t lenght = Map->memmap[i].length;
            uint64_t base = Map->memmap[i].base;
            if(Map->memmap[i].base <= (TRAMPOLINE_ADDRESS + TRAMPOLINE_SIZE)){
                lenght -= (TRAMPOLINE_ADDRESS + TRAMPOLINE_SIZE - Map->memmap[i].base);
                base = TRAMPOLINE_ADDRESS + TRAMPOLINE_SIZE;
            }

            if (lenght > bitmapSize){
                BitmapSegment = (uintptr_t)base;
                break;
            }
        }
    }

    memoryInfo.freePageMemory = PageCount;
    memoryInfo.usedPageMemory = 0x0;
    memoryInfo.totalPageMemory = PageCount;
    memoryInfo.reservedPageMemory = 0x0;
    memoryInfo.totalUsablePageMemory = Pmm_GetMemorySize(Map);
    Pmm_InitBitmap(bitmapSize, BitmapSegment);

    Pmm_ReservePages(0x0, PageCount + 1);


    for (uint64_t i = 0; i < Map->entries; i++){
        if (Map->memmap[i].type == STIVALE2_MMAP_USABLE){ 
            Pmm_UnreservePages((uintptr_t)Map->memmap[i].base, Map->memmap[i].length / PAGE_SIZE);
        }
    }

    /* Lock trampoline address */
    Pmm_LockPages((uintptr_t)TRAMPOLINE_ADDRESS, DivideRoundUp(TRAMPOLINE_SIZE, PAGE_SIZE));
    /* Lock bitmap address */
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

void Pmm_InitBitmap(size64_t bitmapSize, uintptr_t bufferAddress){
    Pmm_PageBitmap.Size = bitmapSize;
    Pmm_PageBitmap.Buffer = (uint8_t*)bufferAddress;
    memset(Pmm_PageBitmap.Buffer, 0x0, Pmm_PageBitmap.Size);
}

uint64_t Pmm_PageBitmapIndex = 0;
uintptr_t Pmm_RequestPage(){
    Atomic::atomicAcquire(&Pmm_Mutex, 0);
    for (uint64_t i = Pmm_PageBitmapIndex; i < memoryInfo.totalPageMemory; i++){
        if(!Pmm_PageBitmap.Get(i)){
            Pmm_PageBitmapIndex = i;
            Pmm_LockPage((uintptr_t)(i * PAGE_SIZE));
            Atomic::atomicUnlock(&Pmm_Mutex, 0);
            return (uintptr_t)(i * PAGE_SIZE);
        }
    }
    
    // Panic
    Atomic::atomicUnlock(&Pmm_Mutex, 0);
    KernelPanic("Not enought memory available");
    return NULL; 
}

uintptr_t Pmm_RequestPages(uint64_t pages){
    Atomic::atomicAcquire(&Pmm_Mutex, 0);
	while(Pmm_PageBitmapIndex < memoryInfo.totalPageMemory) {
		for(size64_t j = 0; j < pages; j++) {
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
    Atomic::atomicUnlock(&Pmm_Mutex, 0);
	return NULL;
}

void Pmm_FreePage(uintptr_t address){
    uint64_t index = (uint64_t)address / PAGE_SIZE;
    if (!Pmm_PageBitmap.Get(index)) return;
    if (Pmm_PageBitmap.Set(index, false)){
        memoryInfo.freePageMemory++;
        memoryInfo.usedPageMemory--;
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
        memoryInfo.freePageMemory--;
        memoryInfo.usedPageMemory++;
    }
}

void Pmm_LockPages(uintptr_t address, uint64_t pageCount){
    for (uint64_t t = 0; t < pageCount; t++){
        Pmm_LockPage((uintptr_t)((uint64_t)address + (t * PAGE_SIZE)));
    }
}

void Pmm_UnreservePage(uintptr_t address){
    memoryInfo.reservedPageMemory--;
    Pmm_FreePage(address);
}

void Pmm_UnreservePages(uintptr_t address, uint64_t pageCount){
    for (uint64_t t = 0; t < pageCount; t++){
        Pmm_UnreservePage((uintptr_t)((uint64_t)address + (t * PAGE_SIZE)));
    }
}

void Pmm_ReservePage(uintptr_t address){
    memoryInfo.reservedPageMemory++;
    Pmm_LockPage(address);
}

void Pmm_ReservePages(uintptr_t address, uint64_t pageCount){
    for (uint64_t t = 0; t < pageCount; t++){
        Pmm_ReservePage((uintptr_t)((uint64_t)address + (t * PAGE_SIZE)));
    }
}

uint64_t Pmm_GetTotalRAM(){
    return memoryInfo.totalPageMemory * PAGE_SIZE;
}

uint64_t Pmm_GetFreeRAM(){
    return memoryInfo.freePageMemory * PAGE_SIZE;
}
uint64_t Pmm_GetUsedRAM(){
    return memoryInfo.usedPageMemory * PAGE_SIZE;
}
uint64_t Pmm_GetReservedRAM(){
    return memoryInfo.reservedPageMemory * PAGE_SIZE;
}