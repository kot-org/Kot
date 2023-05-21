#pragma once

#include <lib/sys.h>
#include <arch/arch.h>
#include <lib/types.h>
#include <lib/stack/stack.h>
#include <scheduler/scheduler.h>

void memset(uintptr_t start, uint8_t value, size64_t size);
void memset16(uintptr_t start, uint16_t value, size64_t size);
void memset32(uintptr_t start, uint32_t value, size64_t size);
void memset64(uintptr_t start, uint64_t value, size64_t size);

int memcmp(const void *aptr, const void *bptr, size64_t size);
void memcpy(uintptr_t destination, uintptr_t source, size64_t size);

bool CheckAddress(uintptr_t address, size64_t size, uintptr_t pagingEntry);
bool CheckAddress(uintptr_t address, size64_t size);

bool CheckUserAddress(uintptr_t address, size64_t size, uintptr_t pagingEntry);
bool CheckUserAddress(uintptr_t address, size64_t size);

struct SlaveInfo_t{
    struct kprocess_t* process;
    uintptr_t virtualAddress;
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
    uintptr_t VirtualAddressParent;
    class KStack* SlavesList;
    uint64_t SlavesNumber;
    uint64_t Offset;
    char signature1;
}__attribute__((packed));

uint64_t CreateMemoryField(struct kthread_t* self, struct kprocess_t* process, size64_t size, uint64_t* virtualAddressPointer, uint64_t* keyPointer, enum MemoryFieldType type);
uint64_t AcceptMemoryField(struct kthread_t* self, struct kprocess_t* process, MemoryShareInfo* shareInfo, uint64_t* virtualAddressPointer);
uint64_t CloseMemoryField(struct kthread_t* self, struct kprocess_t* process, MemoryShareInfo* shareInfo, uintptr_t virtualAddress);