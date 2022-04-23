#pragma once

struct KernelInfo{
    struct stivale2_struct_tag_framebuffer* framebuffer;
    struct RamFs* ramfs;
    struct memoryInfo_t* memoryInfo;
    void* smbios;
    void* rsdp;
}__attribute__((packed));