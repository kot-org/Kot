#pragma once
#include <stdint.h>
#include "../../userspace/userspace/userspace.h"
#include "../../paging/pageTableManager.h"
#include "../../memory/heap.h"
#include "../../interrupts/interrupts.h"
#include "../../lib/limits.h"
#include "../../lib/stdio.h"
#include "../../tss/tss.h"
#include "../../graphics.h"


struct Task{
    void* EntryPoint;
    void* Stack;
    struct InterruptStack Regs;   
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

extern "C" void LoadTask(), LoadKernel();


