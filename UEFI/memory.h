/* Data */

typedef struct PageMapIndexer {
    uint64_t PDP_i;
    uint64_t PD_i;
    uint64_t PT_i;
    uint64_t P_i;
};

enum PT_Flag {
    Present = 0,
    ReadWrite = 1,
    WriteThrough = 3,
    CacheDisabled = 4,
    Accessed = 5,
};

typedef struct PageTable { 
    uint64_t entries[512];
}__attribute__((aligned(0x1000)));


/* Code */

uint64_t SetFlag(uint64_t Value, int flag, bool enabled){
    uint64_t bitSelector = (uint64_t)1 << flag;
    Value &= ~bitSelector;
    if (enabled){
        Value |= bitSelector;
    }
    return Value;
}

bool GetFlag(uint64_t Value, int flag){
    uint64_t bitSelector = (uint64_t)1 << flag;
    return Value & bitSelector > 0 ? true : false;
}

uint64_t GetAddress(uint64_t Value){
    return (Value & 0x000ffffffffff000) >> 12;
}

uint64_t SetAddress(uint64_t Value, uint64_t address){
    address &= 0x000000ffffffffff;
    Value &= 0xfff0000000000fff;
    Value |= (address << 12);
    return Value;
}


void* allocatePage(){
    EFI_PHYSICAL_ADDRESS PhysicalBuffer;
    UINTN Pages;
    Pages = 1;
    SystemTable->BootServices->AllocatePages(AllocateAnyPages, EfiBootServicesData, Pages, &PhysicalBuffer);
    return PhysicalBuffer;
}

void* allocatePages(uint64_t pages){
    EFI_PHYSICAL_ADDRESS PhysicalBuffer;
    UINTN Pages;
    Pages = pages;
    SystemTable->BootServices->AllocatePages(AllocateAnyPages, EfiBootServicesData, Pages, &PhysicalBuffer);
    return PhysicalBuffer;
}

uint64_t GetMemorySize(EFI_MEMORY_DESCRIPTOR* mMap, uint64_t mMapEntries, uint64_t mMapDescSize){
    static uint64_t memorySizeBytes = 0;
    if (memorySizeBytes > 0) return memorySizeBytes;

    for (int i = 0; i < mMapEntries; i++){
        EFI_MEMORY_DESCRIPTOR* desc = (EFI_MEMORY_DESCRIPTOR*)((uint64_t)mMap + (i * mMapDescSize));
        memorySizeBytes += desc->NumberOfPages * 0x1000;
    }
    return memorySizeBytes;
}

struct PageMapIndexer PageMapIndexer(uint64_t virtualAddress){
    struct PageMapIndexer pageMapIndexer;
    virtualAddress >>= 12;
    pageMapIndexer.P_i = virtualAddress & 0x1ff;
    virtualAddress >>= 9;
    pageMapIndexer.PT_i = virtualAddress & 0x1ff;
    virtualAddress >>= 9;
    pageMapIndexer.PD_i = virtualAddress & 0x1ff;
    virtualAddress >>= 9;
    pageMapIndexer.PDP_i = virtualAddress & 0x1ff;
    return pageMapIndexer;
}

void MapMemory(void* PML4, void* virtualAddress, void* physicalAddress){
    struct PageMapIndexer indexer = PageMapIndexer((uint64_t)virtualAddress);
    uint64_t PDE;
    
    struct PageTable* Table = (struct PageTable*)PML4;
    PDE = Table->entries[indexer.PDP_i];
    uint64_t PDP;
    if (!GetFlag(PDE, 0)){
        PDP = allocatePage();
        memset(PDP, 0, 0x1000);
        PDE = SetAddress(PDE, PDP >> 12);
        PDE = SetFlag(PDE, 0, true);
        PDE = SetFlag(PDE, 1, true);
        Table->entries[indexer.PDP_i] = PDE;
    }
    else
    {
        PDP = (uint64_t)GetAddress(PDE) << 12;
    }
    
    Table = (struct PageTable*)PDP;
    PDE = Table->entries[indexer.PD_i];
    uint64_t PD;
    if (!GetFlag(PDE, 0)){
        PD = allocatePage();
        memset(PD, 0, 0x1000);
        PDE = SetAddress(PDE, (uint64_t)PD >> 12);
        PDE = SetFlag(PDE, 0, true);
        PDE = SetFlag(PDE, 1, true);
        Table->entries[indexer.PD_i] = PDE;
    }
    else
    {
        PD = (uint64_t)GetAddress(PDE) << 12;
    }

    Table = (struct PageTable*)PD;
    PDE = Table->entries[indexer.PT_i];
    uint64_t PT;
    if (!GetFlag(PDE, 0)){
        PT = allocatePage();
        memset(PT, 0, 0x1000);
        PDE = SetAddress(PDE, (uint64_t)PT >> 12);
        PDE = SetFlag(PDE, 0, true);
        PDE = SetFlag(PDE, 1, true);
        Table->entries[indexer.PT_i] = PDE;
    }
    else
    {
        PT = (uint64_t)GetAddress(PDE) << 12;
    }

    Table = (struct PageTable*)PT;
    PDE = Table->entries[indexer.P_i];
    PDE = SetAddress(PDE, (uint64_t)physicalAddress >> 12);
    PDE = SetFlag(PDE, 0, true);
    PDE = SetFlag(PDE, 1, true);
    Table->entries[indexer.P_i] = PDE;
}

void allocatePagesVirtualAddress(void* PML4, uint64_t pages, void* segment){
    for(uint64_t i = 0; i < pages; i += 0x1000){
        void* PhysicalBuffer = allocatePage();
        MapMemory(PML4, (void*)segment + i, (void*)PhysicalBuffer);
    }
}

void* InitMemory(EFI_MEMORY_DESCRIPTOR* Map, uint64_t MapSize, uint64_t MapDescSize){
    void* PML4 = allocatePage();
    memset(PML4, 0, 0x1000);

    uint64_t MapEntries = MapSize / MapDescSize;

    uint64_t MemorySize = GetMemorySize(Map, MapEntries, MapDescSize);
    MapMemory(PML4, (void*)Map, (void*)Map);

    for (uint64_t t = 0; t < MemorySize; t += 0x1000){
        MapMemory(PML4, (void*)t, (void*)t);
    }

    asm ("mov %0, %%cr3" :: "r" (PML4));
    return PML4;
}
