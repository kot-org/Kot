#pragma once
#include "../lib/types.h"
#include "../boot/bootInfo.h"
#include "../lib/stdio/cstr.h"
#include "../memory/efiMemory.h"
#include "../memory/memory.h"
#include "../misc/bitmap/bitmap.h"
#include "../arch/x86-64/userspace/userspace.h"
#include "../memory/paging/pageFrameAllocator.h"
#include "../memory/paging/pageMapIndexer.h"
#include "../memory/paging/PageTableManager.h"
#include "../memory/paging/paging.h"
#include "../arch/x86-64/gdt/gdt.h"
#include "../arch/x86-64/interrupts/idt.h"
#include "../arch/x86-64/interrupts/interrupts.h"
#include "../arch/x86-64/io/io.h"
#include "../drivers/acpi/acpi.h"
#include "../arch/x86-64/apic/apic.h"
#include "../drivers/hpet/hpet.h"
#include "../arch/x86-64/smp/smp.h"
#include "../arch/x86-64/cpu/cpu.h"
#include "../memory/heap/heap.h"
#include "../lib/limits.h"
#include "../drivers/rtc/rtc.h"
#include "../drivers/fpu/fpu.h"
#include "../drivers/sse/sse.h"
#include "../syscall/syscall.h"
#include "../arch/x86-64/io/msr/msr.h"
#include "../arch/x86-64/io/serial/serial.h"
#include "../arch/x86-64/tss/tss.h"
#include "../RamFS/RamFS.h"
#include "../logs/logs.h"
#include "../elf/elf.h"

struct KernelInfo{
    struct Framebuffer* framebuffer;
    struct RamFs* ramfs;
    struct memoryInfo_t* memoryInfo;
    void* smbios;
    void* rsdp;
}__attribute__((packed));

extern "C" void main(BootInfo*);

void InitializeKernel(BootInfo* bootInfo);

#define HigherHalfAddress 0xFFFF800000000000