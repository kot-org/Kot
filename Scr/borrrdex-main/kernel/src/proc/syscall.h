#pragma once

/* SYSTEM */
#define SYSCALL_HALT    0x001

/* FILE I/O */
#define SYSCALL_READ    0x101
#define SYSCALL_WRITE   0x102

/* PROCESS */
#define SYSCALL_EXIT    0x201

/* MEMORY */
#define SYSCALL_SBRK    0x301

#define STDIN_HANDLE    0x0
#define STDOUT_HANDLE   0x1
#define STDERR_HANDLE   0x2

#include <__config.h>
#include <stdint.h>

__BEGIN_DECLS

void syscall_init();

typedef struct syscall_regs {
    uint64_t rax, rcx, rdx, rbx;
    uint64_t rsp, rbp, rsi, rdi;

    uint64_t r8, r9, r10, r11;
    uint64_t r12, r13, r14, r15;
} syscall_regs_t;

__END_DECLS