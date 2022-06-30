#pragma once
#include <boot/boot.h>

struct KernelInfo{
    uint64_t revision;
    uint64_t architecture;

    struct stivale2_struct_tag_framebuffer framebuffer;
    struct ramfs_t ramfs;
    struct memoryInfo_t* memoryInfo;
    uintptr_t smbios;
    uintptr_t rsdp;
}__attribute__((packed));