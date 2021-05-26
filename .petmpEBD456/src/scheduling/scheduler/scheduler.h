#pragma once
#include <stdint.h>
#include "../../userspace/userspace/userspace.h"
#include "../../paging/pageTableManager.h"
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
    struct ContextStack Regs;   
};

class TaskManager{
    public:
        void Scheduler(struct InterruptStack* Registers);
        void AddTask(void* Address, size_t Size);    
        void AddTaskTest(void* Address, size_t Size);    
        void EnabledScheduler();
        uint64_t GetCurrentTask();

    private:
        bool IsEnabled = false;
        uint64_t NumTask = 0;
        uint64_t CurrentTask = 0;        
        Task Tasks[32];
};

extern TaskManager globalTaskManager;


