#include <memory/memory.h>
#include <lib/stdio/cstr.h>
#include <misc/bitmap/bitmap.h>
#include <arch/arch.h>


memoryInfo_t Pmm_MemoryInfo;
Bitmap Pmm_PageBitmap;
uint64_t Pmm_Mutex;
uint64_t Pmm_FirstFreePageIndex = 0;
freelistinfo_t* Pmm_LastFreeListInfo = NULL;

bool Initialized = false;

static inline uint64_t Pmm_ConvertAddressToIndex(uintptr_t address){
    return ((uint64_t)address) >> 12;
}

static inline uintptr_t Pmm_ConvertIndexToAddress(uint64_t index){
    return (uintptr_t)(index << 12);
}

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

            /* Do not earase trampoline with bitmap */
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

    Pmm_MemoryInfo.freePageMemory = PageCount;
    Pmm_MemoryInfo.usedPageMemory = 0x0;
    Pmm_MemoryInfo.totalPageMemory = PageCount;
    Pmm_MemoryInfo.reservedPageMemory = 0x0;
    Pmm_MemoryInfo.totalUsablePageMemory = Pmm_GetMemorySize(Map);
    Pmm_InitBitmap(bitmapSize, BitmapSegment);

    Pmm_ReservePages(0x0, PageCount + 1);

    /* Protect bitmap address */
    uint64_t ProtectedIndexStart = Pmm_ConvertAddressToIndex(BitmapSegment);
    uint64_t ProtectedIndexEnd = ProtectedIndexStart + DivideRoundUp(Pmm_PageBitmap.Size, PAGE_SIZE);
    for (uint64_t i = 0; i < Map->entries; i++){
        if (Map->memmap[i].type == STIVALE2_MMAP_USABLE){ 
            uint64_t indexstart = Pmm_ConvertAddressToIndex((uintptr_t)Map->memmap[i].base);
            uint64_t pageCount = Map->memmap[i].length / PAGE_SIZE;
            for (uint64_t t = 0; t < pageCount; t++){
                uint64_t index = indexstart + t;
                if(index < ProtectedIndexStart || ProtectedIndexEnd < index){
                    Pmm_UnreservePage_WI(index);
                }
            }
        }
    }

    /* Lock trampoline address */
    Pmm_LockPages((uintptr_t)TRAMPOLINE_ADDRESS, DivideRoundUp(TRAMPOLINE_SIZE, PAGE_SIZE));
}

void Pmm_RemovePagesToFreeList(uint64_t index, uint64_t pageCount){
    freelistinfo_t* FreeListInfo = (freelistinfo_t*)vmm_GetVirtualAddress(Pmm_ConvertIndexToAddress(index));
    if(FreeListInfo->IndexEnd == index){
        if(FreeListInfo->Last != NULL){
            FreeListInfo->Last->Next = FreeListInfo->Next;
        }
        if(FreeListInfo->Next != NULL){
            FreeListInfo->Next->Last = FreeListInfo->Last;
        }
    }else{
        freelistinfo_t* NewFreeListInfo = (freelistinfo_t*)vmm_GetVirtualAddress(Pmm_ConvertIndexToAddress(index + 1));
        if(FreeListInfo->Last != NULL){
            FreeListInfo->Last->Next = NewFreeListInfo;
        }
        if(FreeListInfo->Next != NULL){
            FreeListInfo->Next->Last = NewFreeListInfo;
        }
        memcpy(NewFreeListInfo, FreeListInfo, sizeof(freelistinfo_t));
        NewFreeListInfo->PageSize -= pageCount;
        NewFreeListInfo->Header.End->Start = NewFreeListInfo;
    }
}

void Pmm_AddPageToFreeList(uint64_t index, uint64_t pageCount){
    /* Merge with free list */
    bool IsNextFree = !Pmm_PageBitmap.Get(index + pageCount);
    bool IsLastFree = !Pmm_PageBitmap.Get(index - 1);

    freelistinfomiddle_t* FreeListInfoEnd = NULL;

    if(IsNextFree){
        if(IsLastFree){
            freelistinfo_t* FreeListInfoNext = (freelistinfo_t*)vmm_GetVirtualAddress(Pmm_ConvertIndexToAddress(index + pageCount)); /* We can get start because we are at the start of the freelist segment */
            FreeListInfoEnd = (freelistinfomiddle_t*)vmm_GetVirtualAddress(Pmm_ConvertIndexToAddress(index - 1));
            freelistinfo_t* FreeListInfoLast = FreeListInfoEnd->Start; /* We can get start because we are at the end of the freelist segment */
            
            FreeListInfoLast->Next = FreeListInfoNext->Next;
            FreeListInfoNext->PageSize += pageCount + FreeListInfoNext->PageSize;

            /* Setup middle end as middle segment */
            FreeListInfoLast->Header.End->End = FreeListInfoNext->Header.End;

            FreeListInfoLast->Header.End = FreeListInfoNext->Header.End;
            FreeListInfoLast->Header.End->Start = FreeListInfoLast;
        }else{
            freelistinfo_t* FreeListInfoNext = (freelistinfo_t*)vmm_GetVirtualAddress(Pmm_ConvertIndexToAddress(index + pageCount));
            freelistinfo_t* NewFreeListInfo = (freelistinfo_t*)vmm_GetVirtualAddress(Pmm_ConvertIndexToAddress(index));
            
            /* Relink */
            if(FreeListInfoNext->Last != NULL){
                FreeListInfoNext->Last->Next = NewFreeListInfo;
            }
            if(FreeListInfoNext->Next != NULL){
                FreeListInfoNext->Next->Last = NewFreeListInfo;
            }

            memcpy(NewFreeListInfo, FreeListInfoNext, sizeof(freelistinfo_t));
            NewFreeListInfo->PageSize += pageCount;
            NewFreeListInfo->Header.End->Start = NewFreeListInfo;
        }
    }else if(IsLastFree){
        FreeListInfoEnd = (freelistinfomiddle_t*)vmm_GetVirtualAddress(Pmm_ConvertIndexToAddress(index - 1));
        freelistinfo_t* FreeListInfoLast = FreeListInfoEnd->Start; /* We can get start because we are at the end of the freelist segment */
        FreeListInfoLast->PageSize += pageCount;
        FreeListInfoEnd = (freelistinfomiddle_t*)vmm_GetVirtualAddress(Pmm_ConvertIndexToAddress(index + pageCount - 1));
        FreeListInfoLast->Header.End->End = FreeListInfoEnd;
        FreeListInfoLast->Header.End = FreeListInfoEnd;
        FreeListInfoLast->IndexEnd = index + pageCount - 1;
        FreeListInfoLast->Header.End->Start = FreeListInfoLast;
    }else{
        /* Create free list */
        freelistinfo_t* FreeListInfo = (freelistinfo_t*)vmm_GetVirtualAddress(Pmm_ConvertIndexToAddress(index));
        FreeListInfoEnd = (freelistinfomiddle_t*)vmm_GetVirtualAddress(Pmm_ConvertIndexToAddress(index + pageCount - 1));
        FreeListInfo->Header.End = FreeListInfoEnd;
        FreeListInfo->IndexEnd = index + pageCount - 1;
        FreeListInfo->Last = Pmm_LastFreeListInfo;
        FreeListInfo->Next = NULL;
        FreeListInfo->PageSize = pageCount;
        FreeListInfoEnd->Start = FreeListInfo;
        Pmm_LastFreeListInfo = FreeListInfo;
    }

    for(uint64_t i = 1; i < pageCount - 1; i++){
        freelistinfomiddle_t* FreeListInfoMiddle = (freelistinfomiddle_t*)vmm_GetVirtualAddress(Pmm_ConvertIndexToAddress(index + i));
        FreeListInfoMiddle->End = FreeListInfoEnd;
    }
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

uintptr_t Pmm_RequestPage(){
    Atomic::atomicAcquire(&Pmm_Mutex, 0);
    for (uint64_t index = Pmm_FirstFreePageIndex; index < Pmm_MemoryInfo.totalPageMemory; index++){
        if(!Pmm_PageBitmap.Get(index)){
            Pmm_FirstFreePageIndex = index;
            Pmm_RemovePagesToFreeList(index, 1);
            Pmm_LockPage_WI(index);
            Atomic::atomicUnlock(&Pmm_Mutex, 0);
            return (uintptr_t)(index * PAGE_SIZE);
        }
    }
    
    // Panic
    Atomic::atomicUnlock(&Pmm_Mutex, 0);
    KernelPanic("Not enought memory available");
    return NULL; 
}

uintptr_t Pmm_RequestPages(uint64_t pages){
    Atomic::atomicAcquire(&Pmm_Mutex, 0);
	/* free list */
    Atomic::atomicUnlock(&Pmm_Mutex, 0);
	return NULL;
}

void Pmm_FreePage_WI(uint64_t index){
    if(!Pmm_PageBitmap.Get(index)) return;
    if(Pmm_PageBitmap.Set(index, false)){
        Pmm_AddPageToFreeList(index, 1);
        Pmm_MemoryInfo.freePageMemory++;
        Pmm_MemoryInfo.usedPageMemory--;
        if(Pmm_FirstFreePageIndex > index){
            Pmm_FirstFreePageIndex = index;
        }
    }
}

void Pmm_FreePages_WI(uint64_t index, uint64_t pageCount){
    Pmm_AddPageToFreeList(index, pageCount);
    for (int t = 0; t < pageCount; t++){
        if(!Pmm_PageBitmap.Get(index)) return;
        if(Pmm_PageBitmap.Set(index, false)){
            Pmm_MemoryInfo.freePageMemory++;
            Pmm_MemoryInfo.usedPageMemory--;
            if(Pmm_FirstFreePageIndex > index){
                Pmm_FirstFreePageIndex = index;
            }
        }
    }
}

void Pmm_LockPage_WI(uint64_t index){
    if (Pmm_PageBitmap.Get(index)) return;
    if (Pmm_PageBitmap.Set(index, true)){
        Pmm_MemoryInfo.freePageMemory--;
        Pmm_MemoryInfo.usedPageMemory++;
    }
}

void Pmm_LockPages_WI(uint64_t index, uint64_t pageCount){
    for (uint64_t t = 0; t < pageCount; t++){
        Pmm_LockPage_WI(index + t);
    }
}

void Pmm_UnreservePage_WI(uint64_t index){
    Pmm_MemoryInfo.reservedPageMemory--;
    Pmm_FreePage_WI(index);
}

void Pmm_UnreservePages_WI(uint64_t index, uint64_t pageCount){
    Pmm_MemoryInfo.reservedPageMemory--;
    Pmm_FreePages_WI(index, pageCount);
}

void Pmm_ReservePage_WI(uint64_t index){
    Pmm_MemoryInfo.reservedPageMemory++;
    Pmm_LockPage_WI(index);
}

void Pmm_ReservePages_WI(uint64_t index, uint64_t pageCount){
    Pmm_MemoryInfo.reservedPageMemory++;
    Pmm_LockPages_WI(index, pageCount);
}

void Pmm_FreePage(uintptr_t address){
    Pmm_FreePage_WI(Pmm_ConvertAddressToIndex(address));
}

void Pmm_FreePages(uintptr_t address, uint64_t pageCount){
    Pmm_FreePages_WI(Pmm_ConvertAddressToIndex(address), pageCount);
}

void Pmm_LockPage(uintptr_t address){
    Pmm_LockPage_WI(Pmm_ConvertAddressToIndex(address));
}

void Pmm_LockPages(uintptr_t address, uint64_t pageCount){
    Pmm_LockPages_WI(Pmm_ConvertAddressToIndex(address), pageCount);
}

void Pmm_UnreservePage(uintptr_t address){
    Pmm_UnreservePage_WI(Pmm_ConvertAddressToIndex(address));
}

void Pmm_UnreservePages(uintptr_t address, uint64_t pageCount){
    Pmm_UnreservePages_WI(Pmm_ConvertAddressToIndex(address), pageCount);
}

void Pmm_ReservePage(uintptr_t address){
    Pmm_ReservePage_WI(Pmm_ConvertAddressToIndex(address));
}

void Pmm_ReservePages(uintptr_t address, uint64_t pageCount){
    Pmm_ReservePages_WI(Pmm_ConvertAddressToIndex(address), pageCount);
}

uint64_t Pmm_GetTotalRAM(){
    return Pmm_MemoryInfo.totalPageMemory * PAGE_SIZE;
}

uint64_t Pmm_GetFreeRAM(){
    return Pmm_MemoryInfo.freePageMemory * PAGE_SIZE;
}
uint64_t Pmm_GetUsedRAM(){
    return Pmm_MemoryInfo.usedPageMemory * PAGE_SIZE;
}
uint64_t Pmm_GetReservedRAM(){
    return Pmm_MemoryInfo.reservedPageMemory * PAGE_SIZE;
}