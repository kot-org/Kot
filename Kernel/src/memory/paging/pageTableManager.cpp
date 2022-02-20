#include <memory/paging/PageTableManager.h>
#include <scheduler/scheduler.h>
#include <logs/logs.h>

PageTableManager globalPageTableManager[MAX_PROCESSORS];

void PageTableManager::PageTableManagerInit(PageTable* PML4Address){
    this->PML4 = PML4Address;
    this->PhysicalMemoryVirtualAddress = 0;
    this->VirtualAddress = 0x10000000000000000; //It must be a multiple of 0x1000
}

void PageTableManager::DefinePhysicalMemoryLocation(void* PhysicalMemoryVirtualAddress){
    if((uint64_t)PhysicalMemoryVirtualAddress % 0x1000){
        PhysicalMemoryVirtualAddress -= (uint64_t)PhysicalMemoryVirtualAddress % 0x1000;
        PhysicalMemoryVirtualAddress += 0x1000;
    }
    this->PhysicalMemoryVirtualAddressSaver = PhysicalMemoryVirtualAddress;
}

void PageTableManager::DefineVirtualTableLocation(){
    this->PhysicalMemoryVirtualAddress = PhysicalMemoryVirtualAddressSaver;
    globalAllocator.PageBitmap.Buffer = (uint8_t*)GetVirtualAddress(globalAllocator.PageBitmap.Buffer);
}

void PageTableManager::MapMemory(void* virtualMemory, void* physicalMemory){
    PageMapIndexer indexer = PageMapIndexer((uint64_t)virtualMemory);
    PageDirectoryEntry PDE;

    PageTable* PML4VirtualAddress = (PageTable*)GetVirtualAddress(PML4);
    PDE = PML4VirtualAddress->entries[indexer.PDP_i];
    PageTable* PDP;
    PageTable* PDPVirtualAddress;
    if (!PDE.GetFlag(PT_Flag::Present)){
        PDP = (PageTable*)globalAllocator.RequestPage();
        PDPVirtualAddress = (PageTable*)GetVirtualAddress(PDP);  
        memset(PDPVirtualAddress, 0, 0x1000);
        PDE.SetAddress((uint64_t)PDP >> 12);
        PDE.SetFlag(PT_Flag::Present, true);
        PDE.SetFlag(PT_Flag::ReadWrite, true);
        PML4VirtualAddress->entries[indexer.PDP_i] = PDE;
    }
    else
    {
        PDP = (PageTable*)((uint64_t)PDE.GetAddress() << 12);
        PDPVirtualAddress = (PageTable*)GetVirtualAddress(PDP);    
    }

    
    PDE = PDPVirtualAddress->entries[indexer.PD_i];

    PageTable* PD;
    PageTable* PDVirtualAddress;
    if (!PDE.GetFlag(PT_Flag::Present)){
        PD = (PageTable*)globalAllocator.RequestPage();
        PDVirtualAddress = (PageTable*)GetVirtualAddress(PD); 
        memset(PDVirtualAddress, 0, 0x1000);
        PDE.SetAddress((uint64_t)PD >> 12);
        PDE.SetFlag(PT_Flag::Present, true);
        PDE.SetFlag(PT_Flag::ReadWrite, true);
        PDPVirtualAddress->entries[indexer.PD_i] = PDE;
    }
    else
    {
        PD = (PageTable*)((uint64_t)PDE.GetAddress() << 12);
        PDVirtualAddress = (PageTable*)GetVirtualAddress(PD);  
    }


    PDE = PDVirtualAddress->entries[indexer.PT_i];
    PageTable* PT;
    PageTable* PTVirtualAddress;
    if (!PDE.GetFlag(PT_Flag::Present)){
        PT = (PageTable*)globalAllocator.RequestPage();
        PTVirtualAddress = (PageTable*)GetVirtualAddress(PT);
        memset(PTVirtualAddress, 0, 0x1000);
        PDE.SetAddress((uint64_t)PT >> 12);
        PDE.SetFlag(PT_Flag::Present, true);
        PDE.SetFlag(PT_Flag::ReadWrite, true);
        PDVirtualAddress->entries[indexer.PT_i] = PDE;
    }
    else
    {
        PT = (PageTable*)((uint64_t)PDE.GetAddress() << 12);
        PTVirtualAddress = (PageTable*)GetVirtualAddress(PT);  
    }


    PDE = PTVirtualAddress->entries[indexer.P_i];
    PDE.SetAddress((uint64_t)physicalMemory >> 12);
    PDE.SetFlag(PT_Flag::Present, true);
    PDE.SetFlag(PT_Flag::ReadWrite, true);
    PTVirtualAddress->entries[indexer.P_i] = PDE;
}

void* PageTableManager::MapMemory(void* physicalMemory, size_t pages){
    this->VirtualAddress -= pages * 0x1000;
    void* virtualMemory = (void*)this->VirtualAddress;

    for(int i = 0; i < pages; i++){
        MapMemory((void*)((uint64_t)virtualMemory + i * 0x1000), (void*)((uint64_t)physicalMemory + i * 0x1000));
    }

    return virtualMemory;
}

void PageTableManager::UnmapMemory(void* virtualMemory){
    PageMapIndexer indexer = PageMapIndexer((uint64_t)virtualMemory);
    PageDirectoryEntry PDE;

    PageTable* PML4VirtualAddress = (PageTable*)GetVirtualAddress(PML4);
    PDE = PML4VirtualAddress->entries[indexer.PDP_i];
    PageTable* PDP;
    PageTable* PDPVirtualAddress;
    if (!PDE.GetFlag(PT_Flag::Present)){
        PDP = (PageTable*)globalAllocator.RequestPage();
        PDPVirtualAddress = (PageTable*)GetVirtualAddress(PDP);  
        memset(PDPVirtualAddress, 0, 0x1000);
        PDE.SetAddress((uint64_t)PDP >> 12);
        PDE.SetFlag(PT_Flag::Present, true);
        PDE.SetFlag(PT_Flag::ReadWrite, true);
        PML4VirtualAddress->entries[indexer.PDP_i] = PDE;
    }
    else
    {
        PDP = (PageTable*)((uint64_t)PDE.GetAddress() << 12);
        PDPVirtualAddress = (PageTable*)GetVirtualAddress(PDP);    
    }

    
    PDE = PDPVirtualAddress->entries[indexer.PD_i];

    PageTable* PD;
    PageTable* PDVirtualAddress;
    if (!PDE.GetFlag(PT_Flag::Present)){
        PD = (PageTable*)globalAllocator.RequestPage();
        PDVirtualAddress = (PageTable*)GetVirtualAddress(PD); 
        memset(PDVirtualAddress, 0, 0x1000);
        PDE.SetAddress((uint64_t)PD >> 12);
        PDE.SetFlag(PT_Flag::Present, true);
        PDE.SetFlag(PT_Flag::ReadWrite, true);
        PDPVirtualAddress->entries[indexer.PD_i] = PDE;
    }
    else
    {
        PD = (PageTable*)((uint64_t)PDE.GetAddress() << 12);
        PDVirtualAddress = (PageTable*)GetVirtualAddress(PD);  
    }

    PDE = PDVirtualAddress->entries[indexer.PT_i];
    PageTable* PT;
    PageTable* PTVirtualAddress;
    if (!PDE.GetFlag(PT_Flag::Present)){
        PT = (PageTable*)globalAllocator.RequestPage();
        PTVirtualAddress = (PageTable*)GetVirtualAddress(PT);
        memset(PTVirtualAddress, 0, 0x1000);
        PDE.SetAddress((uint64_t)PT >> 12);
        PDE.SetFlag(PT_Flag::Present, true);
        PDE.SetFlag(PT_Flag::ReadWrite, true);
        PDVirtualAddress->entries[indexer.PT_i] = PDE;
    }
    else
    {
        PT = (PageTable*)((uint64_t)PDE.GetAddress() << 12);
        PTVirtualAddress = (PageTable*)GetVirtualAddress(PT);  
    }


    PDE = PTVirtualAddress->entries[indexer.P_i];
    PDE.SetAddress(0);
    PDE.SetFlag(PT_Flag::Present, false);
    PDE.SetFlag(PT_Flag::ReadWrite, false);
    PTVirtualAddress->entries[indexer.P_i] = PDE;
}

void PageTableManager::MapUserspaceMemory(void* virtualMemory) {
    PageMapIndexer indexer = PageMapIndexer((uint64_t)virtualMemory);
    PageDirectoryEntry PDE;

    PageTable* PML4VirtualAddress = (PageTable*)GetVirtualAddress(PML4);
    PDE = PML4VirtualAddress->entries[indexer.PDP_i];
    PageTable* PDP;
    PageTable* PDPVirtualAddress;
    if (!PDE.GetFlag(PT_Flag::Present)){
        return;
    }
    else
    {
        PDP = (PageTable*)((uint64_t)PDE.GetAddress() << 12);
        PDPVirtualAddress = (PageTable*)GetVirtualAddress(PDP); 
    }
    
    PDE.SetFlag(PT_Flag::User, true);
    PML4VirtualAddress->entries[indexer.PDP_i] = PDE;

    PDE = PDPVirtualAddress->entries[indexer.PD_i];

    PageTable* PD;
    PageTable* PDVirtualAddress;
    if (!PDE.GetFlag(PT_Flag::Present)){
        return;
    }
    else
    {
        PD = (PageTable*)((uint64_t)PDE.GetAddress() << 12);
        PDVirtualAddress = (PageTable*)GetVirtualAddress(PD);  
    }

    PDE.SetFlag(PT_Flag::User, true);
    PDPVirtualAddress->entries[indexer.PD_i] = PDE;

    PDE = PDVirtualAddress->entries[indexer.PT_i];

    PageTable* PT;
    PageTable* PTVirtualAddress;
    if (!PDE.GetFlag(PT_Flag::Present)){
        return;
    }
    else
    {
        PT = (PageTable*)((uint64_t)PDE.GetAddress() << 12);
        PTVirtualAddress = (PageTable*)GetVirtualAddress(PT);
    }

    PDE.SetFlag(PT_Flag::User, true);
    PDVirtualAddress->entries[indexer.PT_i] = PDE;

    PDE = PTVirtualAddress->entries[indexer.P_i];
    PDE.SetFlag(PT_Flag::User, true);
    PTVirtualAddress->entries[indexer.P_i] = PDE;
}

void* PageTableManager::GetPhysicalAddress(void* virtualMemory){
    PageMapIndexer indexer = PageMapIndexer((uint64_t)virtualMemory);
    PageDirectoryEntry PDE;

    PageTable* PML4VirtualAddress = (PageTable*)GetVirtualAddress(PML4);
    PDE = PML4VirtualAddress->entries[indexer.PDP_i];
    PageTable* PDP;
    PageTable* PDPVirtualAddress;
    if (!PDE.GetFlag(PT_Flag::Present)){
        return 0;
    }
    else
    {
        PDP = (PageTable*)((uint64_t)PDE.GetAddress() << 12);
        PDPVirtualAddress = (PageTable*)GetVirtualAddress(PDP); 
    }
    
    
    PDE = PDPVirtualAddress->entries[indexer.PD_i];

    PageTable* PD;
    PageTable* PDVirtualAddress;
    if (!PDE.GetFlag(PT_Flag::Present)){
        return 0;
    }
    else
    {
        PD = (PageTable*)((uint64_t)PDE.GetAddress() << 12);
        PDVirtualAddress = (PageTable*)GetVirtualAddress(PD);  
    }

    PDE = PDVirtualAddress->entries[indexer.PT_i];

    PageTable* PT;
    PageTable* PTVirtualAddress;
    if (!PDE.GetFlag(PT_Flag::Present)){
        return 0;
    }
    else
    {
        PT = (PageTable*)((uint64_t)PDE.GetAddress() << 12);
        PTVirtualAddress = (PageTable*)GetVirtualAddress(PT);
    }

    PDE = PTVirtualAddress->entries[indexer.P_i];
    return (void*)((uint64_t)PDE.GetAddress() * 0x1000 + (uint64_t)virtualMemory % 0x1000);
}

void* PageTableManager::GetVirtualAddress(void* physicalAddress){
    return (void*)((uint64_t)PhysicalMemoryVirtualAddress + (uint64_t)physicalAddress);
}

void PageTableManager::CopyAll(PageTableManager* pageTableManagerToCopy){
    DefinePhysicalMemoryLocation(pageTableManagerToCopy->PhysicalMemoryVirtualAddress);
    this->PhysicalMemoryVirtualAddress = PhysicalMemoryVirtualAddressSaver;
    PageTable* PML4VirtualAddressDestination = (PageTable*)GetVirtualAddress(PML4);
    PageTable* PML4VirtualAddressToCopy = (PageTable*)GetVirtualAddress(pageTableManagerToCopy->PML4);
    for(int i = 0; i < 512; i++){
        PageDirectoryEntry PDE = PML4VirtualAddressDestination->entries[i];
        PageTable* PDP;
        PageTable* PDPVirtualAddress;
        if (!PDE.GetFlag(PT_Flag::Present)){
            PDP = (PageTable*)globalAllocator.RequestPage();
            PDPVirtualAddress = (PageTable*)GetVirtualAddress(PDP);  
            memset(PDPVirtualAddress, 0, 0x1000);
            PDE.SetAddress((uint64_t)PDP >> 12);
            PDE.SetFlag(PT_Flag::Present, true);
            PDE.SetFlag(PT_Flag::ReadWrite, true);
            PML4VirtualAddressDestination->entries[i] = PDE;
        }

        PML4VirtualAddressDestination->entries[i] = PML4VirtualAddressToCopy->entries[i];
    }
}

void PageTableManager::CopyHigherHalf(PageTableManager* pageTableManagerToCopy){
    DefinePhysicalMemoryLocation(pageTableManagerToCopy->PhysicalMemoryVirtualAddress);
    this->PhysicalMemoryVirtualAddress = PhysicalMemoryVirtualAddressSaver;
    PageTable* PML4VirtualAddressDestination = (PageTable*)GetVirtualAddress(PML4);
    PageTable* PML4VirtualAddressToCopy = (PageTable*)GetVirtualAddress(pageTableManagerToCopy->PML4);
    for(int i = 256; i < 512; i++){
        PageDirectoryEntry PDE = PML4VirtualAddressDestination->entries[i];
        PageTable* PDP;
        PageTable* PDPVirtualAddress;
        if (!PDE.GetFlag(PT_Flag::Present)){
            PDP = (PageTable*)globalAllocator.RequestPage();
            PDPVirtualAddress = (PageTable*)GetVirtualAddress(PDP);  
            memset(PDPVirtualAddress, 0, 0x1000);
            PDE.SetAddress((uint64_t)PDP >> 12);
            PDE.SetFlag(PT_Flag::Present, true);
            PDE.SetFlag(PT_Flag::ReadWrite, true);
            PML4VirtualAddressDestination->entries[i] = PDE;
        }

        PML4VirtualAddressDestination->entries[i] = PML4VirtualAddressToCopy->entries[i];
    }
}

void PageTableManager::CopyLowerHalf(PageTableManager* pageTableManagerToCopy){
    DefinePhysicalMemoryLocation(pageTableManagerToCopy->PhysicalMemoryVirtualAddress);
    this->PhysicalMemoryVirtualAddress = PhysicalMemoryVirtualAddressSaver;
    PageTable* PML4VirtualAddressDestination = (PageTable*)GetVirtualAddress(PML4);
    PageTable* PML4VirtualAddressToCopy = (PageTable*)GetVirtualAddress(pageTableManagerToCopy->PML4);
    for(int i = 0; i < 256; i++){
        PageDirectoryEntry PDE = PML4VirtualAddressDestination->entries[i];
        PageTable* PDP;
        PageTable* PDPVirtualAddress;
        if (!PDE.GetFlag(PT_Flag::Present)){
            PDP = (PageTable*)globalAllocator.RequestPage();
            PDPVirtualAddress = (PageTable*)GetVirtualAddress(PDP);  
            memset(PDPVirtualAddress, 0, 0x1000);
            PDE.SetAddress((uint64_t)PDP >> 12);
            PDE.SetFlag(PT_Flag::Present, true);
            PDE.SetFlag(PT_Flag::ReadWrite, true);
            PML4VirtualAddressDestination->entries[i] = PDE;
        }

        PML4VirtualAddressDestination->entries[i] = PML4VirtualAddressToCopy->entries[i];
    }
}

void PageTableManager::LoadLowerHalf(){
    this->PhysicalMemoryVirtualAddress = PhysicalMemoryVirtualAddressSaver;
    PageTable* PML4VirtualAddressDestination = (PageTable*)GetVirtualAddress(PML4);
    for(int i = 0; i < 256; i++){
        PageDirectoryEntry PDE = PML4VirtualAddressDestination->entries[i];
        PageTable* PDP;
        PageTable* PDPVirtualAddress;
        if (!PDE.GetFlag(PT_Flag::Present)){
            PDP = (PageTable*)globalAllocator.RequestPage();
            PDPVirtualAddress = (PageTable*)GetVirtualAddress(PDP);  
            memset(PDPVirtualAddress, 0, 0x1000);
            PDE.SetAddress((uint64_t)PDP >> 12);
            PDE.SetFlag(PT_Flag::Present, true);
            PDE.SetFlag(PT_Flag::ReadWrite, true);
            PML4VirtualAddressDestination->entries[i] = PDE;
        }
    }
}

void PageTableManager::ChangePaging(PageTableManager* NewPaging){
    asm("mov %0, %%cr3" :: "r" (NewPaging->PML4));
}

void PageTableManager::RestorePaging(){
    asm("mov %0, %%cr3" :: "r" (PML4));
}

bool PageTableManager::GetFlags(void* virtualMemory, int flags){
    PageMapIndexer indexer = PageMapIndexer((uint64_t)virtualMemory);
    PageDirectoryEntry PDE;

    PageTable* PML4VirtualAddress = (PageTable*)GetVirtualAddress(PML4);
    PDE = PML4VirtualAddress->entries[indexer.PDP_i];
    PageTable* PDP;
    PageTable* PDPVirtualAddress;
    if (!PDE.GetFlag(PT_Flag::Present)){
        return false;
    }
    else
    {
        PDP = (PageTable*)((uint64_t)PDE.GetAddress() << 12);
        PDPVirtualAddress = (PageTable*)GetVirtualAddress(PDP); 
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
        PDVirtualAddress = (PageTable*)GetVirtualAddress(PD);  
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
        PTVirtualAddress = (PageTable*)GetVirtualAddress(PT);
    }

    PDVirtualAddress->entries[indexer.PT_i] = PDE;

    PDE = PTVirtualAddress->entries[indexer.P_i];
    return PDE.GetFlag((PT_Flag)flags);
}

void PageTableManager::SetFlags(void* virtualMemory, int flags, bool value){
    PageMapIndexer indexer = PageMapIndexer((uint64_t)virtualMemory);
    PageDirectoryEntry PDE;

    PageTable* PML4VirtualAddress = (PageTable*)GetVirtualAddress(PML4);
    PDE = PML4VirtualAddress->entries[indexer.PDP_i];
    PageTable* PDP;
    PageTable* PDPVirtualAddress;
    if (!PDE.GetFlag(PT_Flag::Present)){
        return;
    }
    else
    {
        PDP = (PageTable*)((uint64_t)PDE.GetAddress() << 12);
        PDPVirtualAddress = (PageTable*)GetVirtualAddress(PDP); 
    }
    
    PML4VirtualAddress->entries[indexer.PDP_i] = PDE;

    PDE = PDPVirtualAddress->entries[indexer.PD_i];

    PageTable* PD;
    PageTable* PDVirtualAddress;
    if (!PDE.GetFlag(PT_Flag::Present)){
        return;
    }
    else
    {
        PD = (PageTable*)((uint64_t)PDE.GetAddress() << 12);
        PDVirtualAddress = (PageTable*)GetVirtualAddress(PD);  
    }

    PDPVirtualAddress->entries[indexer.PD_i] = PDE;

    PDE = PDVirtualAddress->entries[indexer.PT_i];

    PageTable* PT;
    PageTable* PTVirtualAddress;
    if (!PDE.GetFlag(PT_Flag::Present)){
        return;
    }
    else
    {
        PT = (PageTable*)((uint64_t)PDE.GetAddress() << 12);
        PTVirtualAddress = (PageTable*)GetVirtualAddress(PT);
    }

    PDVirtualAddress->entries[indexer.PT_i] = PDE;

    PDE = PTVirtualAddress->entries[indexer.P_i];
    PDE.SetFlag((PT_Flag)flags, value);
    PTVirtualAddress->entries[indexer.P_i] = PDE;   
}

PageTableManager* PageTableManager::SetupProcessPaging(){
    PageTableManager* ReturnValue = (PageTableManager*)malloc(sizeof(PageTableManager));
    void* PML4 = globalAllocator.RequestPage();
    memset(GetVirtualAddress(PML4), 0, 0x1000);
    ReturnValue->PageTableManagerInit((PageTable*)PML4);
    ReturnValue->CopyHigherHalf(this);
    ReturnValue->LoadLowerHalf();
    ReturnValue->PhysicalMemoryVirtualAddress = PhysicalMemoryVirtualAddress;   
    return ReturnValue;
}

PageTableManager* PageTableManager::SetupThreadPaging(PageTableManager* parent){
    PageTableManager* ReturnValue = (PageTableManager*)malloc(sizeof(PageTableManager));
    void* PML4 = globalAllocator.RequestPage();
    memset(GetVirtualAddress(PML4), 0, 0x1000);
    ReturnValue->PageTableManagerInit((PageTable*)PML4);
    ReturnValue->CopyHigherHalf(this);
    ReturnValue->CopyLowerHalf(parent);
    ReturnValue->PhysicalMemoryVirtualAddress = PhysicalMemoryVirtualAddress; 
    return ReturnValue;      
}