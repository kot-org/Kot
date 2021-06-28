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

        /*printf("%k %u %k", 0xffffff00, MainNodeScheduler->Content.ID, 0xffffffff);
        if(MainNodeScheduler->Next == NULL){
            MainNodeScheduler = FirstNode;
        }else{
            MainNodeScheduler = MainNodeScheduler->Next;
        }
        
        NodeExecutePerCore[CoreID] = MainNodeScheduler;


        Registers->rax = MainNodeScheduler->Content.Regs.rax;
        Registers->rcx = MainNodeScheduler->Content.Regs.rcx;
        Registers->rdx = MainNodeScheduler->Content.Regs.rdx;
        Registers->rsi = MainNodeScheduler->Content.Regs.rsi;
        Registers->rdi = MainNodeScheduler->Content.Regs.rdi;
        Registers->rbp = MainNodeScheduler->Content.Regs.rbp;
        Registers->r8 = MainNodeScheduler->Content.Regs.r8;
        Registers->r9 = MainNodeScheduler->Content.Regs.r9;
        Registers->r10 = MainNodeScheduler->Content.Regs.r10;
        Registers->r11 = MainNodeScheduler->Content.Regs.r11;
        Registers->r12 = MainNodeScheduler->Content.Regs.r12;
        Registers->r13 = MainNodeScheduler->Content.Regs.r13;
        Registers->r14 = MainNodeScheduler->Content.Regs.r14;
        Registers->r15 = MainNodeScheduler->Content.Regs.r15;
        Registers->rip = MainNodeScheduler->Content.Regs.rip;
        Registers->cs = MainNodeScheduler->Content.Regs.cs;
        Registers->rflags = MainNodeScheduler->Content.Regs.rflags;
        Registers->rsp = MainNodeScheduler->Content.Regs.rsp;
        Registers->ss = MainNodeScheduler->Content.Regs.ss;*/
    }
}

TaskNode* TaskManager::AddTask(void* EntryPoint, size_t Size, bool IsIddle){ 
    TaskNode* node = (TaskNode*)malloc(sizeof(TaskNode));
    
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
    node->Content.IsIddle = IsIddle;
    
    
    node->Content.ID = IDTask; //min of ID is 0
    IDTask++;
    NumTaskTotal++;
    //link
    node = NewNode(node);
    if(IsIddle){  
        IdleNode[IddleTaskNumber] = node;

        IddleTaskNumber++;
    }else if(IddleTaskNumber != 0){
        DeleteTask(IdleNode[IddleTaskNumber - 1]); /* because we add 1 in this function */
    }

    return node;
}

TaskNode* TaskManager::NewNode(TaskNode* node){
    if(FirstNode == NULL){        
        node->Last = NULL;  
        FirstNode = node;  
        MainNodeScheduler = node; 
    }else{
        node->Last = LastNode; 
        LastNode->Next = node;
    }

    MainNode = node;
    LastNode = node;
    return node;
}

TaskNode* TaskManager::CreatDefaultTask(){
    TaskNode* node = globalTaskManager.AddTask((void*)IdleTask, 0x1000, true);
}

void TaskManager::DeleteTask(TaskNode* node){
    if(node->Content.IsIddle){
        IddleTaskNumber--;
    }

    TaskNode* next = node->Next;
    TaskNode* last = node->Last;

    if(node == MainNode || node == LastNode || node == FirstNode || node == MainNodeScheduler){
        if(next != NULL){
            MainNode = next;
        }else{
            MainNode = last;
        }
    }

    last->Next = next;
    next->Last = last;

    NumTaskTotal--;
    free((void*)node);

    if(NumTaskTotal <= APIC::ProcessorCount){
        IdleNode[IddleTaskNumber] = NULL;
        CreatDefaultTask();
    }
}

void TaskManager::InitScheduler(uint8_t NumberOfCores){
    for(int i = 0; i < NumberOfCores; i++){
        CreatDefaultTask();
    }   

    TaskManagerInit = true;
}

void TaskManager::EnabledScheduler(uint8_t CoreID){ 
    if(TaskManagerInit){
        TaskNode* node = MainNodeScheduler;
        MainNodeScheduler = MainNodeScheduler->Next;

        EnableSystemCall(); 
        
        NodeExecutePerCore[CoreID] = node;

        JumpIntoUserspace(node->Content.EntryPoint, node->Content.Stack, node->Content.Regs.cs, node->Content.Regs.ss, CoreID); 
    }

}

TaskNode* TaskManager::GetCurrentTask(uint8_t CoreID){
    return NodeExecutePerCore[CoreID];
}
