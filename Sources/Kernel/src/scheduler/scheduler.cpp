#include <scheduler/scheduler.h>

TaskManager* globalTaskManager;

void TaskManager::Scheduler(ContextStack* Registers, uint64_t CoreID){  
    if(Atomic::atomicLock(&MutexScheduler, 0)){
        if(IsSchedulerEnable[CoreID]){  
            uint64_t actualTime = HPET::GetTime();
            thread_t* ThreadEnd = ThreadExecutePerCore[CoreID];
            if(ThreadExecutePerCore[CoreID] != NULL){
                /* Save & enqueu thread */
                ThreadEnd->SaveContext(Registers, CoreID);
                EnqueueTaskWithoutLock(ThreadEnd);
                //Warning("PID %x use %u% of cpu", ThreadEnd->Parent->PID, (ThreadEnd->TimeAllocate * 100 / (actualTime - ThreadEnd->CreationTime)) / NumberOfCPU);
            }

            /* Update time */
            TimeByCore[CoreID] = actualTime;

            /* Find & load new task */
            thread_t* ThreadStart = GetTreadWithoutLock();
            ThreadStart->CreateContext(Registers, CoreID);
        } 
        Atomic::atomicUnlock(&MutexScheduler, 0);       
    }

}

void TaskManager::EnqueueTask(thread_t* thread){
    Atomic::atomicAcquire(&MutexScheduler, 0);
    EnqueueTaskWithoutLock(thread);
    Atomic::atomicUnlock(&MutexScheduler, 0);
}

void TaskManager::EnqueueTaskWithoutLock(thread_t* thread){
    if(thread->IsInQueue) return;
    
    if(FirstNode == NULL){
        FirstNode = thread;
    }

    if(LastNode != NULL){
        LastNode->Next = thread;
        thread->Last = LastNode;
    }

    LastNode = thread;
    thread->Next = FirstNode;

    thread->IsInQueue = true;
}

void TaskManager::DequeueTask(thread_t* thread){
    Atomic::atomicAcquire(&MutexScheduler, 0);
    DequeueTaskWithoutLock(thread);
    Atomic::atomicUnlock(&MutexScheduler, 0);
}

void TaskManager::DequeueTaskWithoutLock(thread_t* thread){
    if(!thread->IsInQueue) return;
    if(FirstNode == thread){
        if(FirstNode != thread->Next){
           FirstNode = thread->Next; 
        }else{
            FirstNode = NULL;
            LastNode = NULL;
            thread->IsInQueue = false;
            return;
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

thread_t* TaskManager::GetTreadWithoutLock(){
    thread_t* ReturnValue = FirstNode;
    DequeueTaskWithoutLock(ReturnValue);
    return ReturnValue;
}

uint64_t TaskManager::CreateThread(thread_t** self, process_t* proc, uintptr_t entryPoint, uint64_t externalData){
    *self = proc->CreateThread(entryPoint, externalData);
    return KSUCCESS;
}

uint64_t TaskManager::CreateThread(thread_t** self, process_t* proc, uintptr_t entryPoint, uint8_t privilege, uint64_t externalData){
    *self = proc->CreateThread(entryPoint, externalData);
    return KSUCCESS;
}

uint64_t TaskManager::DuplicateThread(thread_t** self, process_t* proc, thread_t* source, uint64_t externalData){
    if(source->Parent != proc) return KFAIL;
    *self = proc->DuplicateThread(source, externalData);
    return KSUCCESS;
}

uint64_t TaskManager::ExecThread(thread_t* self, parameters_t* FunctionParameters){
    self->Launch(FunctionParameters);
    return KSUCCESS;
}

uint64_t TaskManager::Pause(ContextStack* Registers, uint64_t CoreID, thread_t* task){
    if(task->IsInQueue){
        DequeueTask(task);
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
    Atomic::atomicAcquire(&MutexScheduler, 0);

    if(task->IsInQueue){
        DequeueTask(task);
    }else{
        ThreadExecutePerCore[task->CoreID] = NULL;
        if(task->CoreID == CoreID){
            Registers->rip = (uint64_t)IddleTaskPointer;
        }
    }
    Atomic::atomicUnlock(&MutexScheduler, 0);

    task->Exit(Registers, CoreID);


    return KSUCCESS;
}
uint64_t TaskManager::ShareDataUsingStackSpace(thread_t* self, uintptr_t data, size_t size, uint64_t* location){
    return self->ShareDataUsingStackSpace(data, size, location);
}

uint64_t TaskManager::CreateProcess(process_t** key, uint8_t priviledge, uint64_t externalData){
    Atomic::atomicLock(&MutexScheduler, 1);
    process_t* proc = (process_t*)calloc(sizeof(process_t));

    if(ProcessList == NULL){
        ProcessList = CreateNode((uintptr_t)0);
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
    Atomic::atomicUnlock(&MutexScheduler, 1);
    return KSUCCESS;
}

thread_t* process_t::CreateThread(uintptr_t entryPoint, uint64_t externalData){
    return CreateThread(entryPoint, DefaultPriviledge, externalData);
}

thread_t* process_t::CreateThread(uintptr_t entryPoint, uint8_t priviledge, uint64_t externalData){
    Atomic::atomicLock(&globalTaskManager->MutexScheduler, 1);
    thread_t* thread = (thread_t*)calloc(sizeof(thread_t));
    if(Childs == NULL){
        Childs = CreateNode((uintptr_t)0);
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

    /* Thread data */
    uintptr_t threadDataPA = Pmm_RequestPage();
    thread->threadData = (SelfData*)vmm_GetVirtualAddress(threadDataPA);
    
    Keyhole_Create(&thread->threadData->ThreadKey, this, this, DataTypeThread, (uint64_t)thread, DefaultFlagsKey);
    Keyhole_Create(&thread->threadData->ProcessKey, this, this, DataTypeProcess, (uint64_t)this, DefaultFlagsKey);

    vmm_Map(thread->Paging, (uintptr_t)SelfDataStartAddress, threadDataPA, thread->RingPL == UserAppRing);

    thread->EntryPoint = entryPoint;

    /* Setup registers */
    SetupRegistersForTask(thread);

    /* Setup SIMD */
    thread->SIMDSaver = simdCreateSaveSpace();

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

    Atomic::atomicUnlock(&globalTaskManager->MutexScheduler, 1);
    return thread;
}

thread_t* process_t::DuplicateThread(thread_t* source, uint64_t externalData){
    Atomic::atomicLock(&globalTaskManager->MutexScheduler, 1);
    thread_t* thread = (thread_t*)calloc(sizeof(thread_t));
    if(Childs == NULL){
        Childs = CreateNode((uintptr_t)0);
        thread->ThreadNode = Childs->Add(thread);
    }else{
        thread->ThreadNode = Childs->Add(thread);
    }

    /* Allocate context */
    thread->Regs = (ContextStack*)calloc(sizeof(ContextStack));

    /* Copy paging */
    thread->Paging = vmm_SetupThread(SharedPaging);

    /* Load new stack */
    thread->SetupStack();

    /* Thread data */
    uintptr_t threadDataPA = Pmm_RequestPage();
    thread->threadData = (SelfData*)vmm_GetVirtualAddress(threadDataPA);
    
    Keyhole_Create(&thread->threadData->ThreadKey, this, this, DataTypeThread, (uint64_t)thread, KeyholeFlagFullPermissions);
    Keyhole_Create(&thread->threadData->ProcessKey, this, this, DataTypeProcess, (uint64_t)this, KeyholeFlagFullPermissions);

    vmm_Map(thread->Paging, (uintptr_t)SelfDataStartAddress, threadDataPA, source->Regs->cs == GDTInfoSelectorsRing[UserAppRing].Code);


    /* Setup SIMD */
    thread->SIMDSaver = simdCreateSaveSpace();

    /* Thread info for kernel */
    thread->Info = (threadInfo_t*)malloc(sizeof(threadInfo_t));
    thread->Info->SyscallStack = (uint64_t)malloc(KERNEL_STACK_SIZE) + KERNEL_STACK_SIZE; 
    thread->Info->CS = source->Regs->cs;
    thread->Info->SS = source->Regs->ss;
    thread->Info->Thread = thread;
    thread->EntryPoint = source->EntryPoint;

    /* Setup priviledge */
    thread->Priviledge = source->Priviledge;
    thread->RingPL = source->RingPL;

    /* Setup registers */
    SetupRegistersForTask(thread);
    thread->Regs->ThreadInfo = thread->Info;

    /* Other data */
    thread->externalData = externalData;
    thread->CreationTime = HPET::GetTime();
    thread->MemoryAllocated = 0;
    thread->TimeAllocate = 0;
    thread->Parent = this;
    thread->IsCIP = source->IsCIP;
    thread->TCIP = source->TCIP;

    /* ID */
    thread->TID = TID; 
    TID++;
    NumberOfThread++;

    Atomic::atomicUnlock(&globalTaskManager->MutexScheduler, 1);
    return thread;
}


void thread_t::SetupStack(){
    uint64_t StackLocation = StackTop;
    this->Regs->rsp = StackLocation;
    this->Stack = (StackInfo*)malloc(sizeof(StackInfo));
    this->Stack->StackStart = StackLocation;
    this->Stack->StackEndMax = StackBottom;

    /* Clear stack */
    vmm_page_table* PML4VirtualAddress = (vmm_page_table*)vmm_GetVirtualAddress((uintptr_t)Paging);
    PML4VirtualAddress->entries[0xff] = NULL;
}



void TaskManager::SwitchTask(ContextStack* Registers, uint64_t CoreID, thread_t* task){
    if(task == NULL) return;

    Atomic::atomicAcquire(&MutexScheduler, 0);

    //Update time
    TimeByCore[CoreID] = HPET::GetTime();

    //Load new task
    task->CreateContext(Registers, CoreID);

    Atomic::atomicUnlock(&MutexScheduler, 0);
}

void TaskManager::CreateIddleTask(){
    if(IddleProc == NULL){
        CreateProcess(&IddleProc, 3, 0);
    }
    thread_t* thread = IddleProc->CreateThread(IddleTaskPointer, 0);

    IdleNode[IddleTaskNumber] = thread;
    IddleTaskNumber++;
    
    thread->Launch();
}
void TaskManager::InitScheduler(uint8_t NumberOfCores, uintptr_t IddleTaskFunction){
    uintptr_t physcialMemory = Pmm_RequestPage();
    uintptr_t virtualMemory = (uintptr_t)vmm_GetVirtualAddress(physcialMemory);
    vmm_Map(vmm_PageTable, virtualMemory, physcialMemory, true);
    memcpy(virtualMemory, IddleTaskFunction, PAGE_SIZE);

    IddleTaskPointer = virtualMemory; 

    for(int i = 0; i < NumberOfCores; i++){
        CreateIddleTask();
    } 

    NumberOfCPU = NumberOfCores;
    TaskManagerInit = true;
    globalAddressForStackSpaceSharing = malloc(ShareMaxIntoStackSpace);
}

void TaskManager::EnabledScheduler(uint64_t CoreID){ 
    if(TaskManagerInit){
        Atomic::atomicAcquire(&MutexScheduler, 0);

        ThreadExecutePerCore[CoreID] = NULL;
        
        uint64_t Stack = (uint64_t)malloc(KERNEL_STACK_SIZE) + KERNEL_STACK_SIZE;

        TSSSetStack(CoreID, (uintptr_t)Stack);
        TSSSetIST(CoreID, IST_Scheduler, Stack);

        CPU::SetCPUGSKernelBase((uint64_t)SelfDataStartAddress); // keys position

        CPU::SetCPUFSBase((uint64_t)SelfDataEndAddress); // Thread Local Storage

        SyscallEnable(GDTInfoSelectorsRing[KernelRing].Code, GDTInfoSelectorsRing[UserAppRing].Code); 

        IsSchedulerEnable[CoreID] = true;
        Atomic::atomicUnlock(&MutexScheduler, 0);
        Successful("Scheduler is enabled for the processor : %u", CoreID);
    }
}

thread_t* TaskManager::GetCurrentThread(uint64_t CoreID){
    return ThreadExecutePerCore[CoreID];
}

void thread_t::SaveContext(ContextStack* Registers, uint64_t CoreID){
    uint64_t actualTime = HPET::GetTime();
    TimeAllocate += actualTime - Parent->TaskManagerParent->TimeByCore[CoreID];
    SaveContext(Registers);
}

void thread_t::SaveContext(ContextStack* Registers){
    simdSave(SIMDSaver);
    memcpy(Regs, Registers, sizeof(ContextStack));
}

void thread_t::CreateContext(ContextStack* Registers, uint64_t CoreID){
    this->CoreID = CoreID;
    Parent->TaskManagerParent->ThreadExecutePerCore[CoreID] = this;
    simdSave(SIMDSaver);
    memcpy(Registers, Regs, sizeof(ContextStack));
}

void thread_t::SetParameters(parameters_t* FunctionParameters){
    Regs->arg0 = FunctionParameters->Parameter0;
    Regs->arg1 = FunctionParameters->Parameter1;
    Regs->arg2 = FunctionParameters->Parameter2;
    Regs->arg3 = FunctionParameters->Parameter3;
    Regs->arg4 = FunctionParameters->Parameter4;
    Regs->arg5 = FunctionParameters->Parameter5;
}

void thread_t::CopyStack(thread_t* source){
    vmm_page_table* PML4VirtualAddressSource = (vmm_page_table*)vmm_GetVirtualAddress((uintptr_t)source->Paging);
    vmm_page_table* PML4VirtualAddressDestination = (vmm_page_table*)vmm_GetVirtualAddress((uintptr_t)Paging);
    PML4VirtualAddressSource->entries[0xff] = PML4VirtualAddressDestination->entries[0xff];
    Stack = source->Stack;
}

bool thread_t::ExtendStack(uint64_t address){
    if(this->Stack == NULL) return false;

    address -= address % PAGE_SIZE;
    if(this->Stack->StackStart <= address) return false;
    if(address <= this->Stack->StackEndMax) return false;
    
    if(!vmm_GetFlags(Paging, (uintptr_t)address, vmm_PhysicalStorage)){
        vmm_Map(Paging, (uintptr_t)address, Pmm_RequestPage(), true, true, true);
        return true;
    }

    return false;
}

bool thread_t::ExtendStack(uint64_t address, size_t size){
    if(this->Stack == NULL) return false;

    size += address % PAGE_SIZE;
    address -= address % PAGE_SIZE;
    if(this->Stack->StackStart <= address) return false;
    if(address <= this->Stack->StackEndMax) return false;
    
    uint64_t pageCount = DivideRoundUp(size, PAGE_SIZE);
    
    for(uint64_t i = 0; i < pageCount; i++){
        if(!vmm_GetFlags(Paging, (uintptr_t)(address + i * PAGE_SIZE), vmm_PhysicalStorage) || !vmm_GetFlags(Paging, (uintptr_t)(address + i * PAGE_SIZE), vmm_Present)){
            vmm_Map(Paging, (uintptr_t)(address + i * PAGE_SIZE), Pmm_RequestPage(), true, true, true);
        }        
    }

    return true;
}

KResult thread_t::ShareDataUsingStackSpace(uintptr_t data, size_t size, uint64_t* location){
    *location = 0;
    if(!IsBlock){
        *location = NULL;
        return KFAIL;
    }
    if(size == 0){
        *location = NULL;
        return KFAIL;
    } 
    if(size > ShareMaxIntoStackSpace){
        *location = NULL;
        return KFAIL;
    } 
    uintptr_t address = (uintptr_t)(Regs->rsp - size);
    if(ExtendStack((uint64_t)address, size)){
        //store data in global memory
        Atomic::atomicAcquire(&globalTaskManager->lockglobalAddressForStackSpaceSharing, 0);
        memcpy(Parent->TaskManagerParent->globalAddressForStackSpaceSharing, (uintptr_t)data, size);
        
        pagetable_t lastPageTable = vmm_GetPageTable();
        vmm_Swap(Paging);
        // We consider that we have direct access to data but not to address

        memcpy(address, Parent->TaskManagerParent->globalAddressForStackSpaceSharing, size);
        Atomic::atomicUnlock(&globalTaskManager->lockglobalAddressForStackSpaceSharing, 0);
        Regs->rsp -= size;
        vmm_Swap(lastPageTable);
        *location = (uint64_t)address;
    }

    return KFAIL;
}

bool thread_t::CIP(ContextStack* Registers, uint64_t CoreID, thread_t* thread, parameters_t* FunctionParameters){
    if(FunctionParameters != NULL){
        thread->SetParameters(FunctionParameters);
    }
    CIP(Registers, CoreID, thread);
}
    
bool thread_t::CIP(ContextStack* Registers, uint64_t CoreID, thread_t* thread){
    Atomic::atomicAcquire(&globalTaskManager->MutexScheduler, 0);

    thread_t* child = thread->Parent->DuplicateThread(thread, this->externalData);
    child->IsCIP = true;
    child->TCIP = this;

    //Save context
    thread->IsBlock = true;
    SaveContext(Registers, CoreID);

    //Update time
    Parent->TaskManagerParent->TimeByCore[CoreID] = HPET::GetTime();

    //Load new task
    child->CreateContext(Registers, CoreID);

    Atomic::atomicUnlock(&globalTaskManager->MutexScheduler, 0);

    return true;
}

bool thread_t::Launch(parameters_t* FunctionParameters){
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

    Registers->rip = (uint64_t)Parent->TaskManagerParent->IddleTaskPointer;

    return true;
}

bool thread_t::Exit(ContextStack* Registers, uint64_t CoreID){
    if(IsCIP){
        uint64_t ReturnValue = Registers->GlobalPurpose;
        Parent->TaskManagerParent->SwitchTask(Registers, CoreID, TCIP);
        Registers->GlobalPurpose = ReturnValue;
    }else if(IsEvent){
        // Clear event
    }

    ThreadNode->Delete();

    /* TODO clear task data and stack */

    free(this->Regs);
    free(this); 
    return true;
}