#pragma once
#include "../../lib/types.h"
#include "paging.h"
#include "PageMapIndexer.h"
#include "PageFrameAllocator.h"
#include "../memory.h"

class PageTableManager {
    public:
    PageTableManager(PageTable* PML4Address);
    PageTable* PML4;
    void MapMemory(void* virtualMemory, void* physicalMemory);
    void UnmapMememory(void* virtualMemory);
    void MapUserspaceMemory(void* virtualMemory);
};

extern PageTableManager globalPageTableManager;