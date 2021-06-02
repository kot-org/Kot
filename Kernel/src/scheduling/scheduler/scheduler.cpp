#include "scheduler.h"

TaskManager globalTaskManager;

void TaskManager::Scheduler(struct InterruptStack* Registers){
    
    if(IsEnabled){          
        Tasks[CurrentTask].Regs.rax = Registers->rax;
        Tasks[CurrentTask].Regs.rcx = Registers->rcx;
        Tasks[CurrentTask].Regs.rdx = Registers->rdx;
        Tasks[CurrentTask].Regs.rsi = Registers->rsi;
        Tasks[CurrentTask].Regs.rdi = Registers->rdi;
        Tasks[CurrentTask].Regs.rbp = Registers->rbp;
        Tasks[CurrentTask].Regs.r8 = Registers->r8;
        Tasks[CurrentTask].Regs.r9 = Registers->r9;
        Tasks[CurrentTask].Regs.r10 = Registers->r10;
        Tasks[CurrentTask].Regs.r11 = Registers->r11;
        Tasks[CurrentTask].Regs.r12 = Registers->r12;
        Tasks[CurrentTask].Regs.r13 = Registers->r13;
        Tasks[CurrentTask].Regs.r14 = Registers->r14;
        Tasks[CurrentTask].Regs.r15 = Registers->r15;
        Tasks[CurrentTask].Regs.rip = Registers->rip;
        Tasks[CurrentTask].Regs.cs = Registers->cs;
        Tasks[CurrentTask].Regs.rflags = Registers->rflags;
        Tasks[CurrentTask].Regs.rsp = Registers->rsp;
        Tasks[CurrentTask].Regs.ss = Registers->ss;

        CurrentTask++;
        if(NumTask <= CurrentTask){
            CurrentTask = 0;
        }

        Registers->rax = Tasks[CurrentTask].Regs.rax;
        Registers->rcx = Tasks[CurrentTask].Regs.rcx;
        Registers->rdx = Tasks[CurrentTask].Regs.rdx;
        Registers->rsi = Tasks[CurrentTask].Regs.rsi;
        Registers->rdi = Tasks[CurrentTask].Regs.rdi;
        Registers->rbp = Tasks[CurrentTask].Regs.rbp;
        Registers->r8 = Tasks[CurrentTask].Regs.r8;
        Registers->r9 = Tasks[CurrentTask].Regs.r9;
        Registers->r10 = Tasks[CurrentTask].Regs.r10;
        Registers->r11 = Tasks[CurrentTask].Regs.r11;
        Registers->r12 = Tasks[CurrentTask].Regs.r12;
        Registers->r13 = Tasks[CurrentTask].Regs.r13;
        Registers->r14 = Tasks[CurrentTask].Regs.r14;
        Registers->r15 = Tasks[CurrentTask].Regs.r15;
        Registers->rip = Tasks[CurrentTask].Regs.rip;
        Registers->cs = Tasks[CurrentTask].Regs.cs;
        Registers->rflags = Tasks[CurrentTask].Regs.rflags;
        Registers->rsp = Tasks[CurrentTask].Regs.rsp;
        Registers->ss = Tasks[CurrentTask].Regs.ss;
    }
}

void TaskManager::AddTask(void* EntryPoint, size_t Size){ 
    Task* task = &Tasks[NumTask];  

    for(int i = 0; i < (Size / 0x1000) + 1; i++){
        globalPageTableManager.MapUserspaceMemory((void*)((uint64_t)EntryPoint + i * 0x1000));
    }

    uint64_t StackSize = sizeof(ContextStack);
    task->Stack = malloc(StackSize);
    for(int i = 0; i < (StackSize / 0x1000) + 1; i++){
        globalPageTableManager.MapUserspaceMemory((void*)((uint64_t)task->Stack + i * 0x1000));
    }  

    task->EntryPoint = EntryPoint; 
    Tasks[NumTask].Regs.rip = EntryPoint; 
    Tasks[NumTask].Regs.cs = (void*)GDTInfoSelectors.UCode; //user code selector
    Tasks[NumTask].Regs.ss = (void*)GDTInfoSelectors.UData; //user data selector
    Tasks[NumTask].Regs.rsp = task->Stack;
    Tasks[NumTask].Regs.rflags = (void*)0x202; //interrupts & syscall
    NumTask++;
}

void TaskManager::EnabledScheduler(){
    Task* task = &Tasks[CurrentTask];
    IsEnabled = true;
    EnableSystemCall();  
    
    JumpIntoUserspace(task->EntryPoint, task->Stack, task->Regs.cs, task->Regs.ss);
}

uint64_t TaskManager::GetCurrentTask(){
    return CurrentTask;
}
