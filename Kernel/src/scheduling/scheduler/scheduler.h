#pragma once

#include <stdint.h>
#include "../../userspace/userspace/userspace.h"
#include "../../paging/pageTableManager.h"
#include "../../memory/heap.h"
#include "../../lib/limits.h"
#include "../../lib/stdio.h"
#include "../../tss/tss.h"
#include "../../graphics.h"

struct Registers{
    uint64_t rax = 0;
    uint64_t rbx = 0;
    uint64_t rcx = 0;
    uint64_t rdx = 0;
    uint64_t rsi = 0;
    uint64_t rdi = 0;
    uint64_t rbp = 0;
    uint64_t rsp = 0;
    uint64_t r8 = 0;
    uint64_t r9 = 0;
    uint64_t r10 = 0;
    uint64_t r11 = 0;
    uint64_t r12 = 0;
    uint64_t r13 = 0;
    uint64_t r14 = 0;
    uint64_t r15 = 0;
    void* rip = 0;
}; 

struct Task{
    void* EntryPoint;
    void* Stack;
    uint64_t PID;
    Registers registers;    
    bool IsInit = false;
};

struct InterruptGeneralRegisters {
	uint64_t	rax, rbx, rcx, rdx, rsi, rdi, rbp, r8, r9, r10, r11, r12, r13, r14, r15;
};

struct InterruptStack {
	struct InterruptGeneralRegisters	general;
	uint64_t							rip, cs, rflags, rsp, ss;
};

class Schedule{
    public:
        void* Main(void* stack);
        void AddTask(void* Address, size_t Size);    
        void EnabledScheduler();
    
    private:
        bool IsEnabled = false;
        uint64_t PID;
        uint64_t PIDexec;
        Task** Tasks;
};

extern Schedule schedule;

extern "C" uint64_t GetRax();
extern "C" uint64_t GetRcx();
extern "C" uint64_t GetRdx();
extern "C" uint64_t GetRbx();
extern "C" uint64_t GetRsp();
extern "C" uint64_t GetRbp();
extern "C" uint64_t GetRsi();
extern "C" uint64_t GetRdi();
extern "C" uint64_t GetR8();
extern "C" uint64_t GetR9();
extern "C" uint64_t GetR10();
extern "C" uint64_t GetR11();
extern "C" uint64_t GetR12();
extern "C" uint64_t GetR13();
extern "C" uint64_t GetR14();
extern "C" uint64_t GetR15();

extern "C" void SetRax(uint64_t value);
extern "C" void SetRcx(uint64_t value);
extern "C" void SetRdx(uint64_t value);
extern "C" void SetRbx(uint64_t value);
extern "C" void SetRsp(uint64_t value);
extern "C" void SetRbp(uint64_t value);
extern "C" void SetRsi(uint64_t value);
extern "C" void SetRdi(uint64_t value);
extern "C" void SetR8(uint64_t value);
extern "C" void SetR9(uint64_t value);
extern "C" void SetR10(uint64_t value);
extern "C" void SetR11(uint64_t value);
extern "C" void SetR12(uint64_t value);
extern "C" void SetR13(uint64_t value);
extern "C" void SetR14(uint64_t value);
extern "C" void SetR15(uint64_t value);
