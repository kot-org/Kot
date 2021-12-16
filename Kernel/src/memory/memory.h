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
        bool ReadOnly;
        size_t Size;
        uint64_t PageNumber;
        //Parent
        struct PageTableManager* PageTableParent;
        void* VirtualAddressParent;
    }__attribute__((packed));

    bool CreatSharing(struct PageTableManager* pageTable, size_t size, uint64_t* virtualAddressPointer, uint64_t* keyPointer, bool ReadOnly, uint8_t Priviledge);
    bool GetSharing(struct PageTableManager* pageTable, void* key, uint64_t* virtualAddressPointer, uint8_t Priviledge);
}