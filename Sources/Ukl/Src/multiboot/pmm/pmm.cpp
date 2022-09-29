#include <multiboot/pmm/pmm.h>

Bitmap Pmm_PageBitmap;
uint64_t Pmm_FirstFreePageIndex = 0;
memoryInfo_t Pmm_MemoryInfo;

static inline uint64_t Pmm_ConvertAddressToIndex(uintptr_t address){
    return ((uint64_t)address) >> 12;
}

static inline uintptr_t Pmm_ConvertIndexToAddress(uint64_t index){
    return (uintptr_t)(index << 12);
}

void Pmm_Init(multiboot_tag_mmap* Map){
    uintptr_t BitmapSegment = NULL;

    uint64_t memorySize = Pmm_GetMemorySize(Map);
    uint64_t PageCountTotal = 0;
    uint64_t LastBase = 0;
    for (uint64_t i = 0; i < Map->entry_size; i++){
        if(Map->entries[i].addr > LastBase){
            LastBase = Map->entries[i].addr;
            PageCountTotal = DivideRoundUp(Map->entries[i].addr + Map->entries[i].len, PAGE_SIZE);
        }
    }

    uint64_t bitmapSize = DivideRoundUp(PageCountTotal, 8);

    for (uint64_t i = 0; i < Map->entry_size; i++){
        if (Map->entries[i].type == UKL_MMAP_USABLE){
            uint64_t lenght = Map->entries[i].len;
            uint64_t base = Map->entries[i].addr;

            /* Do not earase trampoline with bitmap */
            if(Map->entries[i].addr <= (PROTECT_ADDRESS + PROTECT_SIZE)){
                lenght -= (PROTECT_ADDRESS + PROTECT_SIZE - Map->entries[i].addr);
                base = PROTECT_ADDRESS + PROTECT_SIZE;
            }

            if (lenght > bitmapSize){
                BitmapSegment = (uintptr_t)base;
                break;
            }
        }
    }


    Pmm_MemoryInfo.freePageMemory = PageCountTotal;
    Pmm_MemoryInfo.usedPageMemory = 0x0;
    Pmm_MemoryInfo.totalPageMemory = PageCountTotal;
    Pmm_MemoryInfo.reservedPageMemory = 0x0;
    Pmm_MemoryInfo.totalUsablePageMemory = PageCountTotal;
    Pmm_InitBitmap(bitmapSize, BitmapSegment);

    Pmm_ReservePages(0x0, PageCountTotal + 1);

    /* Protect bitmap address */
    uint64_t ProtectedIndexStart = Pmm_ConvertAddressToIndex(BitmapSegment);
    uint64_t ProtectedIndexEnd = ProtectedIndexStart + DivideRoundUp(Pmm_PageBitmap.Size, PAGE_SIZE);
    for (uint64_t i = 0; i < Map->entry_size; i++){
        if (Map->entries[i].type == UKL_MMAP_USABLE){ 
            uint64_t indexstart = Pmm_ConvertAddressToIndex((uintptr_t)Map->entries[i].addr);
            uint64_t pageCount = Map->entries[i].len / PAGE_SIZE;
            if(indexstart > ProtectedIndexEnd){
                Pmm_UnreservePages_WI(indexstart, pageCount);
            }else if(indexstart + pageCount < ProtectedIndexStart){
                Pmm_UnreservePages_WI(indexstart, pageCount);
            }else{
                for(uint64_t t = 0; t < pageCount; t++){
                    uint64_t index = indexstart + t;
                    if(index < ProtectedIndexStart || ProtectedIndexEnd < index){
                        Pmm_UnreservePage_WI(index);
                    }
                }                
            }

        }
    }

    /* Lock trampoline address */
    Pmm_LockPages((uintptr_t)PROTECT_ADDRESS, DivideRoundUp(PROTECT_SIZE, PAGE_SIZE));
}

uint64_t Pmm_GetMemorySize(multiboot_tag_mmap* Map){
    static uint64_t memorySizeBytes = 0;
    if (memorySizeBytes > 0) return memorySizeBytes;

    for (uint64_t i = 0; i < Map->entry_size; i++){
        memorySizeBytes += Map->entries[i].len;
    }

    return memorySizeBytes;
}

void Pmm_InitBitmap(size64_t bitmapSize, uintptr_t bufferAddress){
    Pmm_PageBitmap.Size = bitmapSize;
    Pmm_PageBitmap.Buffer = (uint8_t*)bufferAddress;
    memset(Pmm_PageBitmap.Buffer, 0x0, Pmm_PageBitmap.Size);
}

uintptr_t Pmm_RequestPage(){
    for (uint64_t index = Pmm_FirstFreePageIndex; index < Pmm_MemoryInfo.totalPageMemory; index++){
        if(!Pmm_PageBitmap.GetAndSet(index, true)){
            Pmm_MemoryInfo.freePageMemory--;
            Pmm_MemoryInfo.usedPageMemory++;
            return (uintptr_t)(index * PAGE_SIZE);
        }
    }
    return NULL; 
}

void Pmm_FreePage_WI(uint64_t index){
    if(Pmm_PageBitmap.GetAndSet(index, false)){
        Pmm_MemoryInfo.freePageMemory++;
        Pmm_MemoryInfo.usedPageMemory--;
        if(Pmm_FirstFreePageIndex > index){
            Pmm_FirstFreePageIndex = index;
        }
    }
}

void Pmm_FreePages_WI(uint64_t index, uint64_t pageCount){
    uint64_t indexEnd = index + pageCount;
    for (uint64_t t = index; t < indexEnd; t++){
        if(Pmm_PageBitmap.GetAndSet(t, false)){
            Pmm_MemoryInfo.freePageMemory++;
            Pmm_MemoryInfo.usedPageMemory--;
            if(Pmm_FirstFreePageIndex > t){
                Pmm_FirstFreePageIndex = t;
            }
        }
    }
}

void Pmm_LockPage_WI(uint64_t index){
    if(!Pmm_PageBitmap.GetAndSet(index, true)){
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