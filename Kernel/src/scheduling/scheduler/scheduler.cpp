#include "scheduler.h"

Schedule scheduler;

void Schedule::Scheduler(struct InterruptStack* Registers){
    void* rip = 0;
    if(IsEnabled){
        Task* LatsTask = Tasks[PIDexec];
        LatsTask->Registers = Registers;  
        
        PIDexec++;
        if(PIDexec >= PID){
            PIDexec = 0;
        }

        Task* task = Tasks[PIDexec];
        if(!task->IsInit){
            //task->Registers = NULL;
            task->Registers->rip = (uint64_t)task->EntryPoint;
            task->TimeSchedule++;
        }
        Registers = task->Registers;
    }
}

void Schedule::AddTask(void* EntryPoint, size_t Size){ 
    //realloc task table
    Task** NewTable = new Task*[PID + 1];
    memcpy(NewTable, Tasks, sizeof(Tasks));
    //free(NewTable);
    globalPageTableManager.MapUserspaceMemory(EntryPoint);
    Tasks[PID]->EntryPoint = EntryPoint; 
    Tasks[PID]->Stack = globalAllocator.RequestPage();   
    PID++;
}

void Schedule::EnabledScheduler(){
    IsEnabled = true;
    EnableSystemCall(); 
    JumpIntoUserspace(Tasks[0]->EntryPoint, Tasks[0]->Stack);
}
