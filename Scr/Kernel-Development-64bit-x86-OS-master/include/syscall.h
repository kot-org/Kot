#ifndef _SYSCALL_H
#define _SYSCALL_H
#include <sys/defs.h>

static __inline uint64_t syscall_0(uint64_t n) {
        uint64_t ret = -1;
        __asm volatile("movq %1,%%rax;" // move content of 'n' into rax
                   "int $0x80;"  // call interrupt to run system routione
                   "movq %%rax,%0;" // move the result of the system routine into the ret variable to be returned
                   : "=r"(ret) // list of variables that are outputs 
                   : "r"(n) // list of variables that are inputs 
    : "rax", "memory"); // list of clobbered regs

    return ret;
}

static __inline uint64_t syscall_1(uint64_t n, uint64_t a1) {
        uint64_t ret = -1;
        __asm volatile("movq %1,%%rax;" // move content of 'n' into rax
                   "movq %2,%%rbx;" // move content of 'a1' into rbx
                   "int $0x80;" // call interrupt to run system routione
                   "movq %%rax,%0;" // move the result of the system routine into the ret variable to be returned
                   : "=r"(ret) // list of variables that are outputs
                   : "r"(n), // list of variables that are inputs
                   "r"(a1)
    : "rax", "rbx", "memory"); // list of clobbered regs

        
    return ret;
}

static __inline uint64_t syscall_2(uint64_t n, uint64_t a1, uint64_t a2) {
    uint64_t ret = -1;
        __asm volatile("movq %1,%%rax;" // move content of 'n' into rax
                   "movq %2,%%rbx;" // move content of 'a1' into rbx
                   "movq %3,%%rsi;" // move content of 'a2' into rsi
                   "int $0x80;" // call interrupt to run system routione
                   "movq %%rax,%0;" // move the result of the system routine into the ret variable to be returned
                   : "=r"(ret) // list of variables that are outputs
                   : "r"(n), // list of variables that are inputs
                   "r"(a1),
                   "r"(a2)
    : "rax", "rbx", "rsi", "memory"); // list of clobbered regs

//kprintf("%x",ret);
    return ret;
}

static __inline uint64_t syscall_3(uint64_t n, uint64_t a1, uint64_t a2, uint64_t a3) {
    uint64_t ret = -1;
        __asm volatile("movq %1,%%rax;" // move content of 'n' into rax
                   "movq %2,%%rbx;" // move content of 'a1' into rbx
                   "movq %3,%%rsi;" // move content of 'a2' into rsi
                   "movq %4,%%rdx;" // move content of 'a3' into rdx
                   "int $0x80;" // call interrupt to run system routione
                   "movq %%rax,%0;" // move the result of the system routine into the ret variable to be returned
                   : "=r"(ret) // list of variables that are outputs
                   : "r"(n), // list of variables that are inputs
                   "r"(a1),
                   "r"(a2),
                   "r"(a3)
    : "rax", "rbx", "rsi", "rdx", "memory"); // list of clobbered regs


    return ret;
}


#endif
