#include "scheduler.h"

TaskManager globalTaskManager;

void TaskManager::Scheduler(struct InterruptStack* Registers){
    
    if(IsEnabled){          
        Tasks[CurrentTask].Regs.rip = Registers->rip;

        CurrentTask++;
        if(NumTask <= CurrentTask){
            CurrentTask = 0;
        }

        Registers->rip = Tasks[CurrentTask].Regs.rip;
    }
}

void TaskManager::AddTask(void* EntryPoint, size_t Size){ 
    Task* task = &Tasks[NumTask];   
    globalPageTableManager.MapUserspaceMemory(EntryPoint);
    task->EntryPoint = EntryPoint; 

    struct InterruptStack Regs;
    Regs.rip = EntryPoint; 
    task->Regs = Regs;
    
    task->Stack = globalAllocator.RequestPage();
    globalPageTableManager.MapUserspaceMemory(task->Stack);
    NumTask++;
}

void TaskManager::EnabledScheduler(){
    IsEnabled = true;
    EnableSystemCall(); 
    JumpIntoUserspace(Tasks[CurrentTask].EntryPoint, Tasks[CurrentTask].Stack);
}

uint64_t TaskManager::GetCurrentTask(){
    return CurrentTask;
}
