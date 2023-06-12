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
    /* In this function we want to destroy everything but with some pity for the kernel so we will only destroy what we need to destroy */
    freelistinfo_t* FreeListInfo = (freelistinfo_t*)vmm_GetVirtualAddress(Pmm_ConvertIndexToAddress(index));
    if(FreeListInfo->PageSize == pageCount){
        /* So here our caller are asking us to destroy the all segments */
        /* So we just need to relink to create a world where this segment have never existed... */
        if(FreeListInfo->Last != NULL){
            FreeListInfo->Last->Next = FreeListInfo->Next;
        }
        if(FreeListInfo->Next != NULL){
            FreeListInfo->Next->Last = FreeListInfo->Last;
        }

        /* Check if it's not the entry */
        if(FreeListInfo == Pmm_LastFreeListInfo){
            /* So if it's last of the list we just need to update the entry to the last field because it's the only field of the free list info that's abble to be not NULL */
            Pmm_LastFreeListInfo = FreeListInfo->Last;
        }
    }else{
        /* We have to move forward the header */
        freelistinfo_t* NewFreeListInfo = (freelistinfo_t*)vmm_GetVirtualAddress(Pmm_ConvertIndexToAddress(index + pageCount));
        
        /* Steal the link like good stealer */
        NewFreeListInfo->Next = FreeListInfo->Next;
        NewFreeListInfo->Last = FreeListInfo->Last;

        /* Check if the segment that we're stealing have still link in his pocket */
        if(FreeListInfo == Pmm_LastFreeListInfo){
            Pmm_LastFreeListInfo = NewFreeListInfo;
        }

        /* Update some trivial numbers */
        /* Remove some size */
        NewFreeListInfo->PageSize =  FreeListInfo->PageSize - pageCount;
        NewFreeListInfo->IndexStart = FreeListInfo->IndexStart + pageCount;

        /* We keep the position of the end segment */
        NewFreeListInfo->IndexEnd = FreeListInfo->IndexEnd;

        /* Save the end position in the new struct */
        NewFreeListInfo->Header.End = FreeListInfo->Header.End;

        /* Update the end */
        NewFreeListInfo->Header.End->Start = NewFreeListInfo;
    }
}

void Pmm_AddPageToFreeList(uint64_t index, uint64_t pageCount){
    /* Merge with free list */
    bool IsNextFree = !Pmm_Bitmap.Get(index + pageCount);
    bool IsLastFree = !Pmm_Bitmap.Get(index - 1);
    if(IsNextFree){
        if(IsLastFree){
            /* So we have next and last, and this we need to merge them */
            /* Get next structure start which we can locate at the end of this (index + pageCount)*/
            freelistinfo_t* FreeListInfoNext = (freelistinfo_t*)vmm_GetVirtualAddress(Pmm_ConvertIndexToAddress(index + pageCount)); /* We can Get start because we are at the start of the freelist segment */
            
            /* Locate end of the last to find the start header to have necessary informations */
            freelistinfoend_t* FreeListInfoEnd = (freelistinfoend_t*)vmm_GetVirtualAddress(Pmm_ConvertIndexToAddress(index - 1));
            
            /* As I said before we want to locate the last headers which will be the main header of the new segment*/
            freelistinfo_t* FreeListInfoLast = FreeListInfoEnd->Start; 

            /* Here we will destory last end and next start structure because we don't need them anymore */

            /* Update the main structures */
            /* As I said before we only keep the main structure and the end structure so link them together */
            FreeListInfoLast->Header.End = FreeListInfoNext->Header.End;
            FreeListInfoLast->Header.End->Start = FreeListInfoLast;
            FreeListInfoLast->IndexEnd = FreeListInfoNext->IndexEnd;

            /* Update size */
            FreeListInfoLast->PageSize += pageCount + FreeListInfoNext->PageSize;

            /* Relink, I mean here we destroy segment so we have to be CAREFUL to not destroy the main list */
            /* So as we destroy the next start structure let's remove it from the list*/
            if(FreeListInfoNext->Last){
                FreeListInfoNext->Last->Next = FreeListInfoNext->Next;
            }
            if(FreeListInfoNext->Next){
                FreeListInfoNext->Next->Last = FreeListInfoNext->Last;
            }

            /* Don't be happy now we have still work to do, if the entry Pmm_LastFreeListInfo is the segment to destroy */
            if(FreeListInfoNext == Pmm_LastFreeListInfo){
                /* If it's the case the next field will be null so we can only use the last field of the struct */
                Pmm_LastFreeListInfo = FreeListInfoNext->Last;
            }
        }else{
            /* So here only next is free let's destroy the next start structure */
            /* Locate useful structures */
            freelistinfo_t* FreeListInfoNext = (freelistinfo_t*)vmm_GetVirtualAddress(Pmm_ConvertIndexToAddress(index + pageCount));
            freelistinfo_t* NewFreeListInfo = (freelistinfo_t*)vmm_GetVirtualAddress(Pmm_ConvertIndexToAddress(index));

            /* And link the start struct with the end*/
            NewFreeListInfo->Header.End = FreeListInfoNext->Header.End;
            
            /* Update end structures and link it to the new structure */
            NewFreeListInfo->Header.End->Start = NewFreeListInfo;

            /* The new structures is the start */
            NewFreeListInfo->IndexStart = index;

            /* Set size */
            NewFreeListInfo->PageSize = FreeListInfoNext->PageSize + pageCount;

            /* Keep index end we don't change anything here */
            NewFreeListInfo->IndexEnd = FreeListInfoNext->IndexEnd;

            /* Relink, here we will steal the list from the next field because we want to destroy it definitively */
            NewFreeListInfo->Next = FreeListInfoNext->Next;
            NewFreeListInfo->Last = FreeListInfoNext->Last;

            /* Check if the segment that we're stealing have still link in his pocket */
            if(FreeListInfoNext == Pmm_LastFreeListInfo){
                Pmm_LastFreeListInfo = NewFreeListInfo;
            }
        }
    }else if(IsLastFree){
        /* Get strucutres */
        freelistinfoend_t* FreeListInfoEnd = (freelistinfoend_t*)vmm_GetVirtualAddress(Pmm_ConvertIndexToAddress(index - 1));
        
        /* We get the end segment so with the header locate the start segment */
        freelistinfo_t* FreeListInfoLast = FreeListInfoEnd->Start;
        
        /* Locate the new end structures */
        FreeListInfoEnd = (freelistinfoend_t*)vmm_GetVirtualAddress(Pmm_ConvertIndexToAddress(index + pageCount - 1));
        
        /* Add new pages we found */
        FreeListInfoLast->PageSize += pageCount;
        FreeListInfoLast->IndexEnd = index + pageCount - 1;

        /* Define end and start */
        FreeListInfoLast->Header.End = FreeListInfoEnd;
        FreeListInfoEnd->Start = FreeListInfoLast;

        /* We do NOT link to the list here because we already done that when we create the segment */
    }else{
        /* Get strucutres */
        freelistinfo_t* FreeListInfo = (freelistinfo_t*)vmm_GetVirtualAddress(Pmm_ConvertIndexToAddress(index));
        freelistinfoend_t* FreeListInfoEnd = (freelistinfoend_t*)vmm_GetVirtualAddress(Pmm_ConvertIndexToAddress(index + pageCount - 1));
        
        /* Some numbers to characterize the segment */
        FreeListInfo->IndexStart = index;
        FreeListInfo->IndexEnd = index + pageCount - 1;
        FreeListInfo->PageSize = pageCount;

        /* Define end and start */
        FreeListInfo->Header.End = FreeListInfoEnd;
        FreeListInfoEnd->Start = FreeListInfo;

        /* Link to list */
        FreeListInfo->Last = Pmm_LastFreeListInfo;
        FreeListInfo->Next = NULL;
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
    for(uint64_t index = Pmm_FirstFreePageIndex; index < Pmm_MemoryInfo.totalPageMemory; index++){
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
    Pmm_LockPages_WI(index, pageCount);
    Pmm_RemovePagesToFreeList(index, pageCount);
    AtomicRelease(&Pmm_Mutex);
	return (void*)(index * PAGE_SIZE);
}

void Pmm_FreePage_WI(uint64_t index){
    AtomicAcquire(&Pmm_Mutex);
    if(Pmm_Bitmap.GetAndSet(index, false)){
        Pmm_AddPageToFreeList(index, 1);
        Pmm_MemoryInfo.freePageMemory++;
        Pmm_MemoryInfo.usedPageMemory--;
        if(Pmm_FirstFreePageIndex > index){
            Pmm_FirstFreePageIndex = index;
        }
    }
    AtomicRelease(&Pmm_Mutex);
}

void Pmm_FreePages_WI(uint64_t index, uint64_t pageCount){
    AtomicAcquire(&Pmm_Mutex);
    Pmm_AddPageToFreeList(index, pageCount);
    uint64_t indexEnd = index + pageCount;
    for(uint64_t t = index; t < indexEnd; t++){
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