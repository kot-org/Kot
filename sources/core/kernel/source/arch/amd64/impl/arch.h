#ifndef _AMD64_IMPL_ARCH_H
#define _AMD64_IMPL_ARCH_H

#include <stdint.h>

#define KERNEL_STACK_SIZE 0x10000

#define ARCH_CONTEXT_ARG0(context)              ((context)->rdi)
#define ARCH_CONTEXT_ARG1(context)              ((context)->rsi)
#define ARCH_CONTEXT_ARG2(context)              ((context)->rdx)
#define ARCH_CONTEXT_ARG3(context)              ((context)->rcx)
#define ARCH_CONTEXT_ARG4(context)              ((context)->r8)
#define ARCH_CONTEXT_ARG5(context)              ((context)->r9)
#define ARCH_CONTEXT_THREAD(context)            ((context)->rdi)
#define ARCH_CONTEXT_RETURN(context)            ((context)->rax)
#define ARCH_CONTEXT_SYSCALL_SELECTOR(context)  ((context)->ctx_info->thread)

typedef struct{
    void* kernel_stack;
    uint64_t cs;
    uint64_t ss;
    void* thread;
}__attribute__((packed)) context_info_t;

typedef struct{
    uint64_t cr3;

    context_info_t* ctx_info;
    
    uint64_t rax;
    uint64_t rbx;
    uint64_t rcx;
    uint64_t rdx;
    uint64_t rsi;  
    uint64_t rdi;
    uint64_t rbp;

    uint64_t r8;
    uint64_t r9;
    uint64_t r10;
    uint64_t r11;
    uint64_t r12;
    uint64_t r13;
    uint64_t r14;
    uint64_t r15;

    uint64_t interrupt_number; 
    uint64_t error_code; 
    
    uint64_t rip; 
    uint64_t cs; 
    uint64_t rflags; 
    uint64_t rsp; 
    uint64_t ss;
}__attribute__((packed)) cpu_context_t; 

#endif // _AMD64_IMPL_ARCH_H