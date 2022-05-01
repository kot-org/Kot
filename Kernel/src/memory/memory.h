#pragma once

#include <lib/types.h>
#include <kot/memory.h>

void memset(void* start, uint8_t value, uint64_t num);
int memcmp(const void *aptr, const void *bptr, size_t n);

void memcpy(void* destination, void* source, uint64_t num);

bool CheckAddress(void* address, size_t size);

struct MemoryShareInfo{
    char signature0;
    bool Lock;
    uint64_t flags;
    size_t Size;
    uint64_t PageNumber;
    //Parent
    pagetable_t PageTableParent;
    void* VirtualAddressParent;
    char signature1;
}__attribute__((packed));

uint64_t CreatSharing(struct process_t* process, size_t size, uint64_t* virtualAddressPointer, uint64_t* keyPointer, uint64_t flags);
uint64_t GetSharing(struct process_t* process, MemoryShareInfo* shareInfo, uint64_t* virtualAddressPointer);
uint64_t FreeSharing(struct process_t* process, MemoryShareInfo* shareInfo, void* virtualAddress);