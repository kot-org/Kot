#pragma once

#include <arch/arch.h>
#include <kot/types.h>
#include <memory/memory.h>
#include <lib/stdio/cstr.h>
#include <misc/bitmap/bitmap.h>
#include <drivers/acpi/acpi.h>
#include <drivers/hpet/hpet.h>
#include <heap/heap.h>
#include <lib/limits.h>
#include <drivers/rtc/rtc.h>
#include <syscall/syscall.h>
#include <boot/boot.h>
#include <logs/logs.h>
#include <FS/RamFS.h>
#include <elf/elf.h>

struct KernelInfo{
    struct stivale2_struct_tag_framebuffer* framebuffer;
    struct RamFs* ramfs;
    struct memoryInfo_t* memoryInfo;
    void* smbios;
    void* rsdp;
}__attribute__((packed));

extern "C" void main(stivale2_struct*);

void InitializeKernel(stivale2_struct* stivale2_struct);

#define HigherHalfAddress 0xFFFF800000000000