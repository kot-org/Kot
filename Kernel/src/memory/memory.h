#pragma once

#include "efiMemory.h"
#include "../memory/paging/PageTableManager.h"

uint64_t GetMemorySize(EFI_MEMORY_DESCRIPTOR* mMap, uint64_t nMApEntries, uint64_t mMapDescSize);
void memset(void* start, uint8_t value, uint64_t num);
int memcmp(const void *aptr, const void *bptr, size_t n);

void memcpy(void* destination, void* source, uint64_t num);

namespace Memory{
    struct MemoryShareInfo{
        bool Lock;
        size_t Size;
        uint64_t PageNumber;
        //Parent
        struct PageTableManager* PageTableParent;
        void* VirtualAddressParent;
    }__attribute__((packed));

    void* CreatSharing(struct PageTableManager* pageTable, size_t size, void* virtualAddress, uint8_t Priviledge);
    bool GetSharing(struct PageTableManager* pageTable, void* key, void* virtualAddress, uint8_t Priviledge);
}