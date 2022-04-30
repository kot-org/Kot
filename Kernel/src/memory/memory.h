#pragma once

#include <lib/types.h>

void memset(void* start, uint8_t value, uint64_t num);
int memcmp(const void *aptr, const void *bptr, size_t n);

void memcpy(void* destination, void* source, uint64_t num);

bool CheckAddress(void* address, size_t size);

enum memory_share_flag{
    memory_share_flag_ReadOnly          = 0,
    /* NLA : no live actualization means that once the memory is get by the client the won't be update 
    if the server memory is update also the address give by the server is suppose tu be already alocated */
    memory_share_flag_NLA               = 1,
};

struct MemoryShareInfo{
    char signature0;
    bool Lock;
    uint64_t flags;
    size_t Size;
    uint64_t PageNumber;
    //Parent
    pagetable_t PageTableParent;
    void* VirtualAddressParent;
    char signature1;
}__attribute__((packed));

uint64_t CreatSharing(struct thread_t* thread, size_t size, uint64_t* virtualAddressPointer, uint64_t* keyPointer, uint64_t flags);
uint64_t GetSharing(struct thread_t* thread, MemoryShareInfo* shareInfo, uint64_t* virtualAddressPointer);
uint64_t FreeSharing(struct thread_t* thread, MemoryShareInfo* shareInfo, void* virtualAddress);