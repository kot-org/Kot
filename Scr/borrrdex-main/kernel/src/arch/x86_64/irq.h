#pragma once

#ifndef __cplusplus
#error C++ Only
#endif

#include <cstdint>

constexpr uint32_t EFLAGS_INTERRUPT_FLAG = 1 << 9;

typedef struct regs {
    uint64_t rax, rcx, rdx, rbx;
    uint64_t kernel_rsp, rbp, rsi, rdi;

    uint64_t r8, r9, r10, r11;
    uint64_t r12, r13, r14, r15;

    uint64_t irq, error_code;

    uint64_t rip, cs, rflags, rsp, ss;
} regs_t;

typedef void (*int_handler_t)();