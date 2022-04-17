#pragma once
#include <kot/types.h>

struct stivale2_struct_tag_framebuffer {
    uint64_t identifier;
    uint64_t NAME;
    uint64_t framebuffer_addr;
    uint16_t framebuffer_width;
    uint16_t framebuffer_height;
    uint16_t framebuffer_pitch;
    uint16_t framebuffer_bpp;
    uint8_t  memory_model;
    uint8_t  red_mask_size;
    uint8_t  red_mask_shift;
    uint8_t  green_mask_size;
    uint8_t  green_mask_shift;
    uint8_t  blue_mask_size;
    uint8_t  blue_mask_shift;
    uint8_t  unused;
};

struct RamFs{
	void* RamFsBase;
	size_t Size;
}__attribute__((packed));

struct memoryInfo_t{
    uint64_t totalMemory;
    uint64_t freeMemory;
    uint64_t reservedMemory;
    uint64_t usedMemory;    
}__attribute__((packed));

struct Timer{
    uint64_t* Counter;
    uint64_t Frequency;
}__attribute__((packed));

struct KernelInfo{
    struct stivale2_struct_tag_framebuffer* framebuffer;
    struct RamFs* ramfs;
    struct memoryInfo_t* memoryInfo;
    void* smbios;
    void* rsdp;
}__attribute__((packed));