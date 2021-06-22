#include "scheduler.h"

TaskManager globalTaskManager;

void TaskManager::Scheduler(struct InterruptStack* Registers, uint8_t CoreID){    
    if(CoreInUserSpace[CoreID]){ 
        TaskNode* node = NodeExecutePerCore[CoreID];

        node->Content.Regs.rax = Registers->rax;
        node->Content.Regs.rcx = Registers->rcx;
        node->Content.Regs.rdx = Registers->rdx;
        node->Content.Regs.rsi = Registers->rsi;
        node->Content.Regs.rdi = Registers->rdi;
        node->Content.Regs.rbp = Registers->rbp;
        node->Content.Regs.r8 = Registers->r8;
        node->Content.Regs.r9 = Registers->r9;
        node->Content.Regs.r10 = Registers->r10;
        node->Content.Regs.r11 = Registers->r11;
        node->Content.Regs.r12 = Registers->r12;
        node->Content.Regs.r13 = Registers->r13;
        node->Content.Regs.r14 = Registers->r14;
        node->Content.Regs.r15 = Registers->r15;
        node->Content.Regs.rip = Registers->rip;
        node->Content.Regs.cs = Registers->cs;
        node->Content.Regs.rflags = Registers->rflags;
        node->Content.Regs.rsp = Registers->rsp;
        node->Content.Regs.ss = Registers->ss;

        MainNodeScheduler = node;
        if(MainNodeScheduler->Next == NULL){
            MainNodeScheduler = FirstNode;
        }else{
            MainNodeScheduler = MainNodeScheduler->Next;
        }

        NodeExecutePerCore[CoreID] = MainNodeScheduler;

        Registers->rax = node->Content.Regs.rax;
        Registers->rcx = node->Content.Regs.rcx;
        Registers->rdx = node->Content.Regs.rdx;
        Registers->rsi = node->Content.Regs.rsi;
        Registers->rdi = node->Content.Regs.rdi;
        Registers->rbp = node->Content.Regs.rbp;
        Registers->r8 = node->Content.Regs.r8;
        Registers->r9 = node->Content.Regs.r9;
        Registers->r10 = node->Content.Regs.r10;
        Registers->r11 = node->Content.Regs.r11;
        Registers->r12 = node->Content.Regs.r12;
        Registers->r13 = node->Content.Regs.r13;
        Registers->r14 = node->Content.Regs.r14;
        Registers->r15 = node->Content.Regs.r15;
        Registers->rip = node->Content.Regs.rip;
        Registers->cs = node->Content.Regs.cs;
        Registers->rflags = node->Content.Regs.rflags;
        Registers->rsp = node->Content.Regs.rsp;
        Registers->ss = node->Content.Regs.ss;
    }
}

TaskNode* TaskManager::AddTask(void* EntryPoint, size_t Size){ 
    TaskNode* node = (TaskNode*)malloc(sizeof(TaskNode));
    
    //content
    
    uint64_t StackSize = sizeof(ContextStack);
    node->Content.Stack = malloc(StackSize);
    for(int i = 0; i < (StackSize / 0x1000) + 1; i++){
        globalPageTableManager.MapUserspaceMemory((void*)((uint64_t)node->Content.Stack + i * 0x1000));
    }

    for(int i = 0; i < (Size / 0x1000) + 1; i++){
        globalPageTableManager.MapUserspaceMemory((void*)((uint64_t)EntryPoint + i * 0x1000));
    }

    node->Content.EntryPoint = EntryPoint; 
    node->Content.Regs.rip = EntryPoint; 
    node->Content.Regs.cs = (void*)GDTInfoSelectors.UCode; //user code selector
    node->Content.Regs.ss = (void*)GDTInfoSelectors.UData; //user data selector
    node->Content.Regs.rsp = node->Content.Stack;
    node->Content.Regs.rflags = (void*)0x202; //interrupts & syscall
    
    node->Content.ID = NumTaskTotal;
    NumTaskTotal++;
    
    if(LastNode == NULL){        
        node->Last = NULL;  
        FirstNode = node;  
        MainNodeScheduler = FirstNode; 
    }else{
        node->Last = LastNode; 
        LastNode->Next = node;
    }

    MainNode = node;
    LastNode = MainNode;
}

TaskNode* TaskManager::CreatDefaultTask(){
    extern uint64_t IdleTaskStart;
    extern uint64_t IdleTaskEnd;
    TaskNode* node = (TaskNode*)malloc(sizeof(TaskNode));

    if(NumTaskTotal == 0){
        MainNode = node;
        MainNode->Last = NULL;  
        FirstNode = MainNode;   
    }else{
        node->Last = MainNode;
    }

    MainNode->Next = node;
    
    //content
    
    size_t Size = IdleTaskEnd - IdleTaskStart;
    void* EntryPoint = malloc(Size);
    memcpy(EntryPoint, (void*)IdleTask, Size);
    
    uint64_t StackSize = sizeof(ContextStack);
    node->Content.Stack = malloc(StackSize);
    for(int i = 0; i < (StackSize / 0x1000) + 1; i++){
        globalPageTableManager.MapUserspaceMemory((void*)((uint64_t)node->Content.Stack + i * 0x1000));
    }

    for(int i = 0; i < (Size / 0x1000) + 1; i++){
        globalPageTableManager.MapUserspaceMemory((void*)((uint64_t)EntryPoint + i * 0x1000));
    }

    node->Content.EntryPoint = EntryPoint; 
    node->Content.Regs.rip = EntryPoint; 
    node->Content.Regs.cs = (void*)GDTInfoSelectors.UCode; //user code selector
    node->Content.Regs.ss = (void*)GDTInfoSelectors.UData; //user data selector
    node->Content.Regs.rsp = node->Content.Stack;
    node->Content.Regs.rflags = (void*)0x202; //interrupts & syscall
    
    NumTaskTotal++;

    TaskNode* LastMainNode = MainNode;
    MainNode = node;
    MainNode->Last = LastMainNode;
    MainNodeScheduler = FirstNode;
}

void TaskManager::DeleteTask(TaskNode* task){
    TaskNode* last = task->Last;
    TaskNode* next = task->Next;
    last->Next = next;
    next->Last = last;
    NumTaskTotal--;
}


void TaskManager::EnabledScheduler(uint8_t CoreID){  
    TaskNode* node = MainNodeScheduler;
    if(MainNodeScheduler->Next == NULL){
        MainNodeScheduler = CreatDefaultTask();
    }else{
        MainNodeScheduler = MainNodeScheduler->Next;
    }

    NodeExecutePerCore[CoreID] = node;

    globalPageTableManager.MapUserspaceMemory((void*)syscall_entry);
    EnableSystemCall(); 
     
    NodeExecutePerCore[CoreID] = node;

    JumpIntoUserspace(node->Content.EntryPoint, node->Content.Stack, node->Content.Regs.cs, node->Content.Regs.ss, CoreID);
}

TaskNode* TaskManager::GetCurrentTask(uint8_t CoreID){
    return NodeExecutePerCore[CoreID];
}
