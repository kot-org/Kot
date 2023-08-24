#ifndef _IMPL_ARCH_H
#define _IMPL_ARCH_H 1

#include <arch/include.h>

/* 
The file in ARCH_INCLUDE(impl/vmm.h) is expected to have :
    - The declaration of struct : cpu_context_t
    - The following macros :
        - ARCH_CONTEXT_ARG0(context)
        - ARCH_CONTEXT_ARG1(context)
        - ARCH_CONTEXT_ARG2(context)
        - ARCH_CONTEXT_ARG3(context)
        - ARCH_CONTEXT_ARG4(context)
        - ARCH_CONTEXT_ARG5(context)
        - ARCH_CONTEXT_RETURN(context)
        - ARCH_CONTEXT_SYSCALL_SELECTOR(context)
*/
#include ARCH_INCLUDE(impl/arch.h)

/**
 * arch specific stuff that don't need pmm and vmm to be initialized or that are needed to initialize vmm
*/
void arch_stage1(void);

/**
 * arch specific stuff that need heap to be initialize
*/
void arch_stage2(void);

#include <stdnoreturn.h>

void arch_pause(void);
noreturn void arch_idle(void);
noreturn void arch_reboot(void);
noreturn void arch_shutdown(void);

#endif // _ARCH_H
