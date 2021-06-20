#include "scheduler.h"

TaskManager globalTaskManager;

void TaskManager::Scheduler(struct InterruptStack* Registers, uint8_t CoreID){    
    if(IsEnabled){  
        if(NumTaskPerCore[CoreID] == 0){ 
            //idle task
            if(IsProcessorIdle){
                IdleTask[CoreID].Regs.rax = Registers->rax;
                IdleTask[CoreID].Regs.rcx = Registers->rcx;
                IdleTask[CoreID].Regs.rdx = Registers->rdx;
                IdleTask[CoreID].Regs.rsi = Registers->rsi;
                IdleTask[CoreID].Regs.rdi = Registers->rdi;
                IdleTask[CoreID].Regs.rbp = Registers->rbp;
                IdleTask[CoreID].Regs.r8 = Registers->r8;
                IdleTask[CoreID].Regs.r9 = Registers->r9;
                IdleTask[CoreID].Regs.r10 = Registers->r10;
                IdleTask[CoreID].Regs.r11 = Registers->r11;
                IdleTask[CoreID].Regs.r12 = Registers->r12;
                IdleTask[CoreID].Regs.r13 = Registers->r13;
                IdleTask[CoreID].Regs.r14 = Registers->r14;
                IdleTask[CoreID].Regs.r15 = Registers->r15;
                IdleTask[CoreID].Regs.rip = Registers->rip;
                IdleTask[CoreID].Regs.cs = Registers->cs;
                IdleTask[CoreID].Regs.rflags = Registers->rflags;
                IdleTask[CoreID].Regs.rsp = Registers->rsp;
                IdleTask[CoreID].Regs.ss = Registers->ss;                
            }else{
                Registers->rax = IdleTask[CoreID].Regs.rax;
                Registers->rcx = IdleTask[CoreID].Regs.rcx;
                Registers->rdx = IdleTask[CoreID].Regs.rdx;
                Registers->rsi = IdleTask[CoreID].Regs.rsi;
                Registers->rdi = IdleTask[CoreID].Regs.rdi;
                Registers->rbp = IdleTask[CoreID].Regs.rbp;
                Registers->r8 = IdleTask[CoreID].Regs.r8;
                Registers->r9 = IdleTask[CoreID].Regs.r9;
                Registers->r10 = IdleTask[CoreID].Regs.r10;
                Registers->r11 = IdleTask[CoreID].Regs.r11;
                Registers->r12 = IdleTask[CoreID].Regs.r12;
                Registers->r13 = IdleTask[CoreID].Regs.r13;
                Registers->r14 = IdleTask[CoreID].Regs.r14;
                Registers->r15 = IdleTask[CoreID].Regs.r15;
                Registers->rip = IdleTask[CoreID].Regs.rip;
                Registers->cs = IdleTask[CoreID].Regs.cs;
                Registers->rflags = IdleTask[CoreID].Regs.rflags;
                Registers->rsp = IdleTask[CoreID].Regs.rsp;
                Registers->ss = IdleTask[CoreID].Regs.ss; 
                IsProcessorIdle[CoreID] = true;
            }            
            return;
        }      

        Tasks[CoreID][CurrentTask[CoreID]].Regs.rax = Registers->rax;
        Tasks[CoreID][CurrentTask[CoreID]].Regs.rcx = Registers->rcx;
        Tasks[CoreID][CurrentTask[CoreID]].Regs.rdx = Registers->rdx;
        Tasks[CoreID][CurrentTask[CoreID]].Regs.rsi = Registers->rsi;
        Tasks[CoreID][CurrentTask[CoreID]].Regs.rdi = Registers->rdi;
        Tasks[CoreID][CurrentTask[CoreID]].Regs.rbp = Registers->rbp;
        Tasks[CoreID][CurrentTask[CoreID]].Regs.r8 = Registers->r8;
        Tasks[CoreID][CurrentTask[CoreID]].Regs.r9 = Registers->r9;
        Tasks[CoreID][CurrentTask[CoreID]].Regs.r10 = Registers->r10;
        Tasks[CoreID][CurrentTask[CoreID]].Regs.r11 = Registers->r11;
        Tasks[CoreID][CurrentTask[CoreID]].Regs.r12 = Registers->r12;
        Tasks[CoreID][CurrentTask[CoreID]].Regs.r13 = Registers->r13;
        Tasks[CoreID][CurrentTask[CoreID]].Regs.r14 = Registers->r14;
        Tasks[CoreID][CurrentTask[CoreID]].Regs.r15 = Registers->r15;
        Tasks[CoreID][CurrentTask[CoreID]].Regs.rip = Registers->rip;
        Tasks[CoreID][CurrentTask[CoreID]].Regs.cs = Registers->cs;
        Tasks[CoreID][CurrentTask[CoreID]].Regs.rflags = Registers->rflags;
        Tasks[CoreID][CurrentTask[CoreID]].Regs.rsp = Registers->rsp;
        Tasks[CoreID][CurrentTask[CoreID]].Regs.ss = Registers->ss;

        CurrentTask[CoreID]++;
        if(NumTaskPerCore[CoreID] <= CurrentTask[CoreID]){
            CurrentTask[CoreID] = 0;
        }

        Registers->rax = Tasks[CoreID][CurrentTask[CoreID]].Regs.rax;
        Registers->rcx = Tasks[CoreID][CurrentTask[CoreID]].Regs.rcx;
        Registers->rdx = Tasks[CoreID][CurrentTask[CoreID]].Regs.rdx;
        Registers->rsi = Tasks[CoreID][CurrentTask[CoreID]].Regs.rsi;
        Registers->rdi = Tasks[CoreID][CurrentTask[CoreID]].Regs.rdi;
        Registers->rbp = Tasks[CoreID][CurrentTask[CoreID]].Regs.rbp;
        Registers->r8 = Tasks[CoreID][CurrentTask[CoreID]].Regs.r8;
        Registers->r9 = Tasks[CoreID][CurrentTask[CoreID]].Regs.r9;
        Registers->r10 = Tasks[CoreID][CurrentTask[CoreID]].Regs.r10;
        Registers->r11 = Tasks[CoreID][CurrentTask[CoreID]].Regs.r11;
        Registers->r12 = Tasks[CoreID][CurrentTask[CoreID]].Regs.r12;
        Registers->r13 = Tasks[CoreID][CurrentTask[CoreID]].Regs.r13;
        Registers->r14 = Tasks[CoreID][CurrentTask[CoreID]].Regs.r14;
        Registers->r15 = Tasks[CoreID][CurrentTask[CoreID]].Regs.r15;
        Registers->rip = Tasks[CoreID][CurrentTask[CoreID]].Regs.rip;
        Registers->cs = Tasks[CoreID][CurrentTask[CoreID]].Regs.cs;
        Registers->rflags = Tasks[CoreID][CurrentTask[CoreID]].Regs.rflags;
        Registers->rsp = Tasks[CoreID][CurrentTask[CoreID]].Regs.rsp;
        Registers->ss = Tasks[CoreID][CurrentTask[CoreID]].Regs.ss;
    }
}

void TaskManager::AddTask(void* EntryPoint, size_t Size){ 
    Task* task = &Tasks[CoreSelectToCreat][NumTaskPerCore[CoreSelectToCreat]];  

    for(int i = 0; i < (Size / 0x1000) + 1; i++){
        globalPageTableManager.MapUserspaceMemory((void*)((uint64_t)EntryPoint + i * 0x1000));
    }

    uint64_t StackSize = sizeof(ContextStack);
    task->Stack = malloc(StackSize);
    for(int i = 0; i < (StackSize / 0x1000) + 1; i++){
        globalPageTableManager.MapUserspaceMemory((void*)((uint64_t)task->Stack + i * 0x1000));
    }  

    task->EntryPoint = EntryPoint; 
    Tasks[CoreSelectToCreat][NumTaskPerCore[CoreSelectToCreat]].Regs.rip = EntryPoint; 
    Tasks[CoreSelectToCreat][NumTaskPerCore[CoreSelectToCreat]].Regs.cs = (void*)GDTInfoSelectors.UCode; //user code selector
    Tasks[CoreSelectToCreat][NumTaskPerCore[CoreSelectToCreat]].Regs.ss = (void*)GDTInfoSelectors.UData; //user data selector
    Tasks[CoreSelectToCreat][NumTaskPerCore[CoreSelectToCreat]].Regs.rsp = task->Stack;
    Tasks[CoreSelectToCreat][NumTaskPerCore[CoreSelectToCreat]].Regs.rflags = (void*)0x202; //interrupts & syscall
    
    IsProcessorIdle[CoreSelectToCreat] = false;
    NumTaskPerCore[CoreSelectToCreat]++;
    NumTaskTotal++;
    CoreSelectToCreat++;
    if(APIC::ProcessorCount <= CoreSelectToCreat){
        CoreSelectToCreat = 0;
    }
}

void TaskManager::EnabledScheduler(uint8_t CoreID, void* EntryPointIdleTask){
    //idle task
    Task* task = &IdleTask[CoreID];
    globalPageTableManager.MapUserspaceMemory(EntryPointIdleTask);

    uint64_t StackSize = sizeof(ContextStack);
    task->Stack = malloc(StackSize);
    for(int i = 0; i < (StackSize / 0x1000) + 1; i++){
        globalPageTableManager.MapUserspaceMemory((void*)((uint64_t)task->Stack + i * 0x1000));
    }  

    task->EntryPoint = EntryPointIdleTask; 
    IdleTask[CoreSelectToCreat].Regs.rip = EntryPointIdleTask; 
    IdleTask[CoreSelectToCreat].Regs.cs = (void*)GDTInfoSelectors.UCode; //user code selector
    IdleTask[CoreSelectToCreat].Regs.ss = (void*)GDTInfoSelectors.UData; //user data selector
    IdleTask[CoreSelectToCreat].Regs.rsp = task->Stack;
    IdleTask[CoreSelectToCreat].Regs.rflags = (void*)0x202; //interrupts & syscall

    IsProcessorIdle[CoreID] = true;

    EnableSystemCall();  

    JumpIntoUserspace(task->EntryPoint, task->Stack, IdleTask[CoreSelectToCreat].Regs.cs, IdleTask[CoreSelectToCreat].Regs.ss, CoreID);
}

uint64_t TaskManager::GetCurrentTask(uint8_t CoreID){
    return CurrentTask[CoreID];
}
