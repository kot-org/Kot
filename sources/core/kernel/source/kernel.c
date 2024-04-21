#include <impl/vmm.h>
#include <impl/boot.h>
#include <impl/arch.h>
#include <global/pmm.h>
#include <lib/assert.h>
#include <global/dir.h>
#include <global/file.h>
#include <impl/serial.h>
#include <impl/memory.h>
#include <global/heap.h>
#include <impl/initrd.h>
#include <global/ksym.h>
#include <global/apps.h>
#include <impl/graphics.h>
#include <global/scheduler.h>
#include <global/elf_loader.h>

#include <lib/log.h>

#include <kernel.h>

/**
 * kernel_entry have to be called with 64 bits enabled!
 * it is preferable to make the less things before jumping into kernel_entry
*/
void kernel_entry(void) {
    serial_init();
    log_success("serial : initialized\n");

    initrd_init();
    log_success("initrd : initialized\n");

    graphics_init();
    log_success("graphics : initialized\n");
 
    log_info("version  = %s %lu.%lu\n", KERNEL_VERSION, KERNEL_MAJOR, KERNEL_MINOR);
    log_info("branch   = %s\n", KERNEL_BRANCH);
    log_info("arch     = %s\n", KERNEL_ARCH);
    log_info("protocol = %s\n", BOOT_PROTOCOL);
    log_print("\n");

    arch_stage1();
    log_success("arch stage 1 : done\n");

    memory_init();
    log_success("memory : initialized\n");

    log_info("memory available = %lu MiB\n", pmm_get_available() / 0x100000);
    log_info("memory reserved  = %lu MiB\n", pmm_get_reserved() / 0x100000);
    log_print("\n");

    ksym_init();

    vfs_init();
    log_success("vfs : initialized\n");

    arch_stage2();
    log_success("arch stage 2 : done\n");

    /* scheduler_init needs vfs to be init */ 
    scheduler_init();
    log_success("scheduler : initialized\n");

    arch_stage3();
    log_success("arch stage 3 : done\n");

    time_init();
    log_success("time : initialized\n");

    s_init();
    log_success("socket : initialized\n");

    modules_init();
    log_success("modules : initialized\n");

    apps_init();

    arch_idle();
}
