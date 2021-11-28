#include "scheduler.h"

TaskManager* globalTaskManager;

void TaskManager::Scheduler(InterruptStack* Registers, uint8_t CoreID){  
    if(CoreInUserSpace[CoreID]){  
        uint64_t actualTime = HPET::GetTime();
        TaskNode* node = NodeExecutePerCore[CoreID];
        if(node != NULL){
            node->Content.TimeUsed += actualTime - TimeByCore[CoreID];
            memcpy(node->Content.Regs, Registers, sizeof(ContextStack));
            node->Content.IsRunning = false;
        }

        TimeByCore[CoreID] = actualTime;
        
        MainNodeScheduler = MainNodeScheduler->Next;
        node = MainNodeScheduler;

        while(node->Content.IsRunning || node->Content.IsPaused){
            MainNodeScheduler = MainNodeScheduler->Next;
            node = MainNodeScheduler;
        }   

        node->Content.IsRunning = true;
        node->Content.CoreID = CoreID;

        NodeExecutePerCore[CoreID] = node;

        memcpy(Registers, node->Content.Regs, sizeof(ContextStack));

        asm("mov %0, %%cr3" :: "r" (node->Content.paging.PML4));
    }
}

void TaskManager::SwitchDeviceTaskNode(InterruptStack* Registers, uint8_t CoreID, GUID* APIGUID, uint64_t SpecialEntryPoint, bool IsSpecialEntryPoint){
    //pause this task
    NodeExecutePerCore[CoreID]->Content.IsRunning = false;
    NodeExecutePerCore[CoreID]->Content.IsPaused = true;

    //find task
    if(DeviceTaskNode == NULL) DeviceTaskNode = CreatNode(0);
    uint64_t sizeNode = DeviceTaskNode->GetSize();
    for(int i = 0; i < sizeNode; i++){
        DeviceTaskNodeData* DeviceTaskData = (DeviceTaskNodeData*)DeviceTaskNode->data;
        if(CompareGUID(DeviceTaskData->APIGUID, APIGUID)) break;
        if(DeviceTaskNode->next == NULL) return;
        DeviceTaskNode = DeviceTaskNode->next;
    }
    
    DeviceTaskNodeData* DeviceTaskDataNode = (DeviceTaskNodeData*)DeviceTaskNode->data;
    TaskNode* task = malloc(sizeof(TaskNode));
    memcpy(&task->Content, &DeviceTaskDataNode->task->Content, sizeof(TaskNode));

    task.IsRunning = true;
    task.TaskToLaunchWhenExit = &NodeExecutePerCore[CoreID]->Content;

    NodeExecutePerCore[CoreID] = node;

    memcpy(Registers, (void*)task.Regs, sizeof(ContextStack));

    asm("mov %0, %%cr3" :: "r" (task.paging.PML4));
}

TaskNode* TaskManager::AddTask(bool IsIddle, bool IsLinked, int ring, char* name){ 
    TaskNode* node = (TaskNode*)calloc(sizeof(TaskNode));
    node->Content.Regs = (ContextStack*)calloc(sizeof(ContextStack));

    //Creat task's paging
    void* PML4 = globalAllocator.RequestPage();
    memset(globalPageTableManager.GetVirtualAddress(PML4), 0, 0x1000);
    node->Content.paging.PageTableManagerInit((PageTable*)PML4);
    node->Content.paging.CopyHigherHalf(&globalPageTableManager);
    node->Content.paging.PhysicalMemoryVirtualAddress = globalPageTableManager.PhysicalMemoryVirtualAddress;
    globalPageTableManager.ChangePaging(&node->Content.paging);

    //Creat heap
    node->Content.heap = UserHeap::InitializeHeap((void*)0x400000000000, 0x10, &node->Content.paging); // 0x400000000000 = higher half of the lower half
    uint64_t StackSize = 0x100000; // 1 mb

    node->Content.Stack = node->Content.heap->malloc(StackSize);

    node->Content.Regs->cs = (void*)(GDTInfoSelectorsRing[ring].Code | ring); //user code selector
    node->Content.Regs->ss = (void*)(GDTInfoSelectorsRing[ring].Data | ring); //user data selector
    node->Content.Regs->rsp = (void*)((uint64_t)node->Content.Stack + StackSize); //because the pile goes down
    node->Content.Regs->rflags = (void*)0x202; //interrupts & syscall
    node->Content.IsIddle = IsIddle;
    node->Content.IsRunning = false;
    node->Content.IsPaused = true;

    node->Content.ThreadParent = NULL;
    node->Content.NodeParent = node;
    node->Content.TaskManagerParent = this;

    int counter = strlen(name);
    if(counter > MaxNameTask) counter = MaxNameTask;
    memcpy(node->Content.Name, name, counter);
    node->Content.Name[counter] = 0;
    
    node->Content.PID = IDTask; //min of ID is 0
    IDTask++;
    NumTaskTotal++;
    
    globalPageTableManager.RestorePaging();
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
    TaskNode* node = AddTask(true, IsLinked, UserAppRing, "Idle Task");
    void* physcialMemory = globalAllocator.RequestPage();
    node->Content.paging.MapMemory(0x0, physcialMemory);
    node->Content.paging.MapUserspaceMemory(0x0);
    void* virtualMemory = globalPageTableManager.GetVirtualAddress(physcialMemory);
    memcpy(virtualMemory, (void*)&IdleTask, 0x1000);
    node->Content.Launch((void*)0);
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

    if(last != NULL){
        last->Next = next;     
    }else{
        LastNode->Next = next;
    }

    if(next != NULL){    
        next->Last = last;
    }

    globalPageTableManager.ChangePaging(&node->Content.paging);
    node->Content.heap->free((void*)node->Content.Stack);
    globalPageTableManager.RestorePaging();
    free((void*)node->Content.heap);
    free((void*)node->Content.Regs);
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


void TaskContext::Launch(void* EntryPoint){
    this->Regs->rip = EntryPoint;
    this->EntryPoint = EntryPoint;
    this->IsPaused = false;
}

void TaskContext::Exit(){
    uint8_t CoreID = this->CoreID;
    TaskManagerParent->DeleteTask(NodeParent);
    TaskManagerParent->NodeExecutePerCore[CoreID] = NULL;
}