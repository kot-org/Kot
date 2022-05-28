#pragma once
#include <boot/boot.h>

struct KernelInfo{
    struct stivale2_struct_tag_framebuffer framebuffer;
    struct ramfs_t ramfs;
    struct memoryInfo_t* memoryInfo;
    uintptr_t smbios;
    uintptr_t rsdp;
}__attribute__((packed));