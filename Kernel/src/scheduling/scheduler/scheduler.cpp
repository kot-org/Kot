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

uint64_t TaskManager::ExecuteSubTask(InterruptStack* Registers, uint8_t CoreID, DeviceTaskAdressStruct* DeviceAdress, Parameters* FunctionParameters){
    uint64_t actualTime = HPET::GetTime();
    TimeByCore[CoreID] = actualTime;

    //pause this task
    memcpy(NodeExecutePerCore[CoreID]->Content.Regs, Registers, sizeof(ContextStack));
    NodeExecutePerCore[CoreID]->Content.TimeUsed += actualTime - TimeByCore[CoreID];
    NodeExecutePerCore[CoreID]->Content.IsPaused = true;
    NodeExecutePerCore[CoreID]->Content.IsRunning = false;

    //find task    
    DeviceTaskData* DeviceTaskDataNode = DeviceTaskTable.GetDeviceTaskData(DeviceAdress);
    if(DeviceTaskDataNode == NULL) return 0;
    TaskNode* task = (TaskNode*)malloc(sizeof(TaskNode));
    memcpy(&task->Content, DeviceTaskDataNode->task, sizeof(TaskNode));

    task->Content.Regs = (ContextStack*)malloc(sizeof(ContextStack));
    memcpy(task->Content.Regs, DeviceTaskDataNode->task->Regs, sizeof(ContextStack));

    task->Content.Regs->rip = task->Content.EntryPoint;

    task->Content.IsRunning = true;
    task->Content.TaskToLaunchWhenExit = NodeExecutePerCore[CoreID];

    uint64_t StackSize = 0x100000; // 1 mb
    task->Content.Stack = task->Content.heap->malloc(StackSize);

    task->Content.PID = IDTask;
    task->Content.NodeParent = DeviceTaskDataNode->parent;
    IDTask++;
    NumTaskTotal++;

    NodeExecutePerCore[CoreID] = task;

    task->Content.NodeParent = NewNode(task);

    //copy parameters
    task->Content.Regs->rdi = (void*)FunctionParameters->Parameter0;
    task->Content.Regs->rsi = (void*)FunctionParameters->Parameter1;
    task->Content.Regs->rdx = (void*)FunctionParameters->Parameter2;
    task->Content.Regs->rcx = (void*)FunctionParameters->Parameter3;
    task->Content.Regs->r8 = (void*)FunctionParameters->Parameter4;
    task->Content.Regs->r9 = (void*)FunctionParameters->Parameter5;

    memcpy(Registers, (void*)task->Content.Regs, sizeof(ContextStack));
    asm("mov %0, %%cr3" :: "r" (task->Content.paging.PML4));
    return 1;
}

void TaskManager::CreatSubTask(TaskNode* parent, void* EntryPoint, DeviceTaskAdressStruct* DeviceAdress){
    DeviceTaskData* TaskData = (DeviceTaskData*)malloc(sizeof(DeviceTaskData));
    TaskData->DeviceTaskAdress = DeviceAdress;
    TaskData->parent = parent;
    TaskData->task = (TaskContext*)malloc(sizeof(TaskContext));
    memcpy(TaskData->task, &TaskData->parent->Content, sizeof(TaskContext));
    
    TaskData->task->EntryPoint = EntryPoint;
    TaskData->task->IsTaskInTask = true;

    DeviceTaskTable.SetDeviceTaskData(DeviceAdress, TaskData);
}

DeviceTaskData* DeviceTaskTableStruct::GetDeviceTaskData(DeviceTaskAdressStruct* DeviceAdress){    
    DeviceTaskData* returnValue;
    DeviceTaskTableEntry* L1Table;
    switch (DeviceAdress->type){
        case 0: //kernel
            L1Table = (DeviceTaskTableEntry*)DeviceTaskTableKernel->entries[DeviceAdress->L1];
            break;
        case 1: //driver
            L1Table = (DeviceTaskTableEntry*)DeviceTaskTableDriver->entries[DeviceAdress->L1];
            break;
        case 2: //device
            L1Table = (DeviceTaskTableEntry*)DeviceTaskTableDevice->entries[DeviceAdress->L1];
            break;
        case 3: //app
            L1Table = (DeviceTaskTableEntry*)DeviceTaskTableApp->entries[DeviceAdress->L1];
            break;
    }

    if(L1Table == NULL) return NULL;
    DeviceTaskTableEntry* L2Table = (DeviceTaskTableEntry*)L1Table->entries[DeviceAdress->L2];
    if(L2Table == NULL) return NULL;
    DeviceTaskTableEntry* L3Table = (DeviceTaskTableEntry*)L2Table->entries[DeviceAdress->L3];
    if(L3Table == NULL) return NULL;
    returnValue = (DeviceTaskData*)L3Table->entries[DeviceAdress->FunctionID];

    return returnValue;
}

void DeviceTaskTableStruct::SetDeviceTaskData(DeviceTaskAdressStruct* DeviceAdress, DeviceTaskData* deviceTaskData){    
    DeviceTaskTableEntry* L1Table;
    switch (DeviceAdress->type){
        case 0: //kernel
            if(DeviceTaskTableKernel == NULL) DeviceTaskTableKernel = (DeviceTaskTableEntry*)malloc(sizeof(DeviceTaskTableEntry));
            if(DeviceTaskTableKernel->entries[DeviceAdress->L1] == NULL) DeviceTaskTableKernel->entries[DeviceAdress->L1] = malloc(sizeof(DeviceTaskTableEntry));
            L1Table = (DeviceTaskTableEntry*)DeviceTaskTableKernel->entries[DeviceAdress->L1];
            break;
        case 1: //driver
            if(DeviceTaskTableDriver == NULL) DeviceTaskTableDriver = (DeviceTaskTableEntry*)malloc(sizeof(DeviceTaskTableEntry));
            if(DeviceTaskTableDriver->entries[DeviceAdress->L1] == NULL) DeviceTaskTableDriver->entries[DeviceAdress->L1] = malloc(sizeof(DeviceTaskTableEntry));
            L1Table = (DeviceTaskTableEntry*)DeviceTaskTableDriver->entries[DeviceAdress->L1];
            break;
        case 2: //device
            if(DeviceTaskTableDevice == NULL) DeviceTaskTableDevice = (DeviceTaskTableEntry*)malloc(sizeof(DeviceTaskTableEntry));
            if(DeviceTaskTableDevice->entries[DeviceAdress->L1] == NULL) DeviceTaskTableDevice->entries[DeviceAdress->L1] = malloc(sizeof(DeviceTaskTableEntry));
            L1Table = (DeviceTaskTableEntry*)DeviceTaskTableDevice->entries[DeviceAdress->L1];
            break;
        case 3: //app
            if(DeviceTaskTableApp == NULL) DeviceTaskTableApp = (DeviceTaskTableEntry*)malloc(sizeof(DeviceTaskTableEntry));
            if(DeviceTaskTableApp->entries[DeviceAdress->L1] == NULL) DeviceTaskTableApp->entries[DeviceAdress->L1] = malloc(sizeof(DeviceTaskTableEntry));
            L1Table = (DeviceTaskTableEntry*)DeviceTaskTableApp->entries[DeviceAdress->L1];
            break;
    }

    if(L1Table->entries[DeviceAdress->L2]  == NULL) L1Table->entries[DeviceAdress->L2] = malloc(sizeof(DeviceTaskTableEntry));
    DeviceTaskTableEntry* L2Table = (DeviceTaskTableEntry*)L1Table->entries[DeviceAdress->L2];
    if(L2Table->entries[DeviceAdress->L3] == NULL) L2Table->entries[DeviceAdress->L3] = malloc(sizeof(DeviceTaskTableEntry));
    DeviceTaskTableEntry* L3Table = (DeviceTaskTableEntry*)L2Table->entries[DeviceAdress->L3];
    L3Table->entries[DeviceAdress->FunctionID] = (void*)deviceTaskData;
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
    node->Content.Priviledge = ring;

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

void TaskContext::Launch(void* EntryPoint, Parameters* FunctionParameters){
    this->Regs->rip = EntryPoint;
    this->EntryPoint = EntryPoint;

    this->Regs->rdi = (void*)FunctionParameters->Parameter0;
    this->Regs->rsi = (void*)FunctionParameters->Parameter1;
    this->Regs->rdx = (void*)FunctionParameters->Parameter2;
    this->Regs->rcx = (void*)FunctionParameters->Parameter3;
    this->Regs->r8 = (void*)FunctionParameters->Parameter4;
    this->Regs->r9 = (void*)FunctionParameters->Parameter5;
    this->IsPaused = false;
}

void TaskContext::Exit(){
    uint8_t CoreID = this->CoreID;
    TaskManagerParent->NumTaskTotal--;
    TaskManagerParent->NodeExecutePerCore[CoreID] = NULL;
    TaskManagerParent->DeleteTask(NodeParent);
}

void* TaskContext::ExitTaskInTask(InterruptStack* Registers, uint8_t CoreID, void* returnValue){
    uint64_t actualTime = HPET::GetTime();
    //get return value
    TaskNode* node = TaskToLaunchWhenExit;

    Exit();

    //load main task 
    TaskManagerParent->TimeByCore[CoreID] = actualTime;

    node->Content.IsRunning = true;
    node->Content.IsPaused = false;
    node->Content.CoreID = CoreID;

    TaskManagerParent->NodeExecutePerCore[CoreID] = node;

    memcpy(Registers, node->Content.Regs, sizeof(ContextStack));

    asm("mov %0, %%cr3" :: "r" (node->Content.paging.PML4));

    return returnValue;
}