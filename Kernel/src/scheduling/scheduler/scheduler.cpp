#include "scheduler.h"

TaskManager globalTaskManager;

void TaskManager::Scheduler(struct InterruptStack* Registers, uint8_t CoreID){    
    if(IsEnabled){ 
        TaskNode* node = NodeExecutePerCore[CoreID];
        node->context.Regs.rax = Registers->rax;
        node->context.Regs.rcx = Registers->rcx;
        node->context.Regs.rdx = Registers->rdx;
        node->context.Regs.rsi = Registers->rsi;
        node->context.Regs.rdi = Registers->rdi;
        node->context.Regs.rbp = Registers->rbp;
        node->context.Regs.r8 = Registers->r8;
        node->context.Regs.r9 = Registers->r9;
        node->context.Regs.r10 = Registers->r10;
        node->context.Regs.r11 = Registers->r11;
        node->context.Regs.r12 = Registers->r12;
        node->context.Regs.r13 = Registers->r13;
        node->context.Regs.r14 = Registers->r14;
        node->context.Regs.r15 = Registers->r15;
        node->context.Regs.rip = Registers->rip;
        node->context.Regs.cs = Registers->cs;
        node->context.Regs.rflags = Registers->rflags;
        node->context.Regs.rsp = Registers->rsp;
        node->context.Regs.ss = Registers->ss;

        if(node->next == NULL){
            node = FirstNode;
        }else{
            node = node->next;
        }

        NodeExecutePerCore[CoreID] = node;

        Registers->rax = node->context.Regs.rax;
        Registers->rcx = node->context.Regs.rcx;
        Registers->rdx = node->context.Regs.rdx;
        Registers->rsi = node->context.Regs.rsi;
        Registers->rdi = node->context.Regs.rdi;
        Registers->rbp = node->context.Regs.rbp;
        Registers->r8 = node->context.Regs.r8;
        Registers->r9 = node->context.Regs.r9;
        Registers->r10 = node->context.Regs.r10;
        Registers->r11 = node->context.Regs.r11;
        Registers->r12 = node->context.Regs.r12;
        Registers->r13 = node->context.Regs.r13;
        Registers->r14 = node->context.Regs.r14;
        Registers->r15 = node->context.Regs.r15;
        Registers->rip = node->context.Regs.rip;
        Registers->cs = node->context.Regs.cs;
        Registers->rflags = node->context.Regs.rflags;
        Registers->rsp = node->context.Regs.rsp;
        Registers->ss = node->context.Regs.ss;
    }
}

TaskNode* TaskManager::AddTask(void* EntryPoint, size_t Size){ 
    TaskNode* node = (TaskNode*)malloc(sizeof(TaskNode));

    if(NumTaskTotal == 0){
        MainNode = node;
        MainNode->previous = NULL;  
        FirstNode = MainNode;   
    }
    
    //content
    
    uint64_t StackSize = sizeof(ContextStack);
    node->context.Stack = malloc(StackSize);
    for(int i = 0; i < (StackSize / 0x1000) + 1; i++){
        globalPageTableManager.MapUserspaceMemory((void*)((uint64_t)node->context.Stack + i * 0x1000));
    }

    for(int i = 0; i < (Size / 0x1000) + 1; i++){
        globalPageTableManager.MapUserspaceMemory((void*)((uint64_t)EntryPoint + i * 0x1000));
    }

    globalPageTableManager.MapUserspaceMemory(node->context.Stack);
    node->context.EntryPoint = EntryPoint; 
    node->context.Regs.rip = EntryPoint; 
    node->context.Regs.cs = (void*)GDTInfoSelectors.UCode; //user code selector
    node->context.Regs.ss = (void*)GDTInfoSelectors.UData; //user data selector
    node->context.Regs.rsp = node->context.Stack;
    node->context.Regs.rflags = (void*)0x202; //interrupts & syscall
    
    NumTaskTotal++;

    TaskNode* LastMainNode = MainNode;
    MainNode = node;
    MainNode->previous = LastMainNode;
    LastMainNode->next = MainNode;
}

TaskNode* TaskManager::CreatDefaultTask(){
    extern uint64_t IdleTaskStart;
    extern uint64_t IdleTaskEnd;
    TaskNode* node = (TaskNode*)malloc(sizeof(TaskNode));

    if(NumTaskTotal == 0){
        MainNode = node;
        MainNode->previous = NULL;  
        FirstNode = MainNode;   
    }else{
        node->previous = MainNode;
    }

    MainNode->next = node;
    
    //content
    
    size_t Size = IdleTaskEnd - IdleTaskStart;
    void* EntryPoint = malloc(Size);
    memcpy(EntryPoint, (void*)IdleTask, Size);
    
    uint64_t StackSize = sizeof(ContextStack);
    node->context.Stack = malloc(StackSize);
    for(int i = 0; i < (StackSize / 0x1000) + 1; i++){
        globalPageTableManager.MapUserspaceMemory((void*)((uint64_t)node->context.Stack + i * 0x1000));
    }

    for(int i = 0; i < (Size / 0x1000) + 1; i++){
        globalPageTableManager.MapUserspaceMemory((void*)((uint64_t)EntryPoint + i * 0x1000));
    }

    globalPageTableManager.MapUserspaceMemory(node->context.Stack);
    node->context.EntryPoint = EntryPoint; 
    node->context.Regs.rip = EntryPoint; 
    node->context.Regs.cs = (void*)GDTInfoSelectors.UCode; //user code selector
    node->context.Regs.ss = (void*)GDTInfoSelectors.UData; //user data selector
    node->context.Regs.rsp = node->context.Stack;
    node->context.Regs.rflags = (void*)0x202; //interrupts & syscall
    
    NumTaskTotal++;

    TaskNode* LastMainNode = MainNode;
    MainNode = node;
    MainNode->previous = LastMainNode;
    MainNodeScheduler = FirstNode;
}

void TaskManager::EnabledScheduler(uint8_t CoreID){  
    if(MainNodeScheduler->next == NULL){
        MainNodeScheduler = CreatDefaultTask();
    }else{
        MainNodeScheduler = MainNodeScheduler->next;
    }

    TaskNode* node = MainNodeScheduler;

    NodeExecutePerCore[CoreID] = node;

    EnableSystemCall();  
    
    JumpIntoUserspace(node->context.EntryPoint, node->context.Stack, node->context.Regs.cs, node->context.Regs.ss, CoreID);
}

TaskNode* TaskManager::GetCurrentTask(uint8_t CoreID){
    return NodeExecutePerCore[CoreID];
}
