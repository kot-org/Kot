#include <stdint.h>
#include <stddef.h>
#include "boot/bootInfo.h"
#include "graphics.h"
#include "lib/stdio/cstr.h"
#include "efiMemory.h"
#include "memory.h"
#include "bitmap.h"
#include "userspace/userspace/userspace.h"
#include "paging/pageFrameAllocator.h"
#include "paging/pageMapIndexer.h"
#include "paging/PageTableManager.h"
#include "paging/paging.h"
#include "gdt/gdt.h"
#include "interrupts/IDT.h"
#include "interrupts/interrupts.h"
#include "IO/IO.h"
#include "userInput/mouse.h"
#include "hardware/acpi/acpi.h"
#include "hardware/pci/pci.h"
#include "hardware/apic/apic.h"
#include "hardware/cpu/cpu.h"
#include "memory/heap.h"
#include "scheduling/pit/pit.h"
#include "lib/limits.h"
#include "drivers/rtc/rtc.h"
#include "drivers/fpu/fpu.h"
#include "io/msr/msr.h"
#include "tss/tss.h"

extern "C" void main(BootInfo*);

extern uint64_t _KernelStart;
extern uint64_t _KernelEnd;

void InitializeKernel(BootInfo* bootInfo);