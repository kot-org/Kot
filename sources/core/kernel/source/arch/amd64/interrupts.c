#include <stdbool.h>
#include <lib/log.h>
#include <impl/arch.h>
#include <impl/panic.h>
#include <global/ksym.h>
#include <global/syscall.h>
#include <global/scheduler.h>

#include <arch/include.h>
#include ARCH_INCLUDE(asm.h)
#include ARCH_INCLUDE(apic.h)
#include ARCH_INCLUDE(impl/arch.h)
#include ARCH_INCLUDE(interrupts.h)

char* exceptions_list[32] = {
    "DivisionByZero",
    "Debug",
    "Non Maskable Interrupt",
    "Breakpoint",
    "Overflow",
    "Bound Range Exceeded",
    "Invalid Opcode",
    "Device Not Available",
    "Double Fault",
    "Coprocessor Segment Overrun",
    "Invalid TSS",
    "Segment Not Present",
    "Stack Fault",
    "General Protection Fault",
    "Page Fault",
    "Reserved",
    "x87 Floating-Point Exception",
    "Alignment Check",
    "Machine Check",
    "SIMD Floating-Point Exception",
    "Virtualization Exception",
    "Control Protection Exception",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Hypervisor Injection Exception",
    "VMM Communication Exception",
    "Security Exception",
};

struct stack_frame{
    struct stack_frame* rbp;
    uint64_t rip;
}__attribute__((packed));

static bool is_panicking;

static void dump_registers(cpu_context_t* ctx) {
    log_print("REGISTERS : \n");
    log_printf("rip: %s(%p) | rsp: %p\n", ksym_get_name((void*)ctx->rip), ctx->rip, ctx->rsp);
    log_printf("cr2: %p | cr3: %p\n", asm_read_cr2(), ctx->cr3);
    log_printf("cs : %p  | ss : %p | rflags: %p\n", ctx->cs, ctx->ss, ctx->rflags);

    log_printf("\n");

    log_printf("rax: %p | rbx: %p\n", ctx->rax, ctx->rbx);
    log_printf("rcx: %p | rdx: %p\n", ctx->rcx, ctx->rdx);
    log_printf("rsi: %p | rdi: %p\n", ctx->rsi, ctx->rdi);
    log_printf("rbp: %p | r8 : %p\n", ctx->rbp, ctx->r8);
    log_printf("r9 : %p | r10: %p\n", ctx->r9, ctx->r10);
    log_printf("r11: %p | r12: %p\n", ctx->r11, ctx->r12);
    log_printf("r13: %p | r14: %p\n", ctx->r13, ctx->r14);
    log_printf("r15: %p\n", ctx->r15);
    log_print("\n");
    log_print("------------------------------------------------------------\n");
}

static void dump_backtrace(cpu_context_t* ctx) {
    log_print("BACKTRACE : \n");
    struct stack_frame* frame = (struct stack_frame*)ctx->rbp;

    while (frame) {
        log_printf("- %s(%p)\n", ksym_get_name((void*)frame->rip), frame->rip);
        frame = frame->rbp;
    }
    log_print("\n");
    log_print("------------------------------------------------------------\n");
}

static void interrupt_error_handler(cpu_context_t* ctx, uint8_t cpu_id) {
    if(is_panicking) {
        arch_idle();
    }

    is_panicking = true;

    log_print("------------------------------------------------------------\n");
    log_print("KERNEL FATAL EXCEPTION : \n");
    log_print("\n");
    log_print("------------------------------------------------------------\n");

    dump_registers(ctx);

    dump_backtrace(ctx);

    log_print("\n");

    panic("exception : %s | error code : %d | cpu id : %d\n", exceptions_list[ctx->interrupt_number], ctx->error_code, cpu_id);
}

void interrupt_handler(cpu_context_t* ctx, uint8_t cpu_id) {
    if(ctx->interrupt_number == INT_SCHEDULE_APIC_TIMER){
        scheduler_handler(ctx);
    }else if(ctx->interrupt_number < 32) {
        interrupt_error_handler(ctx, cpu_id);
    }else{
        // TODO
    }

    local_apic_eoi(cpu_id);
}