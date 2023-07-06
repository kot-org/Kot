#pragma once

#include <lib/sys.h>
#include <arch/arch.h>
#include <lib/types.h>
#include <lib/stack/stack.h>
#include <scheduler/scheduler.h>

void memset(void* start, uint8_t value, size64_t size);
void memset16(void* start, uint16_t value, size64_t size);
void memset32(void* start, uint32_t value, size64_t size);
void memset64(void* start, uint64_t value, size64_t size);

int memcmp(const void *aptr, const void *bptr, size64_t size);
void memcpy(void* destination, void* source, size64_t size);

bool CheckAddress(void* address, size64_t size, void* pagingEntry);
bool CheckAddress(void* address, size64_t size);

bool CheckUserAddress(void* address, size64_t size, void* pagingEntry);
bool CheckUserAddress(void* address, size64_t size);

struct SlaveInfo_t{
    struct kprocess_t* process;
    void* virtualAddress;
}__attribute__((packed));

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
}__attribute__((packed));

uint64_t CreateMemoryField(struct kthread_t* self, struct kprocess_t* process, size64_t size, uint64_t* virtualAddressPointer, uint64_t* keyPointer, enum MemoryFieldType type);
uint64_t AcceptMemoryField(struct kthread_t* self, struct kprocess_t* process, MemoryShareInfo* shareInfo, uint64_t* virtualAddressPointer);
uint64_t CloseMemoryField(struct kthread_t* self, struct kprocess_t* process, MemoryShareInfo* shareInfo, void* virtualAddress);

int AllocateAnonymousVMObject(void** Base, size_t Size, bool Hint, bool Fixed);