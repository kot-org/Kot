#include "scheduler.h"

TaskManager globalTaskManager;

void TaskManager::Scheduler(InterruptStack* Registers, uint8_t CoreID){  
    if(CoreInUserSpace[CoreID]){  
        TaskNode* node = NodeExecutePerCore[CoreID];
        
        if(node != NULL){
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
            node->Content.IsRunning = false;
        }
        
        MainNodeScheduler = MainNodeScheduler->Next;
        node = MainNodeScheduler;
        while(node->Content.IsRunning){
            MainNodeScheduler = MainNodeScheduler->Next;
            node = MainNodeScheduler;
        }   
        

        node->Content.IsRunning = true;

        NodeExecutePerCore[CoreID] = node;

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

TaskNode* TaskManager::AddTask(void* EntryPoint, size_t Size, bool IsIddle, bool IsLinked, int ring){ 
    TaskNode* node = (TaskNode*)malloc(sizeof(TaskNode));
    
    uint64_t StackSize = 0x100000; // 1 mb

    node->Content.Stack = malloc(StackSize);
    for(int i = 0; i < (StackSize / 0x1000) + 1; i++){
        globalPageTableManager.MapUserspaceMemory((void*)((uint64_t)node->Content.Stack + i * 0x1000));
    } 

    for(int i = 0; i < (Size / 0x1000) + 1; i++){
        globalPageTableManager.MapUserspaceMemory((void*)((uint64_t)EntryPoint + i * 0x1000));
    } 
    

    node->Content.EntryPoint = EntryPoint; 
    node->Content.Regs.rip = EntryPoint; 
    node->Content.Regs.cs = (void*)(GDTInfoSelectorsRing[ring].Code | ring); //user code selector
    node->Content.Regs.ss = (void*)(GDTInfoSelectorsRing[ring].Data | ring); //user data selector
    node->Content.Regs.rsp = (void*)((uint64_t)node->Content.Stack + StackSize); //because the pile goes down I had not seen it ;(
    node->Content.Regs.rflags = (void*)0x202; //interrupts & syscall
    node->Content.IsIddle = IsIddle;
    
    node->Content.ID = IDTask; //min of ID is 0
    IDTask++;
    NumTaskTotal++;
    //link
    if(IsLinked){
        node = NewNode(node);
        if(IsIddle){  
            IdleNode[IddleTaskNumber] = node;

            IddleTaskNumber++;
        }else if(IddleTaskNumber != 0){
            DeleteTask(IdleNode[IddleTaskNumber - 1]); /* because we add 1 in this function */
        }        
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

    node->Next = FirstNode;
    LastNode = node;
    
    return node;
}

TaskNode* TaskManager::CreatDefaultTask(bool IsLinked){
    TaskNode* node = globalTaskManager.AddTask((void*)IdleTask, 0x1000, true, IsLinked, UserAppRing);
}

void TaskManager::DeleteTask(TaskNode* node){
    if(node->Content.IsIddle){
        IdleNode[IddleTaskNumber] = NULL;
        IddleTaskNumber--;
    }

    NumTaskTotal--;
    if(NumTaskTotal <= APIC::ProcessorCount){
        CreatDefaultTask(true);
    }

    TaskNode* next = node->Next;
    TaskNode* last = node->Last;

    if(node == LastNode){
        if(next != NULL){
            LastNode = next;
        }else{
            LastNode = last;
        }
    }
    if(node == FirstNode){
        if(last != NULL){
            FirstNode = last;
        }else{
            FirstNode = next;
        }
    }
    if(node == MainNodeScheduler){
        if(next != NULL){
            MainNodeScheduler = next;
        }else{
            MainNodeScheduler = last;
        }
    }

    last->Next = next;
    next->Last = last;

    free((void*)node->Content.Stack);
    free((void*)node);
}

void TaskManager::InitScheduler(uint8_t NumberOfCores){
    for(int i = 0; i < NumberOfCores; i++){
        CreatDefaultTask(true);
    } 

    TaskManagerInit = true;
}

static uint64_t mutexSchedulerEnable;

void TaskManager::EnabledScheduler(uint8_t CoreID){ 
    if(TaskManagerInit){
        Atomic::atomicSpinlock(&mutexSchedulerEnable, 0);
        Atomic::atomicLock(&mutexSchedulerEnable, 0); 

        NodeExecutePerCore[CoreID] = NULL;
        
        SaveTSS((uint64_t)CoreID);

        CoreInUserSpace[CoreID] = true;
        Atomic::atomicUnlock(&mutexSchedulerEnable, 0);
        globalLogs->Successful("Scheduler is enabled for the processor : %u", CoreID);
    }
}

TaskNode* TaskManager::GetCurrentTask(uint8_t CoreID){
    return NodeExecutePerCore[CoreID];
}
