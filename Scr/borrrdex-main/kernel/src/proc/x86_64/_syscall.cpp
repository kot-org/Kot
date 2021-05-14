#include "proc/syscall.h"
#include "arch/x86_64/interrupt/idt.h"
#include "arch/x86_64/interrupt/interrupt.h"
#include "arch/x86_64/gdt/gdt.h"
#include "arch/x86_64/tss.h"


extern uintptr_t syscall_entry(uintptr_t, uintptr_t, uintptr_t, uintptr_t);

extern "C" {
    void __syscall_setup();
}

extern "C" void syscall_init() {
    __syscall_setup();
}

extern "C" void syscall_handle(syscall_regs_t* registers) {
    WithInterrupts wi(true);

    registers->rax = syscall_entry(registers->rdi, registers->rsi, registers->rdx, registers->r8);
}