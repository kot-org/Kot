#include "scheduler.h"

TaskManager globalTaskManager;

void TaskManager::Scheduler(struct InterruptStack* Registers){
    if(IsEnabled){   
        if(Registers->cs != (void*)0x08){
            if(Tasks[LastTask].IsInit){
                Tasks[LastTask].Registers = Registers;
            }

            if(!Tasks[CurrentTask].IsInit){
                Tasks[CurrentTask].Registers->rip = Tasks[CurrentTask].EntryPoint;
                Tasks[CurrentTask].IsInit = true;           
            }

            Registers = Tasks[CurrentTask].Registers;  

            LastTask = CurrentTask;
            CurrentTask++;
            if(NumTask <= CurrentTask){
                CurrentTask = 0;
            }
        }else{
            LoadKernel();
        }
    }
}

void TaskManager::AddTask(void* EntryPoint, size_t Size){ 
    globalPageTableManager.MapUserspaceMemory(EntryPoint);
    Tasks[NumTask].EntryPoint = EntryPoint; 
    NumTask++;
}

void TaskManager::EnabledScheduler(){
    IsEnabled = true;
    EnableSystemCall(); 
    Tasks[0].Stack = globalAllocator.RequestPage();
    globalPageTableManager.MapUserspaceMemory(Tasks[0].Stack);
    JumpIntoUserspace(Tasks[0].EntryPoint, Tasks[0].Stack);
}
