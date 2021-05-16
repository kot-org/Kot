#pragma once
#include <stdint.h>
#include "../../userspace/userspace/userspace.h"
#include "../../paging/pageTableManager.h"
#include "../../memory/heap.h"
#include "../../lib/limits.h"
#include "../../lib/stdio.h"
#include "../../tss/tss.h"
#include "../../graphics.h"


struct Task{
    void* EntryPoint;
    void* Stack;
    struct InterruptStack* Registers;   
    bool IsInit = false;
    int  TimeSchedule = 0;
};

class Schedule{
    public:
        void Scheduler(struct InterruptStack* Registers);
        void AddTask(void* Address, size_t Size);    
        void AddTaskTest(void* Address, size_t Size);    
        void EnabledScheduler();

    private:
        bool IsEnabled = false;
        uint64_t PID = 0;
        uint64_t PIDexec;
        Task** Tasks;
};

extern Schedule scheduler;

extern "C" void SwitchTask();


