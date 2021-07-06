#pragma once

#include <stdint.h>
#include "../../graphics.h"
#include "../../lib/stdio.h"
#include "../../hardware/cpu/cpu.h"
#include "../../hardware/atomic/atomic.h"
#include "../../interrupts/interrupts.h"
extern bool wait;
extern "C" void EnableSystemCall();
extern "C" void syscall_entry();

typedef struct SyscallStack{
    void* rax; void* rbx; void* rcx; void* rdx; void* KernelRsp; void* rsi; void* rdi; void* rbp; //push in asm

    void* r8; void* r9; void* r10; void* r11; void* r12; void* r13; void* r14; void* r15; //push in asm
};