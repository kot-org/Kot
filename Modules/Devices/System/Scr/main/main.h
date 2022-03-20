#pragma once
#include <kot/types.h>

struct Framebuffer{
	void* BaseAddress;
    size_t FrameBufferSize;
	unsigned int Width;
	unsigned int Height;
	unsigned int PixelsPerScanLine;
}__attribute__((packed));

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
    struct Framebuffer* framebuffer;
    struct RamFs* ramfs;
    struct memoryInfo_t* memoryInfo;
    void* smbios;
    void* rsdp;
    struct Timer* timer;
}__attribute__((packed));