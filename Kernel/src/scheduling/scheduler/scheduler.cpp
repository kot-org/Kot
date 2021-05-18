#include "scheduler.h"

TaskManager globalTaskManager;

void TaskManager::Scheduler(struct InterruptStack* Registers){
    void* rip = 0;
    if(IsEnabled){   
        if(Tasks[CurrentTask].IsInit){
            Tasks[CurrentTask].Registers = Registers;
        }

        CurrentTask++;
        if(NumTask <= CurrentTask){
            CurrentTask = 0;
        }
;
        if(!Tasks[CurrentTask].IsInit){
            Tasks[CurrentTask].Registers = Registers;
            Tasks[CurrentTask].Registers->rip = Tasks[CurrentTask].EntryPoint;
            Tasks[CurrentTask].TimeSchedule++;
            Tasks[CurrentTask].IsInit = true;             
        }

        Registers = Tasks[CurrentTask].Registers;  
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

    void* Stack = globalAllocator.RequestPage();
    globalPageTableManager.MapUserspaceMemory(Stack);
    JumpIntoUserspace(Tasks[0].EntryPoint, Stack);
}
