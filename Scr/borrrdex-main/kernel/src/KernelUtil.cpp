#include "KernelUtil.h"
#include "string.h"
#include "arch/x86_64/gdt/gdt.h"
#include "arch/x86_64/interrupt/interrupt.h"
#include "arch/x86_64/pic.h"
#include "drivers/x86_64/pit.h"
#include "drivers/x86_64/keyboard.h"
#include "userinput/keymaps.h"
#include "arch/x86_64/io/io.h"
#include "proc/syscall.h"
#include "stalloc.h"
#include "arch/x86_64/io/serial.h"
#include "pci/pci.h"
#include "acpi/xsdt.h"
#include "acpi/mcfg.h"
#include "fs/vfs.h"
#include "memory/heap.h"
#include "init/init.h"
#include "scheduler.h"
#include "thread.h"
#include "Panic.h"
#include "acpi/apic.h"
#include "arch/x86_64/interrupt/idt.h"
#include "../../bios/multiboot.h"

PageTableManager gPageTableManager(NULL);

static void PrepareMemory(BootInfo* bootInfo) {
    uint64_t mMapEntries = bootInfo->mMapSize / bootInfo->mMapDescriptorSize;
    PageFrameAllocator* allocator = PageFrameAllocator::SharedAllocator();
    allocator->ReadEFIMemoryMap(bootInfo->mMap, bootInfo->mMapSize, bootInfo->mMapDescriptorSize);

    uint64_t memorySize = GetMemorySize(bootInfo->mMap, mMapEntries, bootInfo->mMapDescriptorSize);
    PageTableManager::SetSystemMemorySize(memorySize);
    PageTableManager::SetFramebuffer(bootInfo->framebuffer);

    PageTable* PML4 = (PageTable *)allocator->RequestPage();
    memset(PML4, 0, 0x1000);
    gPageTableManager = PageTableManager(PML4);
    gPageTableManager.WriteToCR3();

    thread_get_current_thread_entry()->pml4 = PML4;
}

static void PrepareMemory(multiboot_info_t* bootInfo) {
    PageFrameAllocator* allocator = PageFrameAllocator::SharedAllocator();
    allocator->ReadMultibootMap(bootInfo);
    uint64_t memorySize = bootInfo->memory_low + bootInfo->memory_high;
    PageTableManager::SetSystemMemorySize(memorySize * 1024);

    PageTable* PML4 = (PageTable *)allocator->RequestPage();
    memset(PML4, 0, 0x1000);
    gPageTableManager = PageTableManager(PML4);
    gPageTableManager.WriteToCR3();

    thread_get_current_thread_entry()->pml4 = PML4;
}

PageTableManager* KernelPageTableManager() {
    return &gPageTableManager;
}

void PrepareInterrupts() {
    interrupt_init();
    syscall_init();

    keyboard_init();
    //ps2_mouse_init();
    pit_init();
    rtc_init();
}

static BasicRenderer r(NULL, NULL);
static void* s_system_rsdp_address;
KernelInfo InitializeKernel(BootInfo* bootInfo) {
    r = BasicRenderer(bootInfo->framebuffer, bootInfo->psf1_font);
    GlobalRenderer = &r;

    GlobalRenderer->Clear();
    
    GlobalRenderer->Printf("borrrdex - An operating system for learning operating systems\n");
    GlobalRenderer->Printf("=============================================================\n");
    GlobalRenderer->Printf("\n");
    GlobalRenderer->Printf("borrrdex is heavily based on PonchoOS and KUDOS\n");
    GlobalRenderer->Printf("\n");
    GlobalRenderer->Printf("See serial output for more boot information...\n");

    uart_print("Setting up memory...\r\n");
    PrepareMemory(bootInfo);

    heap_init((void *)0x90000000, 0x10);

    uart_print("Setting up interrupts...\r\n");
    KeyboardMapFunction = JP109Keyboard::translate;
    PrepareInterrupts();
    s_system_rsdp_address = bootInfo->rsdp;
    XSDT xsdtObj((void *)bootInfo->rsdp->xdst_address);
    apic_init((madt_t *)(xsdtObj.get(MADT::signature)));

    thread_table_init();
    scheduler_init();
    modules_init();
    vfs_init();


    //bsp_init();
    
    return {
        &gPageTableManager
    };
}

KernelInfo InitializeKernel(multiboot_info_t* bootInfo) {
    for(uint64_t i = 0xe0000; i < 0xfffff; i += 16) {
        if(strncmp(RSDP::signature, (const char *)i, 8) == 0) {
            s_system_rsdp_address = (void *)i;
            break;
        }
    }

    uart_print("Setting up memory...\r\n");
    PrepareMemory(bootInfo);

    heap_init((void *)0x90000000, 0x10);
    uart_print("Setting up interrupts...\r\n");
    KeyboardMapFunction = JP109Keyboard::translate;
    PrepareInterrupts();

    thread_table_init();
    scheduler_init();
    modules_init();
    vfs_init();


    //bsp_init();
    
    return {
        &gPageTableManager
    };
}

const void* SystemRSDPAddress() {
    return s_system_rsdp_address;
}

void *__dso_handle;

extern "C" int __cxa_atexit(void (*destructor) (void *), void *arg, void *dso) {
    destructor(arg);
    return 0;
}

void* operator new(unsigned long size) {
    return kmalloc(size);
}

void operator delete(void* address) {
    return kfree(address);
}

#if UINT32_MAX == UINTPTR_MAX
#define STACK_CHK_GUARD 0xe2dee396
#else
#define STACK_CHK_GUARD 0x595e9fbd94fda766
#endif
 
uintptr_t __stack_chk_guard = STACK_CHK_GUARD;
 
extern "C" void __stack_chk_fail()
{
	Panic("Stack smashing detected");
}

namespace std {
    [[noreturn]]
    void __throw_length_error(char const * msg) {
        Panic(msg);
        while(true) asm("hlt");
    }

    [[noreturn]]
    void __throw_bad_alloc() {
        Panic("Out of memory from C++ allocation!");
        while(true) asm("hlt");
    }
}