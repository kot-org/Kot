#pragma once
#include <boot/ukl.h>

struct ArchInfo_t{
    uint64_t revision;
    uint64_t architecture;

    struct ukl_framebuffer_t framebuffer;
    struct ukl_initrd_t initrd;
    struct memoryInfo_t* memoryInfo;
    uintptr_t smbios;
    uintptr_t rsdp;

    uint64_t ProcessorCount;

    uint8_t IRQLineStart;
    uint8_t IRQLineSize;

    size64_t IRQSize;
    event_t* IRQEvents; 
    bool* IsIRQEventsFree;   
}__attribute__((packed));