#pragma once
#include <boot/boot.h>

struct ArchInfo_t{
    uint64_t revision;
    uint64_t architecture;

    struct stivale2_struct_tag_framebuffer framebuffer;
    struct initrd_t initrd;
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