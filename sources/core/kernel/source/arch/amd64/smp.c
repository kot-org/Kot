#include <impl/arch.h>
#include <arch/include.h>
#include ARCH_INCLUDE(gdt.h)
#include ARCH_INCLUDE(idt.h)
#include ARCH_INCLUDE(smp.h)
#include ARCH_INCLUDE(cpu.h)
#include ARCH_INCLUDE(simd.h)
#include ARCH_INCLUDE(apic.h)
#include ARCH_INCLUDE(syscall.h)

void trampoline_main(void){
    gdt_init_cpu();

    idt_init_cpu();

    cpu_init();

    simd_init();

    enable_apic(cpu_get_apicid());

    syscall_enable(GDT_KERNEL_CODE * sizeof(gdt_entry_t), GDT_USER_CODE * sizeof(gdt_entry_t));

    start_lapic_timer();

    data_trampoline.status = 0xef;

    __asm__ volatile("sti");

    arch_idle();
}