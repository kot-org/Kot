#include <memory/memory.h>
#include <lib/stdio/cstr.h>
#include <misc/bitmap/bitmap.h>
#include <arch/arch.h>


memoryInfo_t Pmm_MemoryInfo;
Bitmap Pmm_Bitmap;
locker_t Pmm_Mutex;
uint64_t Pmm_FirstFreePageIndex = 0;
freelistinfo_t* Pmm_LastFreeListInfo = NULL;

static inline uint64_t Pmm_ConvertAddressToIndex(void* address){
    return ((uint64_t)address) >> 12;
}

static inline void* Pmm_ConvertIndexToAddress(uint64_t index){
    return (void*)(index << 12);
}

void Pmm_Init(ukl_memmory_info_t* MemoryInfo){
    uint64_t memorySize = Pmm_GetMemorySize(MemoryInfo);
    uint64_t LastBase = 0;

    Pmm_MemoryInfo.freePageMemory = MemoryInfo->page_count_total;
    Pmm_MemoryInfo.usedPageMemory = 0x0;
    Pmm_MemoryInfo.totalPageMemory = MemoryInfo->page_count_total;
    Pmm_MemoryInfo.reservedPageMemory = 0x0;
    Pmm_MemoryInfo.totalUsablePageMemory = MemoryInfo->page_count_total;
    Pmm_InitBitmap((void*)MemoryInfo->bitmap_address, MemoryInfo->bitmap_size);

    Pmm_ReservePages(0x0, MemoryInfo->page_count_total);

    ukl_mmap_info_t* MapEntry = (ukl_mmap_info_t*)vmm_GetVirtualAddress(MemoryInfo->map_main_entry);
    for (uint64_t i = 0; i < MemoryInfo->map_entries_count; i++){
        if (MapEntry->type == UKL_MMAP_AVAILABLE){ 
            uint64_t indexstart = Pmm_ConvertAddressToIndex((void*)MapEntry->base);
            uint64_t pageCount = MapEntry->length / PAGE_SIZE;
            Pmm_UnreservePages_WI(indexstart, pageCount);
        }
        MapEntry = (ukl_mmap_info_t*)vmm_GetVirtualAddress(MapEntry->map_next_entry);
    }

    /* Lock trampoline address */
    Pmm_LockPages((void*)TRAMPOLINE_ADDRESS, DivideRoundUp(TRAMPOLINE_SIZE, PAGE_SIZE));
}

void Pmm_RemovePagesToFreeList(uint64_t index, uint64_t pageCount){
    freelistinfo_t* FreeListInfo = (freelistinfo_t*)vmm_GetVirtualAddress(Pmm_ConvertIndexToAddress(index));
    if(FreeListInfo->PageSize == pageCount){
        if(FreeListInfo == Pmm_LastFreeListInfo){
            if(FreeListInfo->Next != NULL){
                Pmm_LastFreeListInfo = FreeListInfo->Next;
            }else if(FreeListInfo->Last != NULL){
                Pmm_LastFreeListInfo = FreeListInfo->Last;
            }else{
                Pmm_LastFreeListInfo = NULL;
            }
        }
        if(FreeListInfo->Last != NULL){
            FreeListInfo->Last->Next = FreeListInfo->Next;
        }
        if(FreeListInfo->Next != NULL){
            FreeListInfo->Next->Last = FreeListInfo->Last;
        }
    }else{
        freelistinfo_t* NewFreeListInfo = (freelistinfo_t*)vmm_GetVirtualAddress(Pmm_ConvertIndexToAddress(index + pageCount));
        if(FreeListInfo->Last != NULL){
            FreeListInfo->Last->Next = NewFreeListInfo;
        }
        if(FreeListInfo->Next != NULL){
            FreeListInfo->Next->Last = NewFreeListInfo;
        }
        if(FreeListInfo == Pmm_LastFreeListInfo){
            Pmm_LastFreeListInfo = NewFreeListInfo;
        }
        memcpy(NewFreeListInfo, FreeListInfo, sizeof(freelistinfo_t));
        NewFreeListInfo->PageSize -= pageCount;
        NewFreeListInfo->IndexStart += pageCount;
        NewFreeListInfo->Header.End->Start = NewFreeListInfo;
    }
}

void Pmm_AddPageToFreeList(uint64_t index, uint64_t pageCount){
    /* Merge with free list */
    bool IsNextFree = !Pmm_Bitmap.Get(index + pageCount);
    bool IsLastFree = !Pmm_Bitmap.Get(index - 1);
    if(IsNextFree){
        if(IsLastFree){
            freelistinfo_t* FreeListInfoNext = (freelistinfo_t*)vmm_GetVirtualAddress(Pmm_ConvertIndexToAddress(index + pageCount)); /* We can Get start because we are at the start of the freelist segment */
            freelistinfoend_t* FreeListInfoEnd = (freelistinfoend_t*)vmm_GetVirtualAddress(Pmm_ConvertIndexToAddress(index - 1));
            freelistinfo_t* FreeListInfoLast = FreeListInfoEnd->Start; /* We can Get start because we are at the end of the freelist segment */
            
            FreeListInfoLast->Next = FreeListInfoNext->Next;
            FreeListInfoNext->PageSize += pageCount + FreeListInfoLast->PageSize;
            FreeListInfoLast->PageSize += pageCount + FreeListInfoNext->PageSize;

            FreeListInfoLast->IndexEnd = FreeListInfoNext->IndexEnd;
            FreeListInfoLast->Header.End = FreeListInfoNext->Header.End;
            FreeListInfoLast->Header.End->Start = FreeListInfoLast;

            /* Relink */
            if(FreeListInfoNext == Pmm_LastFreeListInfo){
                Pmm_LastFreeListInfo = FreeListInfoLast;
            }
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
            if(FreeListInfoNext == Pmm_LastFreeListInfo){
                Pmm_LastFreeListInfo = NewFreeListInfo;
            }

            memcpy(NewFreeListInfo, FreeListInfoNext, sizeof(freelistinfo_t));
            NewFreeListInfo->PageSize += pageCount;
            NewFreeListInfo->IndexStart = index;
            NewFreeListInfo->Header.End->Start = NewFreeListInfo;
        }
    }else if(IsLastFree){
        freelistinfoend_t* FreeListInfoEnd = (freelistinfoend_t*)vmm_GetVirtualAddress(Pmm_ConvertIndexToAddress(index - 1));
        freelistinfo_t* FreeListInfoLast = FreeListInfoEnd->Start; /* We can Get start because we are at the end of the freelist segment */
        FreeListInfoEnd = (freelistinfoend_t*)vmm_GetVirtualAddress(Pmm_ConvertIndexToAddress(index + pageCount - 1));
        FreeListInfoLast->PageSize += pageCount;
        FreeListInfoLast->IndexEnd = index + pageCount - 1;
        FreeListInfoLast->Header.End = FreeListInfoEnd;
        FreeListInfoEnd->Start = FreeListInfoLast;
    }else{
        /* Create free list */
        freelistinfo_t* FreeListInfo = (freelistinfo_t*)vmm_GetVirtualAddress(Pmm_ConvertIndexToAddress(index));
        freelistinfoend_t* FreeListInfoEnd = (freelistinfoend_t*)vmm_GetVirtualAddress(Pmm_ConvertIndexToAddress(index + pageCount - 1));
        FreeListInfo->Header.End = FreeListInfoEnd;
        FreeListInfo->IndexStart = index;
        FreeListInfo->IndexEnd = index + pageCount - 1;
        FreeListInfo->Last = Pmm_LastFreeListInfo;
        FreeListInfo->Next = NULL;
        FreeListInfo->PageSize = pageCount;
        FreeListInfoEnd->Start = FreeListInfo;

        if(Pmm_LastFreeListInfo != NULL){
            Pmm_LastFreeListInfo->Next = FreeListInfo;
        }

        /* Update last link */
        Pmm_LastFreeListInfo = FreeListInfo;
    }
}

uint64_t Pmm_GetMemorySize(ukl_memmory_info_t* MemInfo){
    return MemInfo->page_count_total * PAGE_SIZE;
}

void Pmm_InitBitmap(void* bufferAddress, size64_t bitmapSize){
    Pmm_Bitmap.Size = bitmapSize;
    Pmm_Bitmap.Buffer = (uint8_t*)bufferAddress;
    memset(Pmm_Bitmap.Buffer, 0x0, Pmm_Bitmap.Size);
}

void* Pmm_RequestPage(){
    AtomicAcquire(&Pmm_Mutex);
    for (uint64_t index = Pmm_FirstFreePageIndex; index < Pmm_MemoryInfo.totalPageMemory; index++){
        if(!Pmm_Bitmap.GetAndSet(index, true)){
            Pmm_FirstFreePageIndex = index;
            Pmm_MemoryInfo.freePageMemory--;
            Pmm_MemoryInfo.usedPageMemory++;
            Pmm_RemovePagesToFreeList(index, 1);
            AtomicRelease(&Pmm_Mutex);
            return (void*)(index * PAGE_SIZE);
        }
    }
    
    // Panic
    KernelPanic("Not enough memory available");
    AtomicRelease(&Pmm_Mutex);
    return NULL; 
}

void* Pmm_RequestPages(uint64_t pageCount){
    AtomicAcquire(&Pmm_Mutex);
    freelistinfo_t* Pmm_FreeList = Pmm_LastFreeListInfo;
	while(Pmm_FreeList->PageSize < pageCount){
        if(Pmm_FreeList->Last != NULL){
            Pmm_FreeList = Pmm_FreeList->Last;
        }else{
            KernelPanic("Not enough memory available");
            AtomicRelease(&Pmm_Mutex);
	        return NULL;
        }
    }

    uint64_t index = Pmm_FreeList->IndexStart;
    Pmm_RemovePagesToFreeList(index, pageCount);
    Pmm_LockPages_WI(index, pageCount);
    AtomicRelease(&Pmm_Mutex);
	return (void*)(index * PAGE_SIZE);
}

void Pmm_FreePage_WI(uint64_t index){
    AtomicAcquire(&Pmm_Mutex);
    // if(Pmm_Bitmap.GetAndSet(index, false)){
    //     Pmm_AddPageToFreeList(index, 1);
    //     Pmm_MemoryInfo.freePageMemory++;
    //     Pmm_MemoryInfo.usedPageMemory--;
    //     if(Pmm_FirstFreePageIndex > index){
    //         Pmm_FirstFreePageIndex = index;
    //     }
    // }
    AtomicRelease(&Pmm_Mutex);
}

void Pmm_FreePages_WI(uint64_t index, uint64_t pageCount){
    AtomicAcquire(&Pmm_Mutex);
    Pmm_AddPageToFreeList(index, pageCount);
    uint64_t indexEnd = index + pageCount;
    for (uint64_t t = index; t < indexEnd; t++){
        if(Pmm_Bitmap.GetAndSet(t, false)){
            Pmm_MemoryInfo.freePageMemory++;
            Pmm_MemoryInfo.usedPageMemory--;
            if(Pmm_FirstFreePageIndex > t){
                Pmm_FirstFreePageIndex = t;
            }
        }
    }
    AtomicRelease(&Pmm_Mutex);
}

void Pmm_LockPage_WI(uint64_t index){
    if(!Pmm_Bitmap.GetAndSet(index, true)){
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

void Pmm_FreePage(void* address){
    Pmm_FreePage_WI(Pmm_ConvertAddressToIndex(address));
}

void Pmm_FreePages(void* address, uint64_t pageCount){
    Pmm_FreePages_WI(Pmm_ConvertAddressToIndex(address), pageCount);
}

void Pmm_LockPage(void* address){
    Pmm_LockPage_WI(Pmm_ConvertAddressToIndex(address));
}

void Pmm_LockPages(void* address, uint64_t pageCount){
    Pmm_LockPages_WI(Pmm_ConvertAddressToIndex(address), pageCount);
}

void Pmm_UnreservePage(void* address){
    Pmm_UnreservePage_WI(Pmm_ConvertAddressToIndex(address));
}

void Pmm_UnreservePages(void* address, uint64_t pageCount){
    Pmm_UnreservePages_WI(Pmm_ConvertAddressToIndex(address), pageCount);
}

void Pmm_ReservePage(void* address){
    Pmm_ReservePage_WI(Pmm_ConvertAddressToIndex(address));
}

void Pmm_ReservePages(void* address, uint64_t pageCount){
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