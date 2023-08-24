#include <impl/arch.h>

void arch_stage1(void) {

}

void arch_stage2(void) {
    
}

void arch_pause(void) {
    
}

noreturn void arch_idle(void) {
    for (;;) {
        __asm__ volatile("wfi");
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
