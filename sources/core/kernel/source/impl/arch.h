#ifndef _IMPL_ARCH_H
#define _IMPL_ARCH_H 1

#include <arch/include.h>

/* 
The file in ARCH_INCLUDE(impl/vmm.h) is expected to have :
    - The declaration of struct as type : cpu_context_t
    - The declaration of typedef : arch_context_arg_t
    - The declaration of typedef : arch_cpu_id_t
    - The declaration of variable : arch_max_cpu_id
    - The following macros :
        - ARCH_CONTEXT_SYSCALL_ARG0(context)
        - ARCH_CONTEXT_SYSCALL_ARG1(context)
        - ARCH_CONTEXT_SYSCALL_ARG2(context)
        - ARCH_CONTEXT_SYSCALL_ARG3(context)
        - ARCH_CONTEXT_SYSCALL_ARG4(context)
        - ARCH_CONTEXT_SYSCALL_ARG5(context)
        - ARCH_CONTEXT_IP(context)
        - ARCH_CONTEXT_SP(context)
        - ARCH_CONTEXT_ARG0(context)
        - ARCH_CONTEXT_ARG1(context)
        - ARCH_CONTEXT_ARG2(context)
        - ARCH_CONTEXT_ARG3(context)
        - ARCH_CONTEXT_ARG4(context)
        - ARCH_CONTEXT_ARG5(context)
        - ARCH_CONTEXT_RETURN(context)
        - ARCH_CONTEXT_CURRENT_THREAD(context)
        - ARCH_CONTEXT_CURRENT_THREAD_FIELD(context)
        - ARCH_CONTEXT_SYSCALL_SELECTOR(context)
        - ARCH_GET_CONTEXT_FROM_THREAD(thread)
*/
#include ARCH_INCLUDE(impl/arch.h)

/**
 * arch specific stuff that don't need pmm and vmm to be initialized or that are needed to initialize vmm
*/
void arch_stage1(void);

/**
 * arch specific stuff that need heap to be initialized
*/
void arch_stage2(void);

/**
 * arch specific stuff that need scheduler to be initialized
*/
void arch_stage3(void);

#include <stdnoreturn.h>

void arch_pause(void);
noreturn void arch_idle(void);
noreturn void arch_reboot(void);
noreturn void arch_shutdown(void);

#endif // _ARCH_H
