#pragma once
#include <kot/types.h>
#include <elf/elf.h>
#include <ramfs/ramfs.h>
#include <ipc/ipc.h>

struct framebuffer_t{
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

struct ramfs_t{
	uintptr_t address;
	size_t size;
}__attribute__((packed));

struct memoryInfo_t{
    uint64_t totalPageMemory;
    uint64_t freePageMemory;
    uint64_t reservedPageMemory;
    uint64_t usedPageMemory;    
}__attribute__((packed));

#define KERNEL_INFO_SIZE 0x5

struct KernelInfo{
    uint64_t revision;
    uint64_t architecture;

    struct framebuffer_t framebuffer;
    struct ramfs_t ramfs;
    struct memoryInfo_t* memoryInfo;
    uintptr_t smbios;
    uintptr_t rsdp;

    uint8_t IRQLineStart;
    uint8_t IRQLineSize;
}__attribute__((packed));

struct InfoSlot{
    size_t size;
    uintptr_t address;
}__attribute__((packed));