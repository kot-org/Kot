#include "scheduler.h"

TaskManager globalTaskManager;

void TaskManager::Scheduler(struct InterruptStack* Registers){
    
    if(IsEnabled){  
        if(Tasks[LastTask].IsInit){
            Tasks[LastTask].Registers->rip = Registers->rip;
        }

        if(!Tasks[CurrentTask].IsInit){
            Tasks[CurrentTask].Registers->rip = Tasks[CurrentTask].EntryPoint;
            Tasks[CurrentTask].IsInit = true;           
        }

        Registers->rip = Tasks[CurrentTask].EntryPoint;

        LastTask = CurrentTask;
        CurrentTask++;
        if(NumTask <= CurrentTask){
            CurrentTask = 0;
        }
        LoadTask();
    }
}

void TaskManager::AddTask(void* EntryPoint, size_t Size){ 
    globalPageTableManager.MapUserspaceMemory(EntryPoint);
    Tasks[NumTask].EntryPoint = EntryPoint; 
    Tasks[NumTask].Stack = globalAllocator.RequestPage();
    globalPageTableManager.MapUserspaceMemory(Tasks[NumTask].Stack);
    NumTask++;
}

void TaskManager::EnabledScheduler(){
    IsEnabled = true;
    EnableSystemCall(); 
    JumpIntoUserspace(Tasks[0].EntryPoint, Tasks[0].Stack);
}
