#pragma once
#include "../arch/x86-64/atomic/atomic.h"
#include "../arch/x86-64/io/serial/serial.h"

struct RegistersLog {
    void* rax; void* rbx; void* rcx; void* rdx; void* rsi; void* rdi; void* rbp; //push in asm

    void* r8; void* r9; void* r10; void* r11; void* r12; void* r13; void* r14; void* r15; //push in asm

    void* rip; void* cs; void* rflags; void* rsp; void* ss; //push by cpu with an interrupt
}__attribute__((packed));

class Logs{
    public:
        void Message(const char* str, ...);
        void Successful(const char* str, ...);
        void Warning(const char* str, ...);
        void Error(const char* str, ...);
        void PrintRegisters(RegistersLog* registers);
};

extern Logs* globalLogs;