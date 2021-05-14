#include "cswitch.h"
#include "gdt/gdt.h"
#include "arch/x86_64/irq.h"
#include "KernelUtil.h"
#include "paging/PageTableManager.h"
#include "thread.h"
#include "tss.h"
#include "interrupt/interrupt.h"
#include "scheduler.h"

constexpr uint32_t THREAD_FLAGS = 0x200202;

void context_init(context_t* ctx, uint64_t entry, uint64_t endentry, uint64_t stack, uint32_t args) {
    if(stack & 0xF) {
        stack = (stack & 0xFFFFFFFFFFFFFFF0);
        stack += 0x10;
    }

    uint64_t* rsp = (uint64_t *)stack;
    uint64_t rbp = stack;

    *(rsp--) = endentry;
    *(rsp--) = GDT_SELECTOR_KERNEL_DATA;
    *(rsp--) = rbp;
    *(rsp--) = THREAD_FLAGS;
    *(rsp--) = GDT_SELECTOR_KERNEL_CODE;
    *(rsp--) = entry;

    rbp = (uint64_t)rsp;

    // Registers
    *(rsp--) = 0; // R15
    *(rsp--) = 0; // R14
    *(rsp--) = 0; // R13
    *(rsp--) = 0; // R12
    *(rsp--) = 0; // R11
    *(rsp--) = 0; // R10
    *(rsp--) = 0; // R9
    *(rsp--) = 0; // R8
    *(rsp--) = (uint64_t)args; // RDI
    *(rsp--) = 0; // RSI
    *(rsp--) = rbp; // RBP
    *(rsp--) = 0; // RSP
    *(rsp--) = 0; // RBX
    *(rsp--) = 0; // RDX
    *(rsp--) = 0; // RCX
    *(rsp) = 0; // RAX

    ctx->stack = rsp;
    
    ctx->pml4 = (uint64_t)KernelPageTableManager()->PML4Address();
    ctx->virt_memory = (uint64_t *)KernelPageTableManager()->PML4Address();
    ctx->flags = 0;
}

void context_enter_userland(context_t* ctx) {
    uintptr_t stack = ((uintptr_t)ctx->stack) - 0x10;
    if(stack & 0xF) {
        stack = (stack & 0xFFFFFFFFFFFFFFF0);
        stack += 0x10;
    }

    uint64_t* rsp = (uint64_t *)stack;
    uint64_t rbp = stack;

    *(rsp--) = 0;
    *(rsp--) = GDT_SELECTOR_USER_DATA | 0b11;
    *(rsp--) = rbp;
    *(rsp--) = THREAD_FLAGS;
    *(rsp--) = GDT_SELECTOR_USER_CODE | 0b11;
    *(rsp--) = ctx->rip;

    rbp = (uint64_t)rsp;

    // Registers
    *(rsp--) = 0; // R15
    *(rsp--) = 0; // R14
    *(rsp--) = 0; // R13
    *(rsp--) = 0; // R12
    *(rsp--) = 0; // R11
    *(rsp--) = 0; // R10
    *(rsp--) = 0; // R9
    *(rsp--) = 0; // R8
    *(rsp--) = 0; // RDI
    *(rsp--) = 0; // RSI
    *(rsp--) = rbp; // RBP
    *(rsp--) = 0; // RSP
    *(rsp--) = 0; // RBX
    *(rsp--) = 0; // RDX
    *(rsp--) = 0; // RCX
    *(rsp) = 0; // RAX

    ctx->stack = rsp;

    thread_get_current_thread_entry()->user_context = ctx;
    thread_get_current_thread_entry()->attribs |= THREAD_FLAG_ENTERUSER;

    thread_switch();

    while(true) {
        asm volatile("hlt");
    }

    __builtin_unreachable();
}

void context_enable_ints(context_t* ctx) {
    ctx->flags |= EFLAGS_INTERRUPT_FLAG;

    ctx->pml4 = (uint64_t)KernelPageTableManager()->PML4Address();
    ctx->virt_memory = (uint64_t *)KernelPageTableManager()->PML4Address();
    ctx->flags = 0;
}

void context_set_sp(context_t* ctx, uint64_t sp) {
    ctx->stack = (uint64_t *)sp;
}

void context_set_ip(context_t* ctx, uint64_t ip) {
    ctx->rip = ip;
}

extern "C" struct dirty_dirty_hack {
    uint64_t stack;
    uint64_t pml4;
};

extern "C" struct dirty_dirty_hack task_switch(uint64_t* stack) {
    thread_table_t* task = thread_get_current_thread_entry();
    uint64_t new_stack;

    if(task->attribs & THREAD_FLAG_USERMODE) {
        task->user_context->stack = stack;
    } else {
        task->context->stack = stack;
    }

    scheduler_schedule();

    task = thread_get_current_thread_entry();

    tss_setstack(interrupt_get_cpu(), (uint64_t)task->context->stack);

    if(task->attribs & THREAD_FLAG_ENTERUSER) {
        task->attribs &= ~THREAD_FLAG_ENTERUSER;
        task->attribs |= THREAD_FLAG_USERMODE;
    }

    if(task->attribs & THREAD_FLAG_USERMODE) {
        new_stack = (uint64_t)task->user_context->stack;
    } else {
        new_stack = (uint64_t)task->context->stack;
    }

    return {
        .stack = new_stack,
        .pml4 = task->context->pml4
    };
}