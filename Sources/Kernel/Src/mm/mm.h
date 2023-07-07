#pragma once

#include <lib/vector.h>
#include <heap/heap.h>

#define BLOCK_SIZE PAGE_SIZE

struct MemoryHandler_t{
    void* Base;
    size_t Size;
    uint64_t RegionCount;
    struct MemoryRegion_t* FirstRegion;
    struct MemoryRegion_t* LastFreeRegion;
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
    bool IsMap;
    locker_t Lock;

    struct MemoryRegion_t* Next;
    struct MemoryRegion_t* Last;
};


MemoryHandler_t* MMCreateHandler(pagetable_t Paging, void* Base, size_t Size);
MemoryHandler_t* MMCloneHandler(pagetable_t Paging, MemoryHandler_t* Source);
KResult MMFreeHandler(MemoryHandler_t* Handler);

MemoryRegion_t* MMAllocateRegionVM(MemoryHandler_t* Handler, void* Base, size_t Size, int Flags, int Prot, int* Errno);
KResult MMFree(MemoryHandler_t* Handler, void* Base, size_t Size);
MemoryRegion_t* MMGetRegion(MemoryHandler_t* Handler, void* Base);

KResult MMAllocateMemoryBlock(MemoryHandler_t* Handler, MemoryRegion_t* Region);
KResult MMAllocateMemoryBlockMaster(MemoryHandler_t* Handler, MemoryRegion_t* Region);
KResult MMAllocateMemoryContigous(MemoryHandler_t* Handler, MemoryRegion_t* Region);
KResult MMMapPhysical(MemoryHandler_t* Handler, MemoryRegion_t* Region, void* Base);

KResult MMProtect(MemoryHandler_t* Handler, void* Base, size_t Size, int Prot);

struct SlaveInfo_t{
    struct kprocess_t* process;
    void* virtualAddress;
};


struct MemoryShareInfo{
    char signature0;
    locker_t Lock;
    enum MemoryFieldType Type;
    size64_t InitialSize;
    size64_t RealSize;
    uint64_t PageNumber;
    //Parent
    struct kprocess_t* Parent;
    pagetable_t PageTableParent;
    void* VirtualAddressParent;
    class KStack* SlavesList;
    uint64_t SlavesNumber;
    uint64_t Offset;
    char signature1;
};


uint64_t MMCreateMemoryField(struct kthread_t* self, struct kprocess_t* process, size64_t size, uint64_t* virtualAddressPointer, uint64_t* keyPointer, enum MemoryFieldType type);
uint64_t MMAcceptMemoryField(struct kthread_t* self, struct kprocess_t* process, struct MemoryShareInfo* shareInfo, uint64_t* virtualAddressPointer);
uint64_t MMCloseMemoryField(struct kthread_t* self, struct kprocess_t* process, struct MemoryShareInfo* shareInfo, void* virtualAddress);