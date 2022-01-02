#include "scheduler.h"

TaskManager* globalTaskManager;

static uint64_t mutexScheduler;

void TaskManager::Scheduler(InterruptStack* Registers, uint8_t CoreID){  
    if(IsSchedulerEnable[CoreID]){  
        Atomic::atomicSpinlock(&mutexScheduler, 1);
        Atomic::atomicLock(&mutexScheduler, 1);

        uint64_t actualTime = HPET::GetTime();
        Task* TaskEnd = NodeExecutePerCore[CoreID];
        if(TaskEnd != NULL){
            TaskEnd->TimeAllocate += actualTime - TimeByCore[CoreID];
            if(TaskEnd->Parent != NULL) TaskEnd->Parent->TimeAllocate += actualTime - TimeByCore[CoreID]; 
            memcpy(TaskEnd->Regs, Registers, sizeof(ContextStack));
            EnqueueTask(TaskEnd);
        }

        TimeByCore[CoreID] = actualTime;

        Task* TaskStart = GetTask();

        TaskStart->TaskQueueParent = NULL;

        TaskStart->CoreID = CoreID;

        NodeExecutePerCore[CoreID] = TaskStart;
        memcpy(Registers, TaskStart->Regs, sizeof(ContextStack));
    
        asm("mov %0, %%cr3" :: "r" (TaskStart->paging.PML4));

        Atomic::atomicUnlock(&mutexScheduler, 1);
    }
}

void TaskManager::EnqueueTask(Task* task){
    if(task->IsInQueue) return;
    Atomic::atomicSpinlock(&mutexScheduler, 0);
    Atomic::atomicLock(&mutexScheduler, 0);
    
    if(FirstNode == NULL) FirstNode = task;

    if(LastNode != NULL){
        LastNode->Next = task;
        task->Last = LastNode;
        task->Next = FirstNode;
    }

    LastNode = task;

    task->IsInQueue = true;
    Atomic::atomicUnlock(&mutexScheduler, 0);
}

void TaskManager::DequeueTask(Task* task){
    if(!task->IsInQueue) return;
    Atomic::atomicSpinlock(&mutexScheduler, 0);
    Atomic::atomicLock(&mutexScheduler, 0);
    if(FirstNode == task){
        if(FirstNode != task->Next){
           FirstNode = task->Next; 
        }else{
            FirstNode = NULL;
        }
    }
    if(LastNode == task){
        if(task->Last != NULL){
            LastNode = task->Last; 
        }else{
            LastNode = FirstNode;
        }
        
    }      

    if(LastNode != NULL){
        LastNode->Next = FirstNode;   
    }
   

    if(task->Last != NULL) task->Last->Next = task->Next;
    task->Next->Last = task->Last;
    task->Last = NULL;
    task->Next = NULL;


    task->IsInQueue = false;
    Atomic::atomicUnlock(&mutexScheduler, 0);
}

void TaskManager::DequeueTaskWithoutLock(Task* task){
    if(FirstNode == task){
        if(FirstNode != task->Next){
           FirstNode = task->Next; 
        }else{
            FirstNode = NULL;
        }
    }
    if(LastNode == task){
        if(task->Last != NULL){
            LastNode = task->Last; 
        }else{
            FirstNode = LastNode;
        }
        
    }      

    LastNode->Next = FirstNode;

    if(task->Last != NULL) task->Last->Next = task->Next;
    task->Next->Last = task->Last;
    task->Last = NULL;
    task->Next = NULL;

    task->IsInQueue = false;
}

Task* TaskManager::GetTask(){
    Atomic::atomicSpinlock(&mutexScheduler, 0);
    Atomic::atomicLock(&mutexScheduler, 0);

    Task* ReturnValue = FirstNode;

    DequeueTaskWithoutLock(ReturnValue);

    Atomic::atomicUnlock(&mutexScheduler, 0);    
    return ReturnValue;
}

void TaskManager::SwitchTask(InterruptStack* Registers, uint8_t CoreID, Task* task){
    if(task == NULL) return;

    Atomic::atomicSpinlock(&mutexScheduler, 1);
    Atomic::atomicLock(&mutexScheduler, 1);

    uint64_t actualTime = HPET::GetTime();
    Task* TaskEnd = NodeExecutePerCore[CoreID];
    if(TaskEnd != NULL){
        TaskEnd->TimeAllocate += actualTime - TimeByCore[CoreID];
        if(TaskEnd->Parent != NULL) TaskEnd->Parent->TimeAllocate += actualTime - TimeByCore[CoreID]; 
        memcpy(TaskEnd->Regs, Registers, sizeof(ContextStack));
        EnqueueTask(TaskEnd);
    }

    TimeByCore[CoreID] = actualTime;

    task->TaskQueueParent = NULL;

    task->CoreID = CoreID;

    NodeExecutePerCore[CoreID] = task;
    memcpy(Registers, task->Regs, sizeof(ContextStack));

    asm("mov %0, %%cr3" :: "r" (task->paging.PML4));

    Atomic::atomicUnlock(&mutexScheduler, 1);
}

Task* TaskManager::DuplicateTask(Task* parent){
    //This function setup minimal register
    Task* task = (Task*)calloc(sizeof(Task));
    task = (Task*)calloc(sizeof(Task));

    if(AllTasks == NULL){
        AllTasks = CreatNode((void*)task);
        task->NodeParent = AllTasks;
    }else{
        task->NodeParent = AllTasks->Add(task);
    }

    task->Regs = (ContextStack*)calloc(sizeof(ContextStack));

    //Copy task's paging
    task->paging = parent->paging;

    //Creat heap
    globalPageTableManager[GetCoreID()].ChangePaging(&task->paging);
    task->heap = parent->heap; // 0x400000000000 = higher half of the lower half
    uint64_t StackSize = 0x100000; // 1 mb

    task->Stack = task->heap->malloc(StackSize);

    task->Regs->rip = parent->EntryPoint;
    task->Regs->cs = parent->Regs->cs; //user code selector
    task->Regs->ss = parent->Regs->ss; //user data selector
    task->Regs->rsp = (void*)((uint64_t)task->Stack + StackSize); //because the pile goes down
    task->Regs->rflags = parent->Regs->rflags; //interrupts & syscall
    task->IsPaused = true;
    task->Priviledge = parent->Priviledge;
    task->InterruptTask = parent->InterruptTask;

    task->CreationTime = HPET::GetTime();

    task->Parent = task;
    task->TaskManagerParent = this;

    int counter = strlen(parent->Name);
    if(counter > MaxNameTask) counter = MaxNameTask;
    memcpy(task->Name, parent->Name, counter);
    task->Name[counter] = 0;
    
    task->PID = IDTask; //min of ID is 0
    IDTask++;
    NumberTaskTotal++;
    
    globalPageTableManager[GetCoreID()].RestorePaging();
    
    return task;
}

uint64_t Task::ExecuteSubTask(InterruptStack* Registers, uint8_t CoreID, DeviceTaskAdressStruct* DeviceAdress, Parameters* FunctionParameters){
    TaskManagerParent->CurrentTaskExecute++;
    //pause this task
    Pause(CoreID, Registers);

    //find task  
    DeviceTaskData* DeviceTaskDataNode = TaskManagerParent->DeviceTaskTable.GetDeviceTaskData(DeviceAdress);
    if(DeviceTaskDataNode == NULL) return 0;

    //load task
    Task* task = (Task*)malloc(sizeof(Task));
    memcpy(task, DeviceTaskDataNode->task, sizeof(Task));

    task->Parent = DeviceTaskDataNode->parent;

    task->Regs = (ContextStack*)calloc(sizeof(ContextStack));

    task->Regs->cs = task->Parent->Regs->cs;
    task->Regs->ss = task->Parent->Regs->ss;
    task->Regs->rflags = (void*)task->Parent->Regs->rflags;
    task->Regs->rip = task->EntryPoint;

    task->TaskToLaunchWhenExit = this;

    //copy parameters
    task->Regs->rdi = (void*)FunctionParameters->Parameter0;
    task->Regs->rsi = (void*)FunctionParameters->Parameter1;
    task->Regs->rdx = (void*)FunctionParameters->Parameter2;
    task->Regs->rcx = (void*)FunctionParameters->Parameter3;
    task->Regs->r8 = (void*)FunctionParameters->Parameter4;
    task->Regs->r9 = (void*)FunctionParameters->Parameter5;

    //setup stack
    uint64_t StackSize = 0x100000; // 1 mb
    if(DeviceAdress->type == 0){ //kernel
        task->Stack = malloc(StackSize);
        task->IsKernelStack = true;
        task->Regs->rsp = (void*)((uint64_t)task->Stack + StackSize);
        task->paging.PML4 = task->TaskToLaunchWhenExit->paging.PML4;
    }else{
        asm("mov %0, %%cr3" :: "r" (task->paging.PML4));
        
        task->Stack = task->heap->malloc(StackSize);
        task->Regs->rsp = (void*)((uint64_t)task->Stack + StackSize);
    }


    task->PID = PID;

    TaskManagerParent->NodeExecutePerCore[CoreID] = task;

    memcpy(Registers, (void*)task->Regs, sizeof(ContextStack));
    
    return 1;
}

void TaskManager::CreatSubTask(Task* parent, void* EntryPoint, DeviceTaskAdressStruct* DeviceAdress){
    DeviceTaskData* TaskData = (DeviceTaskData*)malloc(sizeof(DeviceTaskData));
    TaskData->parent = parent;
    TaskData->task = (Task*)malloc(sizeof(Task));
    memcpy(TaskData->task, parent, sizeof(Task));
    TaskData->task->EntryPoint = EntryPoint;
    TaskData->task->IsTaskInTask = true;
    TaskData->task->IsPaused = false;
    TaskData->task->TaskManagerParent = this;
    DeviceTaskTable.SetDeviceTaskData(DeviceAdress, TaskData);
}

DeviceTaskData* DeviceTaskTableStruct::GetDeviceTaskData(DeviceTaskAdressStruct* DeviceAdress){    
    DeviceTaskData* returnValue;
    DeviceTaskTableEntry* L1Table;
    switch (DeviceAdress->type){
        case 0: //kernel
            if(DeviceTaskTableKernel == NULL) return NULL;
            L1Table = (DeviceTaskTableEntry*)DeviceTaskTableKernel->entries[DeviceAdress->L1];
            break;
        case 1: //driver
            if(DeviceTaskTableDriver == NULL) return NULL;
            L1Table = (DeviceTaskTableEntry*)DeviceTaskTableDriver->entries[DeviceAdress->L1];
            break;
        case 2: //device
            if(DeviceTaskTableDevice == NULL) return NULL;
            L1Table = (DeviceTaskTableEntry*)DeviceTaskTableDevice->entries[DeviceAdress->L1];
            break;
        case 3: //app
            if(DeviceTaskTableApp == NULL) return NULL;
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
    DeviceTaskTableEntry* L1Table = NULL;
    switch (DeviceAdress->type){
        case 0: //kernel
            if(DeviceTaskTableKernel == NULL) DeviceTaskTableKernel = (DeviceTaskTableEntry*)calloc(sizeof(DeviceTaskTableEntry));
            if(DeviceTaskTableKernel->entries[DeviceAdress->L1] == NULL) DeviceTaskTableKernel->entries[DeviceAdress->L1] = calloc(sizeof(DeviceTaskTableEntry));
            L1Table = (DeviceTaskTableEntry*)DeviceTaskTableKernel->entries[DeviceAdress->L1];
            break;
        case 1: //driver
            if(DeviceTaskTableDriver == NULL) DeviceTaskTableDriver = (DeviceTaskTableEntry*)calloc(sizeof(DeviceTaskTableEntry));
            if(DeviceTaskTableDriver->entries[DeviceAdress->L1] == NULL) DeviceTaskTableDriver->entries[DeviceAdress->L1] = calloc(sizeof(DeviceTaskTableEntry));
            L1Table = (DeviceTaskTableEntry*)DeviceTaskTableDriver->entries[DeviceAdress->L1];
            break;
        case 2: //device
            if(DeviceTaskTableDevice == NULL) DeviceTaskTableDevice = (DeviceTaskTableEntry*)calloc(sizeof(DeviceTaskTableEntry));
            if(DeviceTaskTableDevice->entries[DeviceAdress->L1] == NULL) DeviceTaskTableDevice->entries[DeviceAdress->L1] = calloc(sizeof(DeviceTaskTableEntry));
            L1Table = (DeviceTaskTableEntry*)DeviceTaskTableDevice->entries[DeviceAdress->L1];
            break;
        case 3: //app
            if(DeviceTaskTableApp == NULL) DeviceTaskTableApp = (DeviceTaskTableEntry*)calloc(sizeof(DeviceTaskTableEntry));
            if(DeviceTaskTableApp->entries[DeviceAdress->L1] == NULL) DeviceTaskTableApp->entries[DeviceAdress->L1] = calloc(sizeof(DeviceTaskTableEntry));
            L1Table = (DeviceTaskTableEntry*)DeviceTaskTableApp->entries[DeviceAdress->L1];
            break;
    }

    if(L1Table->entries[DeviceAdress->L2]  == NULL) L1Table->entries[DeviceAdress->L2] = calloc(sizeof(DeviceTaskTableEntry));
    DeviceTaskTableEntry* L2Table = (DeviceTaskTableEntry*)L1Table->entries[DeviceAdress->L2];
    if(L2Table->entries[DeviceAdress->L3] == NULL) L2Table->entries[DeviceAdress->L3] = calloc(sizeof(DeviceTaskTableEntry));
    DeviceTaskTableEntry* L3Table = (DeviceTaskTableEntry*)L2Table->entries[DeviceAdress->L3];
    L3Table->entries[DeviceAdress->FunctionID] = (void*)deviceTaskData;
}


Task* TaskManager::AddTask(uint8_t priviledge, char* name){ 
    Task* node = (Task*)calloc(sizeof(Task));
    node = (Task*)calloc(sizeof(Task));

    if(AllTasks == NULL){
        AllTasks = CreatNode((void*)node);
        node->NodeParent = AllTasks;
    }else{
        node->NodeParent = AllTasks->Add(node);
    }

    node->Regs = (ContextStack*)calloc(sizeof(ContextStack));

    //Creat task's paging
    void* PML4 = globalAllocator.RequestPage();
    memset(globalPageTableManager[GetCoreID()].GetVirtualAddress(PML4), 0, 0x1000);
    node->paging.PageTableManagerInit((PageTable*)PML4);
    node->paging.CopyHigherHalf(&globalPageTableManager[GetCoreID()]);
    node->paging.PhysicalMemoryVirtualAddress = globalPageTableManager[GetCoreID()].PhysicalMemoryVirtualAddress;
    globalPageTableManager[GetCoreID()].ChangePaging(&node->paging);

    //Creat heap
    node->heap = UserHeap::InitializeHeap((void*)0x400000000000, 0x10, &node->paging); // 0x400000000000 = higher half of the lower half
    uint64_t StackSize = 0x100000; // 1 mb

    node->Stack = node->heap->malloc(StackSize);

    node->Regs->cs = (void*)(GDTInfoSelectorsRing[priviledge].Code | priviledge); //user code selector
    node->Regs->ss = (void*)(GDTInfoSelectorsRing[priviledge].Data | priviledge); //user data selector
    node->Regs->rsp = (void*)((uint64_t)node->Stack + StackSize); //because the pile goes down
    node->Regs->rflags = (void*)0x202; //interrupts & syscall
    node->IsPaused = true;
    node->Priviledge = priviledge;

    node->CreationTime = HPET::GetTime();

    node->Parent = node;
    node->TaskManagerParent = this;

    int counter = strlen(name);
    if(counter > MaxNameTask) counter = MaxNameTask;
    memcpy(node->Name, name, counter);
    node->Name[counter] = 0;
    
    node->PID = IDTask; //min of ID is 0
    IDTask++;
    NumberTaskTotal++;
    
    globalPageTableManager[GetCoreID()].RestorePaging();

    return node;
}

Task* TaskManager::CreatDefaultTask(){
    Task* node = AddTask(UserAppRing, "Idle Task");

    IdleNode[IddleTaskNumber] = node;
    IddleTaskNumber++;

    void* physcialMemory = globalAllocator.RequestPage();
    node->paging.MapMemory(0x0, physcialMemory);
    node->paging.MapUserspaceMemory(0x0);
    void* virtualMemory = globalPageTableManager[GetCoreID()].GetVirtualAddress(physcialMemory);
    memcpy(virtualMemory, (void*)&IdleTask, 0x1000);

    node->Launch((void*)0);
}

void TaskManager::InitScheduler(uint8_t NumberOfCores){
    for(int i = 0; i < NumberOfCores; i++){
        CreatDefaultTask();
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

        IsSchedulerEnable[CoreID] = true;
        Atomic::atomicUnlock(&mutexSchedulerEnable, 0);
        globalLogs->Successful("Scheduler is enabled for the processor : %u", CoreID);
    }
}

Task* TaskManager::GetCurrentTask(uint8_t CoreID){
    return NodeExecutePerCore[CoreID];
}

void Task::Launch(void* EntryPoint){
    this->Regs->rip = EntryPoint;
    this->EntryPoint = EntryPoint;
    this->IsPaused = false;

    TaskManagerParent->CurrentTaskExecute++;
    TaskManagerParent->EnqueueTask(this);
}

void Task::Launch(void* EntryPoint, Parameters* FunctionParameters){
    this->Regs->rip = EntryPoint;
    this->EntryPoint = EntryPoint;

    this->Regs->rdi = (void*)FunctionParameters->Parameter0;
    this->Regs->rsi = (void*)FunctionParameters->Parameter1;
    this->Regs->rdx = (void*)FunctionParameters->Parameter2;
    this->Regs->rcx = (void*)FunctionParameters->Parameter3;
    this->Regs->r8 = (void*)FunctionParameters->Parameter4;
    this->Regs->r9 = (void*)FunctionParameters->Parameter5;
    this->IsPaused = false;

    TaskManagerParent->CurrentTaskExecute++;
    TaskManagerParent->EnqueueTask(this);
}

void Task::Exit(uint8_t CoreID){
    TaskManagerParent->NodeExecutePerCore[CoreID] = NULL;
    NodeParent->Delete();
    free(Regs);
    free(this);

    asm("mov %0, %%cr3" :: "r" (globalPageTableManager[CoreID].PML4));
}

void Task::Unpause(){
    Atomic::atomicSpinlock(&mutexScheduler, 1);
    Atomic::atomicLock(&mutexScheduler, 1);
    IsPaused = false;
    TaskManagerParent->EnqueueTask(this);
    TaskManagerParent->CurrentTaskExecute++;
    Atomic::atomicUnlock(&mutexScheduler, 1);
}

void Task::Pause(uint8_t CoreID, InterruptStack* Registers){
    Atomic::atomicSpinlock(&mutexScheduler, 1);
    Atomic::atomicLock(&mutexScheduler, 1);
    uint64_t actualTime = HPET::GetTime();
    TimeAllocate += actualTime - TaskManagerParent->TimeByCore[CoreID];
    IsPaused = true;
    memcpy(Regs, Registers, sizeof(ContextStack));
    TaskManagerParent->TimeByCore[CoreID] = actualTime;
    TaskManagerParent->NodeExecutePerCore[CoreID] = NULL;
    TaskManagerParent->DequeueTask(this);
    Atomic::atomicUnlock(&mutexScheduler, 1);
}

void Task::ExitIRQ(){
    Atomic::atomicSpinlock(&mutexScheduler, 1);
    Atomic::atomicLock(&mutexScheduler, 1);
    uint64_t actualTime = HPET::GetTime();
    TimeAllocate += actualTime - TaskManagerParent->TimeByCore[CoreID];
    Regs->rip = EntryPoint;
    TaskManagerParent->TimeByCore[CoreID] = actualTime;
    TaskManagerParent->NodeExecutePerCore[CoreID] = NULL;
    TaskManagerParent->DequeueTask(this);
    Atomic::atomicUnlock(&mutexScheduler, 1);   
}

void* Task::ExitTaskInTask(InterruptStack* Registers, uint8_t CoreID, void* returnValue){
    uint64_t actualTime = HPET::GetTime();
    //get return value
    Task* node = TaskToLaunchWhenExit;

    Exit(CoreID);

    //load main task 
    node->TaskManagerParent->TimeByCore[CoreID] = actualTime;

    node->IsPaused = false;
    node->CoreID = CoreID;

    node->TaskManagerParent->NodeExecutePerCore[CoreID] = node;

    memcpy(Registers, node->Regs, sizeof(ContextStack));


    asm("mov %0, %%cr3" :: "r" (node->paging.PML4));

    return returnValue;
}

