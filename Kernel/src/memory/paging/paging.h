  
#pragma once
#include "../../lib/types.h"

enum PT_Flag {
    Present = 0,
    ReadWrite = 1,
    User = 2,
    WriteThrough = 3,
    CacheDisabled = 4,
    Accessed = 5,
    LargerPages = 7,
    Custom0 = 9,
    Custom1 = 10,
    Custom2 = 11,
};

struct PageDirectoryEntry {
    uint64_t Value;
    void SetFlag(PT_Flag flag, bool enabled);
    bool GetFlag(PT_Flag flag);
    void SetAddress(uint64_t address);
    uint64_t GetAddress();
}__attribute__((packed));

struct PageTable { 
    PageDirectoryEntry entries[512];
}__attribute__((aligned(0x1000)));

extern "C" void LoadPaging(PageTable* PageTableEntry, void* PhysicalMemoryVirtualAddress);