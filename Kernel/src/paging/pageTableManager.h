#pragma once
#include "paging.h"

struct PageLevelIndexes {
	uint16_t	L4_i;	// Index to the page directory pointer table
	uint16_t	L3_i;	// Index to the page directory table
	uint16_t	L2_i;	// Index to the page table
	uint16_t	L1_i;	// Index to the page
};

class PageTableManager {
    public:
    PageTableManager(PageTable* PML4Address);
    PageTable* PML4;
    void MapMemory(void* virtualMemory, void* physicalMemory);
    void UnmapMememory(void* virtualMemory);
    void MapUserspaceMemory(void* virtualMemory);
    void GetIndexes(void* address, PageLevelIndexes* out);
};

extern PageTableManager globalPageTableManager;