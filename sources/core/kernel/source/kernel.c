#include <impl/vmm.h>
#include <impl/boot.h>
#include <impl/arch.h>
#include <global/pmm.h>
#include <lib/assert.h>
#include <global/dir.h>
#include <impl/serial.h>
#include <impl/memory.h>
#include <global/heap.h>
#include <impl/initrd.h>
#include <global/ksym.h>
#include <impl/graphics.h>
#include <global/scheduler.h>
#include <global/elf_loader.h>

#include <lib/log.h>

#include <kernel.h>

void test_userspace(void);

/**
 * kernel_entry have to be called with 64 bits enabled!
 * it is preferable to make the less things before jumping into kernel_entry
*/
void kernel_entry(void) {
    serial_init();
    log_info("version  = %s %lu.%lu\n", KERNEL_VERSION, KERNEL_MAJOR, KERNEL_MINOR);
    log_info("branch   = %s\n", KERNEL_BRANCH);
    log_info("arch     = %s\n", KERNEL_ARCH);
    log_info("protocol = %s\n", BOOT_PROTOCOL);
    log_print("\n");

    arch_stage1();

    memory_init();

    log_info("memory available = %lu MiB\n", pmm_get_available() / 0x100000);
    log_info("memory reserved  = %lu MiB\n", pmm_get_reserved() / 0x100000);
    log_print("\n");

    ksym_init();

    initrd_init();
    /* graphics_init needs memory to be init*/
    graphics_init();

    arch_stage2();

    time_init();

    modules_init();

    int err;
    kernel_dir_t* dir = d_open(NULL, "/sda/", &err);
    assert(dir);

    dirent_t* entries = malloc(20 * sizeof(dirent_t));
    uint64_t read_entries_count = 0;

    while(true){
        d_get_entries(entries, 20 * sizeof(dirent_t), &read_entries_count, dir);
        if(read_entries_count == 0){
            break;
        }

        dirent_t* entry = entries;
        for(uint64_t i = 0; i < read_entries_count / sizeof(dirent_t); i++){
            log_printf("%s\n", entry->d_name);
            entry = (dirent_t*)((off_t)entry + entry->d_reclen);
        }
    }

    arch_idle();
}
