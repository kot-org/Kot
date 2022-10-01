#include <multiboot/pmm/pmm.h>

multiboot_tag_mmap* Pmm_Map;
uint64_t Pmm_LastIndexAllocated;
uint64_t Pmm_LastAddressAllocated;

static inline uint64_t Pmm_ConvertAddressToIndex(uint64_t address){
    return ((uint64_t)address) >> 12;
}

static inline uint64_t Pmm_ConvertIndexToAddress(uint64_t index){
    return (uint64_t)(index << 12);
}

void Pmm_Init(multiboot_tag_mmap* Map){
    Pmm_Map = Map;

    for(uint64_t i = 0; i < Pmm_Map->entry_size; i++){
        multiboot_mmap_entry* entry = &Pmm_Map->entries[i];
        if(entry->type == MULTIBOOT_MEMORY_AVAILABLE){
            Pmm_LastIndexAllocated = i;
            Pmm_LastAddressAllocated = entry->addr;
            if(Pmm_LastAddressAllocated < PROTECT_ADDRESS + PROTECT_SIZE){
                Pmm_LastAddressAllocated = PROTECT_ADDRESS + PROTECT_SIZE;
            }
            break;
        }
    }
}

uint64_t Pmm_GetMemorySize(multiboot_tag_mmap* Map){
    static uint64_t memorySizeBytes = 0;
    if (memorySizeBytes > 0) return memorySizeBytes;

    for (uint64_t i = 0; i < Map->entry_size; i++){
        memorySizeBytes += Map->entries[i].len;
    }

    return memorySizeBytes;
}

uint64_t Pmm_RequestPage(){
    for(uint64_t i = Pmm_LastIndexAllocated; i < Pmm_Map->entry_size; i++){
        multiboot_mmap_entry* entry = &Pmm_Map->entries[i];
        if(entry->type == MULTIBOOT_MEMORY_AVAILABLE){
            size64_t SizeAvailable = (entry->addr + entry->len) - Pmm_LastAddressAllocated;
            if(SizeAvailable >= PAGE_SIZE){
                uint64_t AddressAllocated = Pmm_LastAddressAllocated;
                Pmm_LastAddressAllocated += PAGE_SIZE;
                size64_t SizeAvailableAfterAllocation = (entry->addr + entry->len) - Pmm_LastAddressAllocated;
                if(SizeAvailable < PAGE_SIZE){
                    Pmm_LastIndexAllocated++; // go to the next segment
                }
                char buf[33];
                itoa(AddressAllocated, (char*)&buf, 0x10);
                //Print(buf);
                //Print("\n");
                return (uint64_t)AddressAllocated;
            }
        }
    }
    return NULL; 
}

void Pmm_ExtractsInfo(){
    // TODO
}