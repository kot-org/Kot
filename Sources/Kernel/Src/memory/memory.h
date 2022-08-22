#pragma once

#include <lib/types.h>
#include <kot/memory.h>

bool CheckAddress(uintptr_t address, size64_t size, uintptr_t pagingEntry);
bool CheckAddress(uintptr_t address, size64_t size);

struct MemoryShareInfo{
    char signature0;
    bool Lock;
    enum MemoryFieldType Type;
    size64_t InitialSize;
    size64_t RealSize;
    uint64_t PageNumber;
    //Parent
    pagetable_t PageTableParent;
    uintptr_t VirtualAddressParent;
    char signature1;
}__attribute__((packed));

uint64_t CreateMemoryField(struct kprocess_t* process, size64_t size, uint64_t* virtualAddressPointer, uint64_t* keyPointer, enum MemoryFieldType type);
uint64_t AcceptMemoryField(struct kprocess_t* process, MemoryShareInfo* shareInfo, uint64_t* virtualAddressPointer);
uint64_t FreeMemoryField(struct kprocess_t* process, MemoryShareInfo* shareInfo, uintptr_t virtualAddress);