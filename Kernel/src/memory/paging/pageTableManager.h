#pragma once
#include <lib/types.h>
#include <memory/memory.h>
#include <memory/paging/paging.h>
#include <memory/paging/PageMapIndexer.h>
#include <memory/paging/PageFrameAllocator.h>

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
    void CopyAll(PageTableManager* pageTableManagerToCopy);
    void CopyHigherHalf(PageTableManager* pageTableManagerToCopy);
    void CopyLowerHalf(PageTableManager* pageTableManagerToCopy);
    void LoadLowerHalf();
    void ChangePaging(PageTableManager* NewPaging);
    void RestorePaging();
    bool GetFlags(void* virtualMemory, int flags);
    void SetFlags(void* virtualMemory, int flags, bool value);
    PageTableManager* SetupProcessPaging();
    PageTableManager* SetupThreadPaging(PageTableManager* parent);
    void* PhysicalMemoryVirtualAddressSaver;
    void* PhysicalMemoryVirtualAddress;
    PageTable* PML4;
    uint64_t VirtualAddress;
}__attribute__((packed));

extern PageTableManager globalPageTableManager[MAX_PROCESSORS];