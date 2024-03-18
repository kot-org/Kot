#include <lib/log.h>
#include <impl/arch.h>
#include <lib/assert.h>
#include <global/hw_interrupt.h>

#include <arch/include.h>
#include ARCH_INCLUDE(idt.h)
#include ARCH_INCLUDE(gdt.h)
#include ARCH_INCLUDE(cpu.h)
#include ARCH_INCLUDE(apic.h)
#include ARCH_INCLUDE(acpi.h)
#include ARCH_INCLUDE(simd.h)
#include ARCH_INCLUDE(boot.h)
#include ARCH_INCLUDE(hpet.h)
#include ARCH_INCLUDE(syscall.h)
#include ARCH_INCLUDE(interrupts.h)


void arch_stage1(void) {
    __asm__ volatile("cli");
    log_success("arch stage 1 : interrupts disabled\n");

    gdt_init();
    log_success("arch stage 1 : gdt initialized\n");

    idt_init();
    log_success("arch stage 1 : idt initialized\n");
}

void arch_stage2(void) {
    hw_interrupt_init(256);

    for(int i = 0; i < 32; i++){
        hw_interrupt_lock(i);
    }

    hw_interrupt_lock(INT_SCHEDULE);
    hw_interrupt_lock(INT_SCHEDULE_APIC_TIMER);

    cpu_init();
    log_success("arch stage 2 : cpu initialized\n");

    simd_init();
    log_success("arch stage 2 : simd initialized\n");

    acpi_init();
    log_success("arch stage 2 : acpi initialized\n");

    struct acpi_hpet_header* hpet = acpi_find_table(acpi_rsdp, "HPET");
    assert(hpet);
    hpet_init(hpet);
    log_success("arch stage 2 : hpet initialized\n");

    struct acpi_madt_header* madt = acpi_find_table(acpi_rsdp, "APIC");
    assert(madt);
    apic_init(madt);
    log_success("arch stage 2 : madt initialized\n");
}

void arch_stage3(void) {
    smp_init();
    log_success("arch stage 3 : smp initialized\n");

    syscall_enable(GDT_KERNEL_CODE * sizeof(gdt_entry_t), GDT_USER_CODE * sizeof(gdt_entry_t));
    log_success("arch stage 3 : syscall initialized\n");

    start_lapic_timer();
    log_success("arch stage 3 : lapic timer initialized\n");

    __asm__ volatile("sti");
    log_success("arch stage 3 : interrupt enabled\n");
}

void arch_pause(void) {
    __asm__ volatile("pause");
}

noreturn void arch_idle(void) {
    for (;;) {
        __asm__ volatile("hlt");
    }
}

noreturn void arch_reboot(void) {
    // todo
    arch_idle();
}

noreturn void arch_shutdown(void) {
    // todo
    arch_idle();
}
