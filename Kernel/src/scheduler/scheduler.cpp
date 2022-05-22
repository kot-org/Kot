#include <scheduler/scheduler.h>

TaskManager* globalTaskManager;

static uint64_t mutexScheduler;

void TaskManager::Scheduler(ContextStack* Registers, uint64_t CoreID){  
    if(IsSchedulerEnable[CoreID]){  
        Atomic::atomicAcquire(&mutexScheduler, 1);

        uint64_t actualTime = HPET::GetTime();
        thread_t* ThreadEnd = ThreadExecutePerCore[CoreID];
        if(ThreadExecutePerCore[CoreID] != NULL){
            /* Save & enqueu thread */
            ThreadEnd->SaveContext(Registers, CoreID);
            EnqueueTask(ThreadEnd);
            //globalLogs->Warning("PID %x use %u% of cpu", ThreadEnd->Parent->PID, (ThreadEnd->TimeAllocate * 100 / (actualTime - ThreadEnd->CreationTime)) / NumberOfCPU);
        }

        /* Update time */
        TimeByCore[CoreID] = actualTime;

        /* Find & load new task */
        thread_t* ThreadStart = GetTread();
        ThreadStart->CreatContext(Registers, CoreID);
        
        Atomic::atomicUnlock(&mutexScheduler, 1);
    }
}

void TaskManager::EnqueueTask(thread_t* thread){
    if(thread->IsInQueue) return;
    Atomic::atomicAcquire(&mutexScheduler, 0);
    
    if(FirstNode == NULL) FirstNode = thread;

    if(LastNode != NULL){
        LastNode->Next = thread;
        thread->Last = LastNode;
    }

    LastNode = thread;
    thread->Next = FirstNode;

    thread->IsInQueue = true;
    Atomic::atomicUnlock(&mutexScheduler, 0);
}

void TaskManager::DequeueTask(thread_t* thread){
    if(!thread->IsInQueue) return;
    Atomic::atomicAcquire(&mutexScheduler, 0);

    if(FirstNode == thread){
        if(FirstNode != thread->Next){
           FirstNode = thread->Next; 
        }else{
            FirstNode = NULL;
        }
    }
    if(LastNode == thread){
        if(thread->Last != NULL){
            LastNode = thread->Last; 
        }else{
            LastNode = FirstNode;
        }
        
    }      

    if(LastNode != NULL){
        LastNode->Next = FirstNode;   
    }
   

    if(thread->Last != NULL) thread->Last->Next = thread->Next;
    thread->Next->Last = thread->Last;
    thread->Last = NULL;
    thread->Next = NULL;


    thread->IsInQueue = false;
    Atomic::atomicUnlock(&mutexScheduler, 0);
}

void TaskManager::DequeueTaskWithoutLock(thread_t* thread){
    if(FirstNode == thread){
        if(FirstNode != thread->Next){
           FirstNode = thread->Next; 
        }else{
            FirstNode = NULL;
        }
    }
    if(LastNode == thread){
        if(thread->Last != NULL){
            LastNode = thread->Last; 
        }else{
            LastNode = FirstNode;
        }
        
    }      

    if(LastNode != NULL){
        LastNode->Next = FirstNode;   
    }
   

    if(thread->Last != NULL) thread->Last->Next = thread->Next;
    thread->Next->Last = thread->Last;
    thread->Last = NULL;
    thread->Next = NULL;


    thread->IsInQueue = false;
}

thread_t* TaskManager::GetTread(){
    Atomic::atomicAcquire(&mutexScheduler, 0);

    thread_t* ReturnValue = FirstNode;

    DequeueTaskWithoutLock(ReturnValue);

    Atomic::atomicUnlock(&mutexScheduler, 0);    
    return ReturnValue;
}

uint64_t TaskManager::CreatThread(thread_t** self, process_t* proc, void* entryPoint, uint64_t externalData){
    *self = proc->CreatThread(entryPoint, externalData);
    return KSUCCESS;
}

uint64_t TaskManager::CreatThread(thread_t** self, process_t* proc, void* entryPoint, uint8_t privilege, uint64_t externalData){
    *self = proc->CreatThread(entryPoint, externalData);
    return KSUCCESS;
}

uint64_t TaskManager::DuplicateThread(thread_t** self, process_t* proc, thread_t* source, uint64_t externalData){
    if(source->Parent != proc) return KFAIL;
    *self = proc->DuplicateThread(source, externalData);
    return KSUCCESS;
}

uint64_t TaskManager::ExecThread(thread_t* self, Parameters* FunctionParameters){
    self->Launch(FunctionParameters);
    return KSUCCESS;
}

uint64_t TaskManager::Pause(ContextStack* Registers, uint64_t CoreID, thread_t* task){
    if(task->IsInQueue){
        Atomic::atomicAcquire(&mutexScheduler, 1);
        DequeueTask(task);
        Atomic::atomicUnlock(&mutexScheduler, 1);
    }else if(CoreID == task->CoreID){
        task->Pause(Registers, CoreID);
    }else{
        ThreadExecutePerCore[task->CoreID] = NULL;
        APIC::GenerateInterruption(task->CoreID, IPI_Schedule);
    }

    task->IsBlock = true;

    return KSUCCESS;
}

uint64_t TaskManager::Unpause(thread_t* task){
    task->IsBlock = false;
    EnqueueTask(task);
    return KSUCCESS;
} 

uint64_t TaskManager::Exit(ContextStack* Registers, uint64_t CoreID, thread_t* task){
    Atomic::atomicAcquire(&mutexScheduler, 1);
    
    if(task->IsInQueue){
        Atomic::atomicAcquire(&mutexScheduler, 1);
        DequeueTask(task);
        Atomic::atomicUnlock(&mutexScheduler, 1);
    }else if(CoreID == task->CoreID){
        task->Exit(Registers, CoreID);
    }else{
        ThreadExecutePerCore[task->CoreID] = NULL;
        APIC::GenerateInterruption(task->CoreID, IPI_Schedule);
    }

    /* TODO clear task data */

    Atomic::atomicUnlock(&mutexScheduler, 1);
    return KSUCCESS;
}

uint64_t TaskManager::CreatProcess(process_t** key, uint8_t priviledge, uint64_t externalData){
    process_t* proc = (process_t*)calloc(sizeof(process_t));

    if(ProcessList == NULL){
        ProcessList = CreatNode((void*)0);
        proc->NodeParent = ProcessList->Add(proc);
    }else{
        proc->NodeParent = ProcessList->Add(proc);
    }

    /* Setup default paging */
    proc->SharedPaging = vmm_SetupProcess();

    /* Setup default priviledge */
    proc->DefaultPriviledge = priviledge;

    /* Save time */
    proc->CreationTime = HPET::GetTime();

    /* Other data */
    proc->TaskManagerParent = this;
    proc->Locks = (lock_t*)LockAddress;
    proc->LockLimit = StackBottom;
    proc->LockIndex = 0;
    proc->externalData = externalData;

    proc->PID = PID; 
    PID++;
    NumberProcessTotal++;

    *key = proc;
    return KSUCCESS;
}

thread_t* process_t::CreatThread(void* entryPoint, uint64_t externalData){
    return CreatThread(entryPoint, DefaultPriviledge, externalData);
}

thread_t* process_t::CreatThread(void* entryPoint, uint8_t priviledge, uint64_t externalData){
    thread_t* thread = (thread_t*)calloc(sizeof(thread_t));
    if(Childs == NULL){
        Childs = CreatNode((void*)0);
        thread->ThreadNode = Childs->Add(thread);
    }else{
        thread->ThreadNode = Childs->Add(thread);
    }

    /* Allocate context */
    thread->Regs = (ContextStack*)calloc(sizeof(ContextStack));

    /* Copy paging */
    thread->Paging = vmm_SetupThread(this->SharedPaging);

    /* Load new stack */
    thread->SetupStack();

    /* Setup priviledge */
    thread->Priviledge = priviledge;
    thread->RingPL = GetRingPL(priviledge);

    /* Thread data */
    void* threadDataPA = Pmm_RequestPage();
    SelfData* threadData = (SelfData*)vmm_GetVirtualAddress(threadDataPA);
    
    Keyhole_Creat(&threadData->ThreadKey, this, this, DataTypeThread, (uint64_t)thread, DefaultFlagsKey);
    Keyhole_Creat(&threadData->ProcessKey, this, this, DataTypeProcess, (uint64_t)this, DefaultFlagsKey);

    vmm_Map(thread->Paging, (void*)SelfDataStartAddress, threadDataPA, thread->RingPL == UserAppRing);

    /* Setup registers */
    thread->EntryPoint = entryPoint;
    thread->Regs->rip = (uint64_t)entryPoint;
    thread->Regs->cs = (GDTInfoSelectorsRing[thread->RingPL].Code | thread->RingPL);
    thread->Regs->ss = (GDTInfoSelectorsRing[thread->RingPL].Data | thread->RingPL);
    thread->Regs->rflags.Reserved0 = true;
    thread->Regs->rflags.IF = true;
    thread->Regs->rflags.IOPL = 0;
    thread->Regs->cr3 = (uint64_t)thread->Paging; 

    /* Thread info for kernel */
    thread->Info = (threadInfo_t*)malloc(sizeof(threadInfo_t));
    thread->Info->SyscallStack = (uint64_t)malloc(KERNEL_STACK_SIZE) + KERNEL_STACK_SIZE; 
    thread->Info->CS = thread->Regs->cs;
    thread->Info->SS = thread->Regs->ss;
    thread->Info->Thread = thread;
    thread->Regs->ThreadInfo = thread->Info;

    /* Other data */
    thread->externalData = externalData;
    thread->CreationTime = HPET::GetTime();
    thread->MemoryAllocated = 0;
    thread->TimeAllocate = 0;
    thread->IsBlock = true;
    thread->Parent = this;


    /* ID */
    thread->TID = TID; 
    TID++;
    NumberOfThread++;

    return thread;
}

thread_t* process_t::DuplicateThread(thread_t* source, uint64_t externalData){
    if(source->Parent != this) return NULL;

    thread_t* thread = (thread_t*)calloc(sizeof(thread_t));
    if(Childs == NULL){
        Childs = CreatNode((void*)0);
        thread->ThreadNode = Childs->Add(thread);
    }else{
        thread->ThreadNode = Childs->Add(thread);
    }

    /* Allocate context */
    thread->Regs = (ContextStack*)calloc(sizeof(ContextStack));

    /* Copy paging */
    thread->Paging = vmm_SetupThread(source->Parent->SharedPaging);

    /* Load new stack */
    thread->SetupStack();

    /* Thread data */
    void* threadDataPA = Pmm_RequestPage();
    SelfData* threadData = (SelfData*)vmm_GetVirtualAddress(threadDataPA);
    
    Keyhole_Creat(&threadData->ThreadKey, this, this, DataTypeThread, (uint64_t)thread, FlagFullPermissions);
    Keyhole_Creat(&threadData->ProcessKey, this, this, DataTypeProcess, (uint64_t)this, FlagFullPermissions);

    vmm_Map(thread->Paging, (void*)SelfDataStartAddress, threadDataPA, source->Regs->cs == GDTInfoSelectorsRing[UserAppRing].Code);

    /* Setup registers */
    thread->Regs->rip = (uint64_t)source->EntryPoint;
    thread->Regs->cs = source->Regs->cs; 
    thread->Regs->ss = source->Regs->ss; 
    thread->Regs->rflags = source->Regs->rflags;
    thread->Regs->cr3 = source->Regs->cr3;

    /* Thread info for kernel */
    thread->Info = (threadInfo_t*)malloc(sizeof(threadInfo_t));
    thread->Info->SyscallStack = (uint64_t)malloc(KERNEL_STACK_SIZE) + KERNEL_STACK_SIZE; 
    thread->Info->CS = thread->Regs->cs;
    thread->Info->SS = thread->Regs->ss;
    thread->Info->Thread = thread;
    thread->Regs->ThreadInfo = thread->Info;


    /* Setup priviledge */
    thread->Priviledge = source->Priviledge;
    thread->RingPL = source->RingPL;

    /* Other data */
    thread->externalData = externalData;
    thread->CreationTime = HPET::GetTime();
    thread->MemoryAllocated = 0;
    thread->TimeAllocate = 0;
    thread->Parent = this;

    /* ID */
    thread->TID = TID; 
    TID++;
    NumberOfThread++;

    return thread;
}


void thread_t::SetupStack(){
    uint64_t StackLocation = StackTop;
    this->Regs->rsp = StackLocation;
    this->Stack = (StackInfo*)malloc(sizeof(StackInfo));
    this->Stack->StackStart = StackLocation;
    this->Stack->StackEndMax = StackBottom;

    /* Clear stack */
    vmm_page_table* PML4VirtualAddress = (vmm_page_table*)vmm_GetVirtualAddress((void*)Paging);
    PML4VirtualAddress->entries[0xff] = NULL;
}



void TaskManager::SwitchTask(ContextStack* Registers, uint64_t CoreID, thread_t* task){
    if(task == NULL) return;

    Atomic::atomicAcquire(&mutexScheduler, 1);

    uint64_t actualTime = HPET::GetTime();
    thread_t* TaskEnd = ThreadExecutePerCore[CoreID];
    if(ThreadExecutePerCore[CoreID] != NULL){
        //Save & enqueu task
        TaskEnd->SaveContext(Registers, CoreID);
        EnqueueTask(TaskEnd);
    }

    //Update time
    TimeByCore[CoreID] = actualTime;

    //Load new task
    task->CreatContext(Registers, CoreID);

    Atomic::atomicUnlock(&mutexScheduler, 1);
}

void TaskManager::CreatIddleTask(){
    if(IddleProc == NULL){
        CreatProcess(&IddleProc, 3, 0);
    }
    thread_t* thread = IddleProc->CreatThread(IddleTaskPointer, 0);

    IdleNode[IddleTaskNumber] = thread;
    IddleTaskNumber++;

    thread->Launch();
}
void TaskManager::InitScheduler(uint8_t NumberOfCores, void* IddleTaskFunction){
    void* physcialMemory = Pmm_RequestPage();
    void* virtualMemory = (void*)vmm_GetVirtualAddress(physcialMemory);
    vmm_Map(vmm_PageTable, virtualMemory, physcialMemory, true);
    memcpy(virtualMemory, IddleTaskFunction, PAGE_SIZE);

    IddleTaskPointer = virtualMemory; 

    for(int i = 0; i < NumberOfCores; i++){
        CreatIddleTask();
    } 

    NumberOfCPU = NumberOfCores;
    TaskManagerInit = true;
}

void TaskManager::EnabledScheduler(uint64_t CoreID){ 
    if(TaskManagerInit){
        Atomic::atomicAcquire(&mutexScheduler, 0);

        ThreadExecutePerCore[CoreID] = NULL;
        
        uint64_t Stack = (uint64_t)malloc(KERNEL_STACK_SIZE) + KERNEL_STACK_SIZE;

        TSSSetStack(CoreID, (void*)Stack);
        TSSSetIST(CoreID, IST_Scheduler, Stack);

        IsSchedulerEnable[CoreID] = true;

        CPU::SetCPUGSKernelBase((uint64_t)SelfDataStartAddress); // keys position

        CPU::SetCPUFSBase((uint64_t)SelfDataEndAddress); // Thread Local Storage

        SyscallEnable(GDTInfoSelectorsRing[KernelRing].Code, GDTInfoSelectorsRing[UserAppRing].Code);
        Atomic::atomicUnlock(&mutexScheduler, 0);
        globalLogs->Successful("Scheduler is enabled for the processor : %u", CoreID);
    }
}

thread_t* TaskManager::GetCurrentThread(uint64_t CoreID){
    return ThreadExecutePerCore[CoreID];
}

void thread_t::SaveContext(ContextStack* Registers, uint64_t CoreID){
    uint64_t actualTime = HPET::GetTime();
    TimeAllocate += actualTime - Parent->TaskManagerParent->TimeByCore[CoreID];
    memcpy(Regs, Registers, sizeof(ContextStack));
}

void thread_t::CreatContext(ContextStack* Registers, uint64_t CoreID){
    this->CoreID = CoreID;
    Parent->TaskManagerParent->ThreadExecutePerCore[CoreID] = this;
    memcpy(Registers, Regs, sizeof(ContextStack));
}

void thread_t::SetParameters(Parameters* FunctionParameters){
    Regs->arg0 = FunctionParameters->Parameter0;
    Regs->arg1 = FunctionParameters->Parameter1;
    Regs->arg2 = FunctionParameters->Parameter2;
    Regs->arg3 = FunctionParameters->Parameter3;
    Regs->arg4 = FunctionParameters->Parameter4;
    Regs->arg5 = FunctionParameters->Parameter5;
}

void thread_t::CopyStack(thread_t* source){
    vmm_page_table* PML4VirtualAddressSource = (vmm_page_table*)vmm_GetVirtualAddress((void*)source->Paging);
    vmm_page_table* PML4VirtualAddressDestination = (vmm_page_table*)vmm_GetVirtualAddress((void*)Paging);
    PML4VirtualAddressSource->entries[0xff] = PML4VirtualAddressDestination->entries[0xff];
    Stack = source->Stack;
}

bool thread_t::ExtendStack(uint64_t address){
    if(this->Stack == NULL) return false;

    address -= address % PAGE_SIZE;
    if(this->Stack->StackStart <= address) return false;
    if(address <= this->Stack->StackEndMax) return false;
    
    vmm_Map(Paging, (void*)address, Pmm_RequestPage(), this->RingPL == UserAppRing, true, true);

    return true;
}

void* thread_t::ShareDataInStack(void* data, size_t size){
    if(size == 0) return NULL;
    void* address = (void*)(Regs->rsp - size);
    if(ExtendStack((uint64_t)address)){
        pagetable_t lastPageTable = vmm_GetPageTable();
        vmm_Swap(Paging);
        // We consider that we have direct access to data but not to address

        memcpy(address, (void*)data, size);
        
        Regs->rsp -= size;
        vmm_Swap(lastPageTable);
        return address;
    }

    return NULL;
}

bool thread_t::Fork(ContextStack* Registers, uint64_t CoreID, thread_t* thread, Parameters* FunctionParameters){
    if(FunctionParameters != NULL){
        thread->SetParameters(FunctionParameters);
    }
    Fork(Registers, CoreID, thread);
}
    
bool thread_t::Fork(ContextStack* Registers, uint64_t CoreID, thread_t* thread){
    Atomic::atomicAcquire(&mutexScheduler, 1);

    thread->IsForked = true;
    thread->ForkedThread = this;

    //Save context
    SaveContext(Registers, CoreID);

    //Update time
    Parent->TaskManagerParent->TimeByCore[CoreID] = HPET::GetTime();

    //Load new task
    thread->CreatContext(Registers, CoreID);

    Atomic::atomicUnlock(&mutexScheduler, 1);

    return true;
}

bool thread_t::Launch(Parameters* FunctionParameters){
    if(FunctionParameters != NULL){
        SetParameters(FunctionParameters);
    }
    Launch();
    return true;
}

bool thread_t::Launch(){
    IsBlock = false;
    Parent->TaskManagerParent->EnqueueTask(this);
    return true;
}

bool thread_t::Pause(ContextStack* Registers, uint64_t CoreID){
    //Save context
    SaveContext(Registers, CoreID);

    //Update time
    Parent->TaskManagerParent->TimeByCore[CoreID] = HPET::GetTime();

    Parent->TaskManagerParent->ThreadExecutePerCore[CoreID] = NULL;

    IsBlock = true;

    Registers->rip =(uint64_t)Parent->TaskManagerParent->IddleTaskPointer;

    return true;
}

bool thread_t::Exit(ContextStack* Registers, uint64_t CoreID){
    if(IsForked){
        uint64_t ReturnValue = Registers->GlobalPurpose;
        Parent->TaskManagerParent->SwitchTask(Registers, CoreID, ForkedThread);
        Registers->GlobalPurpose = ReturnValue;
    }else if(IsEvent){
        Event::Exit(this);
        return true;
    }else{
        Parent->TaskManagerParent->ThreadExecutePerCore[CoreID] = NULL;
        Parent->TaskManagerParent->Scheduler(Registers, CoreID);        
    }

    ThreadNode->Delete();

    //TODO : free stack 

    free(this->Regs);
    free(this);
    return true;
}

bool thread_t::SetIOPriviledge(ContextStack* Registers, uint8_t IOPL){
    Registers->rflags.IOPL = (IOPL & 0b11);
    this->IOPL = IOPL & 0b11;
    return true;
}