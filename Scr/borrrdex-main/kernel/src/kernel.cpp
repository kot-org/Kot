#include "acpi/xsdt.h"
#include "acpi/fadt.h"
#include "acpi/mcfg.h"
#include "KernelUtil.h"
#include "memory/Memory.h"
#include "arch/x86_64/io/rtc.h"
#include "arch/x86_64/tss.h"
#include "arch/x86_64/io/serial.h"
#include "graphics/Clock.h"
#include "arch/x86_64/cpuid.h"
#include "arch/x86_64/interrupt/interrupt.h"
#include "drivers/x86_64/pit.h"
#include "Panic.h"
#include "string.h"
#include "paging/PageFrameAllocator.h"
#include "fs/vfs.h"
#include "stdatomic.h"
#include "init/common.h"
#include "thread.h"
#include "stalloc.h"
#include "../../bios/multiboot.h"
#include "drivers/polltty.h"
#include "graphics/kwrite.h"

#include <cstddef>

struct KernelUpdateEntries {
    BasicRenderer *renderer;
    Clock* clock;
    uint16_t tickCount;
};


void render(datetime_t* dt, void* context) {
    KernelUpdateEntries* updateEntries = (KernelUpdateEntries *)context;
    uint16_t tickCount = updateEntries->tickCount++;
    if((tickCount % updateEntries->renderer->get_update_ticks()) == 0) {
        updateEntries->renderer->tick(dt);
    }

    if((tickCount % updateEntries->clock->get_update_ticks()) == 0) {
        updateEntries->clock->tick(dt);
    }
}

extern "C" void init(uint64_t magic, uint8_t* multiboot) {
    multiboot_info_t* mb_info = (multiboot_info_t *)multiboot;
    stalloc_init();
    uart_init();

    polltty_init();
    kwrite("borrrdex - An operating system for learning operating systems\n");
    kwrite("=============================================================\n");
    kwrite("\n");
    kwrite("borrrdex is heavily based on PonchoOS and KUDOS\n");
    kwrite("\n");

    InitializeKernel(mb_info);

    while(true) {
        asm volatile("hlt");
    }
}

extern "C" void _start(BootInfo* bootInfo) {
    stalloc_init();
    uart_init();

    KernelInfo kernelInfo = InitializeKernel(bootInfo);
    if(bootInfo->rsdp) {
        XSDT xsdt((void *)bootInfo->rsdp->xdst_address);
        FADT fadt(xsdt.get(FADT::signature));
        if(fadt.is_valid()) {
            century_register = fadt.data()->century;
        }
    }

    Clock clk;
    KernelUpdateEntries u {
        GlobalRenderer,
        &clk,
        0
    };

    rtc_chain_t renderChain = {
        render,
        &u,
        NULL
    };

    register_rtc_cb(&renderChain);

    tid_t startup_thread = thread_create(init_startup_thread, 0);
    thread_run(startup_thread);

    thread_switch();
    //while(true) asm("hlt");
}