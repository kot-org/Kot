#pragma once
#include <lib/va_args.h>
#include <arch/x86-64/atomic/atomic.h>
#include <arch/x86-64/io/serial/serial.h>

struct RegistersLog {
    uintptr_t rax; uintptr_t rbx; uintptr_t rcx; uintptr_t rdx; uintptr_t rsi; uintptr_t rdi; uintptr_t rbp; //push in asm

    uintptr_t r8; uintptr_t r9; uintptr_t r10; uintptr_t r11; uintptr_t r12; uintptr_t r13; uintptr_t r14; uintptr_t r15; //push in asm

    uintptr_t rip; uintptr_t cs; uintptr_t rflags; uintptr_t rsp; uintptr_t ss; //push by cpu with an interrupt
}__attribute__((packed));

void Message(const char* str, ...);
void Successful(const char* str, ...);
void Warning(const char* str, ...);
void Error(const char* str, ...);
void PrintRegisters(RegistersLog* registers);