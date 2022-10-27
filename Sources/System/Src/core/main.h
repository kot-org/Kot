#pragma once

#include <kot/sys.h>
#include <kot/heap.h>
#include <kot/types.h>
#include <kot/memory.h>
#include <kot++/json.h>

#include <elf/elf.h>
#include <srv/srv.h>
#include <uisd/uisd.h>
#include <acpi/acpi.h>
#include <initrd/initrd.h>
#include <interrupts/interrupts.h>

struct bootframebuffer_t {
    uint64_t framebuffer_base;
    uint64_t framebuffer_width;
    uint64_t framebuffer_height;
    uint64_t framebuffer_pitch;
    uint64_t framebuffer_bpp;
}__attribute__((packed));

struct initrd_t {
	uint64_t base;
	size64_t size;
}__attribute__((packed));

struct memoryInfo_t{
    uint64_t totalPageMemory;
    uint64_t freePageMemory;
    uint64_t reservedPageMemory;
    uint64_t usedPageMemory;    
}__attribute__((packed));

struct KernelInfo {
    uint64_t Revision;
    uint64_t Architecture;

    struct bootframebuffer_t Framebuffer;
    struct initrd_t initrd;
    struct memoryInfo_t* MemoryInfo;
    uintptr_t Smbios;
    uintptr_t Rsdp;

    uint64_t ProcessorCount;

    uint8_t IRQLineStart;
    uint8_t IRQLineSize;

    size64_t IRQSize;
    event_t IRQEvents[];
}__attribute__((packed));

struct InfoSlot {
    size64_t size;
    uintptr_t address;
}__attribute__((packed));

extern process_t proc;