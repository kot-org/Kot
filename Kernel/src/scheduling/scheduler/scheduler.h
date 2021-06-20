#pragma once
#include <stdint.h>
#include "../../userspace/userspace/userspace.h"
#include "../../paging/pageTableManager.h"
#include "../../hardware/apic/apic.h"
#include "../../memory/heap.h"
#include "../../lib/limits.h"
#include "../../lib/stdio.h"
#include "../../tss/tss.h"
#include "../../graphics.h"

typedef struct ContextStack {
    void* rax; void* rbx; void* rcx; void* rdx; void* KernelRsp; void* rsi; void* rdi; void* rbp; //push in asm

    void* r8; void* r9; void* r10; void* r11; void* r12; void* r13; void* r14; void* r15; //push in asm

    void* rip; void* cs; void* rflags; void* rsp; void* ss; //push by cpu with an interrupt
}__attribute__((packed));

struct Task{
    void* EntryPoint;
    void* Stack;
    struct ContextStack Regs;   
};

class TaskManager{
    public:
        void Scheduler(struct InterruptStack* Registers, uint8_t CoreID);
        void AddTask(void* Address, size_t Size);    
        void AddTaskTest(void* Address, size_t Size);    
        void EnabledScheduler(uint8_t CoreID, void* EntryPointIdleTask);
        uint64_t GetCurrentTask(uint8_t CoreID);
        bool IsProcessorIdle[MAX_PROCESSORS];
        bool IsEnabledPerCore[MAX_PROCESSORS];
        bool IsEnabled = false;

    private:        
        uint64_t NumTaskTotal = 0;
        uint64_t NumTaskPerCore[MAX_PROCESSORS];
        uint64_t CurrentTask[MAX_PROCESSORS];        
        Task Tasks[MAX_PROCESSORS][32];
        Task IdleTask[MAX_PROCESSORS];
        int CoreSelectToCreat;
};

extern TaskManager globalTaskManager;


