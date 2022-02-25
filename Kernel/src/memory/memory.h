#pragma once

#include <memory/efiMemory.h>
#include <memory/paging/PageTableManager.h>

uint64_t GetMemorySize(EFI_MEMORY_DESCRIPTOR* mMap, uint64_t nMApEntries, uint64_t mMapDescSize);
void memset(void* start, uint8_t value, uint64_t num);
int memcmp(const void *aptr, const void *bptr, size_t n);

void memcpy(void* destination, void* source, uint64_t num);

struct MemoryShareInfo{
    char signature0;
    bool Lock;
    bool ReadOnly;
    size_t Size;
    uint64_t PageNumber;
    //Parent
    struct PageTableManager* PageTableParent;
    void* VirtualAddressParent;
    char signature1;
}__attribute__((packed));

uint64_t CreatSharing(struct thread_t* thread, size_t size, uint64_t* virtualAddressPointer, uint64_t* keyPointer, bool ReadOnly);
uint64_t GetSharing(struct thread_t* thread, MemoryShareInfo* key, uint64_t* virtualAddressPointer);
uint64_t FreeSharing(struct thread_t* thread, void* virtualAddress);