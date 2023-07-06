#pragma once

#include <lib/vector.h>
#include <heap/heap.h>

#define BLOCK_SIZE PAGE_SIZE

struct MemoryHandler_t{
    void* Base;
    size_t Size;
    kot_vector_t* Regions;
    pagetable_t Paging;
    uint64_t MemoryAllocated;
    locker_t Lock;
};


struct MemoryRegion_t{
    void* Base;
    size_t Size;
    uint64_t BlockCount;
    int Flags;
    int Prot;
    bool IsFree;
    uint64_t Index;
    bool IsMap;
    locker_t Lock;
};


MemoryHandler_t* MMCreateHandler(pagetable_t Paging, void* Base, size_t Size);
MemoryHandler_t* MMCloneHandler(pagetable_t Paging, MemoryHandler_t* Source);
KResult MMFreeHandler(MemoryHandler_t* Handler);

MemoryRegion_t* MMAllocateRegionVM(MemoryHandler_t* Handler, void* Base, size_t Size, int Flags, int Prot);
KResult MMFreeRegionVM(MemoryHandler_t* Handler, MemoryRegion_t* Region, size_t Size);
MemoryRegion_t* MMGetRegion(MemoryHandler_t* Handler, void* Base);

KResult MMAllocateMemoryBlock(MemoryHandler_t* Handler, MemoryRegion_t* Region);
KResult MMAllocateMemoryContigous(MemoryHandler_t* Handler, MemoryRegion_t* Region);
KResult MMMapPhysical(MemoryHandler_t* Handler, MemoryRegion_t* Region, void* Base);

KResult MMProtect(MemoryHandler_t* Handler, MemoryRegion_t* Region);