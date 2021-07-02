#include "PageTableManager.h"
#include "PageMapIndexer.h"
#include <stdint.h>
#include "PageFrameAllocator.h"
#include "../memory.h"

PageTableManager globalPageTableManager = NULL;

PageTableManager::PageTableManager(PageTable* PML4Address){
    this->PML4 = PML4Address;
}

void PageTableManager::MapMemory(void* virtualMemory, void* physicalMemory){
    PageMapIndexer indexer = PageMapIndexer((uint64_t)virtualMemory);
    PageDirectoryEntry PDE;

    PDE = PML4->entries[indexer.PDP_i];
    PageTable* PDP;
    if (!PDE.GetFlag(PT_Flag::Present)){
        PDP = (PageTable*)globalAllocator.RequestPage();
        memset(PDP, 0, 0x1000);
        PDE.SetAddress((uint64_t)PDP >> 12);
        PDE.SetFlag(PT_Flag::Present, true);
        PDE.SetFlag(PT_Flag::ReadWrite, true);
        PML4->entries[indexer.PDP_i] = PDE;
    }
    else
    {
        PDP = (PageTable*)((uint64_t)PDE.GetAddress() << 12);
    }
    
    
    PDE = PDP->entries[indexer.PD_i];
    PageTable* PD;
    if (!PDE.GetFlag(PT_Flag::Present)){
        PD = (PageTable*)globalAllocator.RequestPage();
        memset(PD, 0, 0x1000);
        PDE.SetAddress((uint64_t)PD >> 12);
        PDE.SetFlag(PT_Flag::Present, true);
        PDE.SetFlag(PT_Flag::ReadWrite, true);
        PDP->entries[indexer.PD_i] = PDE;
    }
    else
    {
        PD = (PageTable*)((uint64_t)PDE.GetAddress() << 12);
    }

    PDE = PD->entries[indexer.PT_i];
    PageTable* PT;
    if (!PDE.GetFlag(PT_Flag::Present)){
        PT = (PageTable*)globalAllocator.RequestPage();
        memset(PT, 0, 0x1000);
        PDE.SetAddress((uint64_t)PT >> 12);
        PDE.SetFlag(PT_Flag::Present, true);
        PDE.SetFlag(PT_Flag::ReadWrite, true);
        PD->entries[indexer.PT_i] = PDE;
    }
    else
    {
        PT = (PageTable*)((uint64_t)PDE.GetAddress() << 12);
    }

    PDE = PT->entries[indexer.P_i];
    PDE.SetAddress((uint64_t)physicalMemory >> 12);
    PDE.SetFlag(PT_Flag::Present, true);
    PDE.SetFlag(PT_Flag::ReadWrite, true);
    PT->entries[indexer.P_i] = PDE;
}



void PageTableManager::GetIndexes(void* address, PageLevelIndexes* out){
    uint64_t addr = (uint64_t)address;
    addr>>=12;
    out->L1_i = addr & 0x01FF;
    addr>>=9;
    out->L2_i = addr & 0x01FF;
    addr>>=9;
    out->L3_i = addr & 0x01FF;
    addr>>=9;
    out->L4_i = addr & 0x01FF;
}

void PageTableManager::MapUserspaceMemory(void* virtualMemory) {
    PageDirectoryEntry entry;
    PageLevelIndexes indexes;
    GetIndexes(virtualMemory, &indexes);

    entry = PML4->entries[indexes.L4_i];
    entry.SetFlag(PT_Flag::UserSuper, true);
    PML4->entries[indexes.L4_i] = entry;
    PageTable* l3 = (PageTable*)((uint64_t)entry.GetAddress() << 12);
    entry = l3->entries[indexes.L3_i];
    entry.SetFlag(PT_Flag::UserSuper, true);
    l3->entries[indexes.L3_i] = entry;
    PageTable* l2 = (PageTable*)((uint64_t)entry.GetAddress() << 12);
    entry = l2->entries[indexes.L2_i];
    entry.SetFlag(PT_Flag::UserSuper, true);
    l2->entries[indexes.L2_i] = entry;
    PageTable* l1 = (PageTable*)((uint64_t)entry.GetAddress() << 12);
    entry = l1->entries[indexes.L1_i];
    entry.SetFlag(PT_Flag::UserSuper, true);
    l1->entries[indexes.L1_i] = entry;
}