#include <impl/arch.h>
#include <lib/assert.h>

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


void arch_stage1(void) {
    __asm__ volatile("cli");
    gdt_init();
    idt_init();
}

void arch_stage2(void) {
    cpu_init();
    simd_init();
    acpi_init();

    struct acpi_hpet_header* hpet = acpi_find_table(acpi_rsdp, "HPET");
    assert(hpet);
    hpet_init(hpet);

    struct acpi_madt_header* madt = acpi_find_table(acpi_rsdp, "APIC");
    assert(madt);
    apic_init(madt);

    smp_init();

    syscall_enable(GDT_KERNEL_CODE * sizeof(gdt_entry_t), GDT_USER_CODE * sizeof(gdt_entry_t));

    start_lapic_timer();

    __asm__ volatile("sti");
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
