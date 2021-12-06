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
        bool IsGetByClient;
        size_t Size;
        uint64_t PageNumber;
        uint64_t PIDTask;
        //Parent
        PageTableManager* PageTableParent;
        void* VirtualAddressParent;
    }__attribute__((packed));

    void* CreatSharing(PageTableManager* pageTable, size_t size, void* virtualAddress, uint64_t TaskPID);
    bool GetSharing(PageTableManager* pageTable, void* key, void* virtualAddress, uint64_t TaskPID);
}