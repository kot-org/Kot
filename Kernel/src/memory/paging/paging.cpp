#include <memory/paging/paging.h>

void PageDirectoryEntry::SetFlag(PT_Flag flag, bool enabled){
    uint64_t bitSelector = (uint64_t)1 << flag;
    Value &= ~bitSelector;
    if (enabled){
        Value |= bitSelector;
    }
}

bool PageDirectoryEntry::GetFlag(PT_Flag flag){
    uint64_t bitSelector = (uint64_t)1 << flag;
    return Value & bitSelector > 0 ? true : false;
}

uint64_t PageDirectoryEntry::GetAddress(){
    return (Value & 0x000ffffffffff000) >> 12;
}

void PageDirectoryEntry::SetAddress(uint64_t address){
    address &= 0x000000ffffffffff;
    Value &= 0xfff0000000000fff;
    Value |= (address << 12);
}

bool GetFlags(void* PagingEntry, void* virtualMemory, int flags){
    PageMapIndexer indexer = PageMapIndexer((uint64_t)virtualMemory);
    PageDirectoryEntry PDE;

    PageTable* PML4VirtualAddress = (PageTable*)globalPageTableManager[CPU::GetCoreID()].GetVirtualAddress(PagingEntry);
    PDE = PML4VirtualAddress->entries[indexer.PDP_i];
    PageTable* PDP;
    PageTable* PDPVirtualAddress;
    if (!PDE.GetFlag(PT_Flag::Present)){
        return false;
    }
    else
    {
        PDP = (PageTable*)((uint64_t)PDE.GetAddress() << 12);
        PDPVirtualAddress = (PageTable*)globalPageTableManager[CPU::GetCoreID()].GetVirtualAddress(PDP); 
    }
    
    PML4VirtualAddress->entries[indexer.PDP_i] = PDE;

    PDE = PDPVirtualAddress->entries[indexer.PD_i];

    PageTable* PD;
    PageTable* PDVirtualAddress;
    if (!PDE.GetFlag(PT_Flag::Present)){
        return false;
    }
    else
    {
        PD = (PageTable*)((uint64_t)PDE.GetAddress() << 12);
        PDVirtualAddress = (PageTable*)globalPageTableManager[CPU::GetCoreID()].GetVirtualAddress(PD);  
    }

    PDPVirtualAddress->entries[indexer.PD_i] = PDE;

    PDE = PDVirtualAddress->entries[indexer.PT_i];

    PageTable* PT;
    PageTable* PTVirtualAddress;
    if (!PDE.GetFlag(PT_Flag::Present)){
        return false;
    }
    else
    {
        PT = (PageTable*)((uint64_t)PDE.GetAddress() << 12);
        PTVirtualAddress = (PageTable*)globalPageTableManager[CPU::GetCoreID()].GetVirtualAddress(PT);
    }

    PDVirtualAddress->entries[indexer.PT_i] = PDE;

    PDE = PTVirtualAddress->entries[indexer.P_i];
    return PDE.GetFlag((PT_Flag)flags);
}