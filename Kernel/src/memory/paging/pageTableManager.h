#pragma once
#include "../../lib/types.h"
#include "paging.h"
#include "PageMapIndexer.h"
#include "PageFrameAllocator.h"
#include "../memory.h"

#define MAX_PROCESSORS      256

struct PageTableManager {
    void PageTableManagerInit(PageTable* PML4Address);
    void DefinePhysicalMemoryLocation(void* PhysicalMemoryVirtualAddress);
    void DefineVirtualTableLocation();
    void MapMemory(void* virtualMemory, void* physicalMemory);
    void* MapMemory(void* physicalMemory, size_t pages);
    void UnmapMemory(void* virtualMemory);
    void MapUserspaceMemory(void* virtualMemory);
    void* GetPhysicalAddress(void* virtualAddress);
    void* GetVirtualAddress(void* physicalAddress); //this function exist because physicall address is locate at the higher half
    void CopyHigherHalf(PageTableManager* pageTableManagerToCopy);
    void ChangePaging(PageTableManager* NewPaging);
    void RestorePaging();
    bool GetFlags(void* virtualMemory, int flags);
    void SetFlags(void* virtualMemory, int flags, bool value);
    void* PhysicalMemoryVirtualAddressSaver;
    void* PhysicalMemoryVirtualAddress;
    PageTable* PML4;
    uint64_t VirtualAddress;
}__attribute__((packed));

extern PageTableManager globalPageTableManager[MAX_PROCESSORS];