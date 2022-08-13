#include <scheduler/scheduler.h>

TaskManager* globalTaskManager;

void TaskManager::Scheduler(ContextStack* Registers, uint64_t CoreID){  
    if(Atomic::atomicLock(&MutexScheduler, 0)){
        if(IsSchedulerEnable[CoreID]){  
            uint64_t actualTime = HPET::GetTime();
            kthread_t* threadEnd = threadExecutePerCore[CoreID];
            if(threadExecutePerCore[CoreID] != NULL){
                /* Save & enqueu thread */
                threadEnd->SaveContext(Registers, CoreID);
                EnqueueTaskWithoutLock(threadEnd);
                //Warning("PID %x use %u% of cpu", threadEnd->Parent->PID, (threadEnd->TimeAllocate * 100 / (actualTime - threadEnd->CreationTime)) / NumberOfCPU);
            }

            /* Update time */
            TimeByCore[CoreID] = actualTime;

            /* Find & load new task */
            kthread_t* threadStart = GetTreadWithoutLock();
            threadStart->CreateContext(Registers, CoreID);
        } 
        Atomic::atomicUnlock(&MutexScheduler, 0);       
    }

}

void TaskManager::EnqueueTask(kthread_t* thread){
    Atomic::atomicAcquire(&MutexScheduler, 0);
    EnqueueTaskWithoutLock(thread);
    Atomic::atomicUnlock(&MutexScheduler, 0);
}

void TaskManager::EnqueueTaskWithoutLock(kthread_t* thread){
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

void TaskManager::DequeueTask(kthread_t* thread){
    Atomic::atomicAcquire(&MutexScheduler, 0);
    DequeueTaskWithoutLock(thread);
    Atomic::atomicUnlock(&MutexScheduler, 0);
}

void TaskManager::DequeueTaskWithoutLock(kthread_t* thread){
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

kthread_t* TaskManager::GetTreadWithoutLock(){
    kthread_t* ReturnValue = FirstNode;
    DequeueTaskWithoutLock(ReturnValue);
    return ReturnValue;
}

uint64_t TaskManager::Createthread(kthread_t** self, kprocess_t* proc, uintptr_t entryPoint, uint64_t externalData){
    *self = proc->Createthread(entryPoint, externalData);
    return KSUCCESS;
}

uint64_t TaskManager::Createthread(kthread_t** self, kprocess_t* proc, uintptr_t entryPoint, uint8_t privilege, uint64_t externalData){
    *self = proc->Createthread(entryPoint, externalData);
    return KSUCCESS;
}

uint64_t TaskManager::Duplicatethread(kthread_t** self, kprocess_t* proc, kthread_t* source, uint64_t externalData){
    if(source->Parent != proc) return KFAIL;
    *self = proc->Duplicatethread(source, externalData);
    return KSUCCESS;
}

uint64_t TaskManager::Execthread(kthread_t* self, parameters_t* FunctionParameters){
    if(self->IsBlock){
        self->Launch(FunctionParameters);
        return KSUCCESS;
    }else{
        return KFAIL;
    }
}

uint64_t TaskManager::Pause(ContextStack* Registers, uint64_t CoreID, kthread_t* task){
    if(task->IsInQueue){
        DequeueTask(task);
    }else if(CoreID == task->CoreID){
        task->Pause(Registers, CoreID);
    }else{
        threadExecutePerCore[task->CoreID] = NULL;
        APIC::GenerateInterruption(task->CoreID, IPI_Schedule);
    }

    task->IsBlock = true;

    return KSUCCESS;
}

uint64_t TaskManager::Unpause(kthread_t* task){
    task->IsBlock = false;
    EnqueueTask(task);
    return KSUCCESS;
} 

uint64_t TaskManager::Exit(ContextStack* Registers, uint64_t CoreID, kthread_t* task){    
    Atomic::atomicAcquire(&MutexScheduler, 0);
    if(task->IsIPC){
        Atomic::atomicAcquire(&task->EventLock, 0);
        IPCData_t* Current = task->IPCInfo->CurrentData;
        if(!task->IPCInfo->IsAsync){
            task->Regs->rsp = (uint64_t)StackTop;
            task->Regs->rip = (uint64_t)task->EntryPoint;
            task->Regs->cs = Registers->threadInfo->CS;
            task->Regs->ss = Registers->threadInfo->SS;

            if(task->IPCInfo->TasksInQueu){
                IPCData_t* Next = task->IPCInfo->CurrentData->Next;
                Registers->rsp = (uint64_t)StackTop;
                Registers->rip = (uint64_t)task->Regs->rip;

                free(task->IPCInfo->CurrentData);
                task->IPCInfo->CurrentData = Next;
                task->IPCInfo->TasksInQueu--;
                Current->thread->Regs->GlobalPurpose = Registers->GlobalPurpose;
                Unpause(Current->thread); 
                Atomic::atomicUnlock(&task->EventLock, 0);
            }else{
                Current->thread->Regs->GlobalPurpose = Registers->GlobalPurpose;
                Unpause(Current->thread); 
                
                globalTaskManager->threadExecutePerCore[task->CoreID] = NULL;
                task->IsExit = true;
                task->IsBlock = true;
                Atomic::atomicUnlock(&task->EventLock, 0);
                ForceSchedule();
            }
            return KSUCCESS;
        }else{
            Current->thread->Regs->GlobalPurpose = Registers->GlobalPurpose;
            Unpause(Current->thread);     
            Atomic::atomicUnlock(&task->EventLock, 0);       
        }
    }


    if(task->IsEvent){
        // Clear event
    }

    task->threadNode->Delete();

    /* TODO clear task data and stack */

    free(task->Regs);
    free(task); 

    if(task->IsInQueue){
        DequeueTask(task);
        Atomic::atomicUnlock(&MutexScheduler, 0);
    }else{
        globalTaskManager->threadExecutePerCore[task->CoreID] = NULL;
        Atomic::atomicUnlock(&MutexScheduler, 0);
        ForceSchedule();

        /* noreturn */
    }


    return KSUCCESS;
}
uint64_t TaskManager::ShareDataUsingStackSpace(kthread_t* self, uintptr_t data, size_t size, uint64_t* location){
    return self->ShareDataUsingStackSpace(data, size, location);
}

uint64_t TaskManager::CreateProcess(kprocess_t** key, uint8_t priviledge, uint64_t externalData){
    Atomic::atomicLock(&MutexScheduler, 1);
    kprocess_t* proc = (kprocess_t*)calloc(sizeof(kprocess_t));

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

kthread_t* kprocess_t::Createthread(uintptr_t entryPoint, uint64_t externalData){
    return Createthread(entryPoint, DefaultPriviledge, externalData);
}

kthread_t* kprocess_t::Createthread(uintptr_t entryPoint, uint8_t priviledge, uint64_t externalData){
    Atomic::atomicLock(&globalTaskManager->MutexScheduler, 1);
    kthread_t* thread = (kthread_t*)calloc(sizeof(kthread_t));
    if(Childs == NULL){
        Childs = CreateNode((uintptr_t)0);
        thread->threadNode = Childs->Add(thread);
    }else{
        thread->threadNode = Childs->Add(thread);
    }

    /* Allocate context */
    thread->Regs = (ContextStack*)calloc(sizeof(ContextStack));

    /* Copy paging */
    thread->Paging = vmm_Setupthread(this->SharedPaging);

    /* Load new stack */
    thread->SetupStack();

    /* Setup priviledge */
    thread->Priviledge = priviledge;


    thread->EntryPoint = entryPoint;

    /* Setup registers */
    SetupRegistersForTask(thread);

    /* Setup SIMD */
    thread->SIMDSaver = simdCreateSaveSpace();

    /* thread info for kernel */
    thread->Info = (threadInfo_t*)malloc(sizeof(threadInfo_t));
    thread->Info->SyscallStack = (uint64_t)malloc(KERNEL_STACK_SIZE) + KERNEL_STACK_SIZE; 
    thread->Info->CS = thread->Regs->cs;
    thread->Info->SS = thread->Regs->ss;
    thread->Info->thread = thread;
    thread->Regs->threadInfo = thread->Info;

    /* Other data */
    thread->externalData = externalData;
    thread->CreationTime = HPET::GetTime();
    thread->MemoryAllocated = 0;
    thread->TimeAllocate = 0;
    thread->IsBlock = true;
    thread->IsExit = true;
    thread->Parent = this;
    thread->IPCInfo = (IPCInfo_t*)malloc(sizeof(IPCInfo_t));
    thread->IPCInfo->LastData = (IPCData_t*)malloc(sizeof(IPCData_t));
    thread->IPCInfo->LastData->Next = (IPCData_t*)malloc(sizeof(IPCData_t));
    thread->IPCInfo->CurrentData = thread->IPCInfo->LastData;    

    /* ID */
    thread->TID = TID; 
    TID++;
    NumberOfthread++;

    /* thread data */
    uintptr_t threadDataPA = Pmm_RequestPage();
    thread->threadData = (SelfData*)vmm_GetVirtualAddress(threadDataPA);
    
    Keyhole_Create(&thread->threadData->threadKey, this, this, DataTypethread, (uint64_t)thread, DefaultFlagsKey);
    Keyhole_Create(&thread->threadData->ProcessKey, this, this, DataTypeProcess, (uint64_t)this, DefaultFlagsKey);

    vmm_Map(thread->Paging, (uintptr_t)SelfDataStartAddress, threadDataPA, thread->RingPL == UserAppRing);


    Atomic::atomicUnlock(&globalTaskManager->MutexScheduler, 1);
    return thread;
}

kthread_t* kprocess_t::Duplicatethread(kthread_t* source, uint64_t externalData){
    Atomic::atomicLock(&globalTaskManager->MutexScheduler, 1);
    kthread_t* thread = (kthread_t*)calloc(sizeof(kthread_t));
    if(Childs == NULL){
        Childs = CreateNode((uintptr_t)0);
        thread->threadNode = Childs->Add(thread);
    }else{
        thread->threadNode = Childs->Add(thread);
    }

    /* Allocate context */
    thread->Regs = (ContextStack*)calloc(sizeof(ContextStack));

    /* Copy paging */
    thread->Paging = vmm_Setupthread(SharedPaging);

    /* Load new stack */
    thread->SetupStack();

    /* Setup SIMD */
    thread->SIMDSaver = simdCreateSaveSpace();

    /* thread info for kernel */
    thread->Info = (threadInfo_t*)malloc(sizeof(threadInfo_t));
    thread->Info->SyscallStack = (uint64_t)malloc(KERNEL_STACK_SIZE) + KERNEL_STACK_SIZE; 
    thread->Info->CS = source->Regs->cs;
    thread->Info->SS = source->Regs->ss;
    thread->Info->thread = thread;
    thread->EntryPoint = source->EntryPoint;

    /* Setup priviledge */
    thread->Priviledge = source->Priviledge;
    thread->RingPL = source->RingPL;

    /* Setup registers */
    SetupRegistersForTask(thread);
    thread->Regs->threadInfo = thread->Info;

    /* Other data */
    thread->externalData = externalData;
    thread->CreationTime = HPET::GetTime();
    thread->MemoryAllocated = 0;
    thread->TimeAllocate = 0;
    thread->IsBlock = true;
    thread->IsExit = true;
    thread->Parent = this;
    thread->IPCInfo = (IPCInfo_t*)calloc(sizeof(IPCInfo_t*));
    thread->IPCInfo->LastData = (IPCData_t*)malloc(sizeof(IPCData_t));
    thread->IPCInfo->LastData->Next = (IPCData_t*)malloc(sizeof(IPCData_t));
    thread->IPCInfo->CurrentData = thread->IPCInfo->LastData;
    
    /* ID */
    thread->TID = TID; 
    TID++;
    NumberOfthread++;

    /* thread data */
    uintptr_t threadDataPA = Pmm_RequestPage();
    thread->threadData = (SelfData*)vmm_GetVirtualAddress(threadDataPA);
    
    Keyhole_Create(&thread->threadData->threadKey, this, this, DataTypethread, (uint64_t)thread, KeyholeFlagFullPermissions);
    Keyhole_Create(&thread->threadData->ProcessKey, this, this, DataTypeProcess, (uint64_t)this, KeyholeFlagFullPermissions);

    vmm_Map(thread->Paging, (uintptr_t)SelfDataStartAddress, threadDataPA, source->Regs->cs == GDTInfoSelectorsRing[UserAppRing].Code);

    Atomic::atomicUnlock(&globalTaskManager->MutexScheduler, 1);
    return thread;
}


void kthread_t::SetupStack(){
    uint64_t StackLocation = StackTop;
    this->Regs->rsp = StackLocation;
    this->Stack = (StackInfo*)malloc(sizeof(StackInfo));
    this->Stack->StackStart = StackLocation;
    this->Stack->StackEndMax = StackBottom;

    /* Clear stack */
    vmm_page_table* PML4VirtualAddress = (vmm_page_table*)vmm_GetVirtualAddress((uintptr_t)Paging);
    PML4VirtualAddress->entries[0xff] = NULL;
}



void TaskManager::SwitchTask(ContextStack* Registers, uint64_t CoreID, kthread_t* task){
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
    kthread_t* thread = IddleProc->Createthread(IddleTaskPointer, 0);

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
        threadExecutePerCore[CoreID] = NULL;

        CPU::SetCPUGSKernelBase((uint64_t)SelfDataStartAddress); // keys position

        CPU::SetCPUFSBase((uint64_t)SelfDataEndAddress); // thread Local Storage

        SyscallEnable(GDTInfoSelectorsRing[KernelRing].Code, GDTInfoSelectorsRing[UserAppRing].Code); 

        IsSchedulerEnable[CoreID] = true;
    }
}

kthread_t* TaskManager::GetCurrentthread(uint64_t CoreID){
    return threadExecutePerCore[CoreID];
}

void kthread_t::SaveContext(ContextStack* Registers, uint64_t CoreID){
    uint64_t actualTime = HPET::GetTime();
    TimeAllocate += actualTime - Parent->TaskManagerParent->TimeByCore[CoreID];
    SaveContext(Registers);
}

void kthread_t::SaveContext(ContextStack* Registers){
    simdSave(SIMDSaver);
    memcpy(Regs, Registers, sizeof(ContextStack));
}

void kthread_t::CreateContext(ContextStack* Registers, uint64_t CoreID){
    this->CoreID = CoreID;
    Parent->TaskManagerParent->threadExecutePerCore[CoreID] = this;
    simdSave(SIMDSaver);
    memcpy(Registers, Regs, sizeof(ContextStack));
}

void kthread_t::SetParameters(parameters_t* FunctionParameters){
    Regs->arg0 = FunctionParameters->Arg0;
    Regs->arg1 = FunctionParameters->Arg1;
    Regs->arg2 = FunctionParameters->Arg2;
    Regs->arg3 = FunctionParameters->Arg3;
    Regs->arg4 = FunctionParameters->Arg4;
    Regs->arg5 = FunctionParameters->Arg5;
}

void kthread_t::CopyStack(kthread_t* source){
    vmm_page_table* PML4VirtualAddressSource = (vmm_page_table*)vmm_GetVirtualAddress((uintptr_t)source->Paging);
    vmm_page_table* PML4VirtualAddressDestination = (vmm_page_table*)vmm_GetVirtualAddress((uintptr_t)Paging);
    PML4VirtualAddressSource->entries[0xff] = PML4VirtualAddressDestination->entries[0xff];
    Stack = source->Stack;
}

bool kthread_t::ExtendStack(uint64_t address){
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

bool kthread_t::ExtendStack(uint64_t address, size_t size){
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

KResult kthread_t::ShareDataUsingStackSpace(uintptr_t data, size_t size, uint64_t* location){
    *location = 0;
    if(!IsExit){
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

bool kthread_t::IPC(ContextStack* Registers, uint64_t CoreID, kthread_t* thread, parameters_t* FunctionParameters, bool IsAsync){
    if(IsAsync){
        Atomic::atomicAcquire(&thread->IPCInfo->Lock, 0);
        kthread_t* child = thread->Parent->Duplicatethread(thread, this->externalData);
        child->IsIPC = true;
        child->IPCInfo->CurrentData->thread = this;
        child->IPCInfo->IsAsync = true;

        child->Launch(FunctionParameters);
        Atomic::atomicUnlock(&thread->IPCInfo->Lock, 0);
    }else{
        Atomic::atomicAcquire(&thread->IPCInfo->Lock, 0);
        thread->IsIPC = true;
        thread->IPCInfo->IsAsync = false;

        if(thread->IsExit){
            thread->IPCInfo->CurrentData->thread = this;
            thread->Launch(FunctionParameters);
        }else{
            thread->IPCInfo->LastData = thread->IPCInfo->LastData->Next;
            thread->IPCInfo->LastData->Next = (IPCData_t*)malloc(sizeof(IPCData_t));
            thread->IPCInfo->LastData->thread = this;
            if(FunctionParameters != NULL){
                memcpy(&thread->IPCInfo->LastData->Parameters, FunctionParameters, sizeof(parameters_t));
            }else{
                memset(&thread->IPCInfo->LastData->Parameters, 0, sizeof(parameters_t));
            }

            thread->IPCInfo->TasksInQueu++;
        } 
        Atomic::atomicUnlock(&thread->IPCInfo->Lock, 0);
    }

    /* Pause task */
    Pause(Registers, CoreID);

    /* No return */

    return true;
}

bool kthread_t::Launch(parameters_t* FunctionParameters){
    if(FunctionParameters != NULL){
        SetParameters(FunctionParameters);
    }
    Launch();
    return true;
}

bool kthread_t::Launch(){
    IsBlock = false;
    IsExit = false;
    Parent->TaskManagerParent->EnqueueTask(this);
    return true;
}

bool kthread_t::Pause(ContextStack* Registers, uint64_t CoreID){
    //Save context
    SaveContext(Registers, CoreID);

    //Update time
    Parent->TaskManagerParent->TimeByCore[CoreID] = HPET::GetTime();

    Parent->TaskManagerParent->threadExecutePerCore[CoreID] = NULL;

    IsBlock = true;

    ForceSchedule();

    /* No return */

    return true;
}