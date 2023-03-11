#include <scheduler/scheduler.h>

TaskManager* globalTaskManager;

void TaskManager::Scheduler(ContextStack* Registers, uint64_t CoreID){  
    if(IsSchedulerEnable[CoreID]){
        if(AtomicLock(&SchedulerLock)){
            /* Update time */
            uint64_t actualTime = HPET::GetTime();
            TimeByCore[CoreID] = actualTime;

            /* Save thread */
            kthread_t* threadEnd = ThreadExecutePerCore[CoreID];

            if(ThreadExecutePerCore[CoreID] != NULL){
                threadEnd->SaveContext(Registers, CoreID);
                EnqueueTask_WL(threadEnd);
            }

            /* Find & restore thread */
            kthread_t* threadStart = GetTread_WL();
            threadStart->CreateContext(Registers, CoreID);
            AtomicRelease(&SchedulerLock);       
        }
    }
}

/* To call destroy self you should acuqire mutex sheduler before */
void TaskManager::DestroySelf(ContextStack* Registers, uint64_t CoreID){
    /* Update time */
    uint64_t actualTime = HPET::GetTime();
    TimeByCore[CoreID] = actualTime;

    /* Find & restore thread */
    kthread_t* threadStart = GetTread_WL();
    threadStart->CreateContext(Registers, CoreID);
    AtomicRelease(&SchedulerLock);   
}

void TaskManager::EnqueueTask(kthread_t* thread){
    AcquireScheduler();
    EnqueueTask_WL(thread);
    ReleaseScheduler();
}

void TaskManager::EnqueueTask_WL(kthread_t* thread){
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
    AcquireScheduler();
    DequeueTask_WL(thread);
    ReleaseScheduler();
}

void TaskManager::DequeueTask_WL(kthread_t* thread){
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

kthread_t* TaskManager::GetTread_WL(){
    kthread_t* ReturnValue = FirstNode;
    DequeueTask_WL(ReturnValue);
    return ReturnValue;
}

KResult TaskManager::Createthread(kthread_t** self, kprocess_t* proc, uintptr_t entryPoint, uint64_t externalData){
    *self = proc->Createthread(entryPoint, externalData);
    return KSUCCESS;
}

KResult TaskManager::Createthread(kthread_t** self, kprocess_t* proc, uintptr_t entryPoint, enum Priviledge priviledge, uint64_t externalData){
    *self = proc->Createthread(entryPoint, priviledge, externalData);
    return KSUCCESS;
}

KResult TaskManager::Duplicatethread(kthread_t** self, kprocess_t* proc, kthread_t* source){
    if(source->Parent != proc) return KFAIL;
    *self = proc->Duplicatethread(source);
    return KSUCCESS;
}

KResult ThreadQueu_t::SetThreadInQueu(kthread_t* Caller, kthread_t* Self, arguments_t* FunctionParameters, bool IsAwaitTask, ThreadShareData_t* Data){
    if(TasksInQueu){
        LastData->Next = (ThreadQueuData_t*)kmalloc(sizeof(ThreadQueuData_t));
        LastData = LastData->Next;
    }else{
        LastData = (ThreadQueuData_t*)kmalloc(sizeof(ThreadQueuData_t));
        CurrentData = LastData;
    }

    LastData->IsAwaitTask = IsAwaitTask;
    LastData->Data = NULL;
    LastData->Task = Self;
    LastData->Caller = Caller;

    if(FunctionParameters != NULL){
        memcpy(&LastData->Parameters, FunctionParameters, sizeof(arguments_t));
    }

    if(Data != NULL && Data->Size != NULL){
        LastData->Data = (ThreadShareData_t*)kmalloc(sizeof(ThreadShareData_t));
        LastData->Data->Size = Data->Size;
        LastData->Data->ParameterPosition = Data->ParameterPosition;
        LastData->Data->Data = kmalloc(Data->Size);
        memcpy(LastData->Data->Data, Data->Data, Data->Size);
        if(LastData->Data->ParameterPosition > 0x5){
            LastData->Data->ParameterPosition = 0x0;
        }
    }

    if(IsAwaitTask){
        LastData->AwaitTask = Caller;
    }

    if(TasksInQueu){
        TasksInQueu++;
    }else{
        TasksInQueu++;
        ExecuteThreadInQueu();
        globalTaskManager->ReleaseScheduler();
    }
    return KSUCCESS;
}

/* Set Thread In Queu No Scheduler Unlock */
KResult ThreadQueu_t::SetThreadInQueu_NSU(kthread_t* Caller, kthread_t* Self, arguments_t* FunctionParameters, bool IsAwaitTask, ThreadShareData_t* Data){
    if(TasksInQueu){
        LastData->Next = (ThreadQueuData_t*)kmalloc(sizeof(ThreadQueuData_t));
        LastData = LastData->Next;
    }else{
        LastData = (ThreadQueuData_t*)kmalloc(sizeof(ThreadQueuData_t));
        CurrentData = LastData;
    }

    LastData->IsAwaitTask = IsAwaitTask;
    LastData->Data = NULL;
    LastData->Task = Self;
    LastData->Caller = Caller;

    if(FunctionParameters != NULL){
        memcpy(&LastData->Parameters, FunctionParameters, sizeof(arguments_t));
    }else{
        memset(&LastData->Parameters, 0, sizeof(arguments_t));
    }

    if(Data != NULL && Data->Size != NULL){
        LastData->Data = (ThreadShareData_t*)kmalloc(sizeof(ThreadShareData_t));
        LastData->Data->Size = Data->Size;
        LastData->Data->ParameterPosition = Data->ParameterPosition;
        LastData->Data->Data = kmalloc(Data->Size);
        memcpy(LastData->Data->Data, Data->Data, Data->Size);
        if(LastData->Data->ParameterPosition > 0x5){
            LastData->Data->ParameterPosition = 0x0;
        }
    }

    if(IsAwaitTask){
        LastData->AwaitTask = Caller;
    }

    if(TasksInQueu){
        TasksInQueu++;
        globalTaskManager->AcquireScheduler();
    }else{
        TasksInQueu++;
        ExecuteThreadInQueu();
    }
    return KSUCCESS;
}

KResult ThreadQueu_t::ExecuteThreadInQueu(){
    if(TasksInQueu){
        uint64_t DataLocation = CurrentData->Task->Stack->StackStart;
        if(CurrentData->Data){
            // We can use share stack space here because we already reset rsp with context
            CurrentData->Task->ShareDataUsingStackSpace(CurrentData->Data->Data, CurrentData->Data->Size, (uintptr_t*)&DataLocation);
            CurrentData->Parameters.arg[CurrentData->Data->ParameterPosition] = DataLocation;
            kfree(CurrentData->Data->Data);
            kfree(CurrentData->Data);
        }

        globalTaskManager->AcquireScheduler();

        SelfData* Data = CurrentData->Task->threadData;
        Data->PID_TLI = CurrentData->Caller->Parent->PID;
        Data->TID_TLI = CurrentData->Caller->TID;
        Data->ExternalData_P_TLI = CurrentData->Caller->Parent->ExternalData_P;
        Data->Priviledge_TLI = CurrentData->Caller->Priviledge;

        CurrentData->Task->ResetContext(CurrentData->Task->Regs);

        CurrentData->Task->Regs->rsp = DataLocation;
        
        CurrentData->Task->Launch_WL(&CurrentData->Parameters);
        return KSUCCESS;
    }else{
        globalTaskManager->AcquireScheduler();
        return KFAIL;
    }
}

KResult ThreadQueu_t::NextThreadInQueu_WL(){
    ThreadQueuData_t* CurrentDataOld = CurrentData;
    CurrentData = CurrentData->Next;
    kfree(CurrentDataOld);
    TasksInQueu--;
    return KSUCCESS;
}

KResult TaskManager::Execthread(kthread_t* Caller, kthread_t* Self, enum ExecutionType Type, arguments_t* FunctionParameters, ThreadShareData_t* Data, ContextStack* Registers){
    if(Self->IsEvent) return KFAIL;

    ThreadQueu_t* queu = Self->Queu;
    AtomicAquire(&queu->Lock);
    switch (Type){
        case ExecutionTypeQueu:{
            queu->SetThreadInQueu(Caller, Self, FunctionParameters, false, Data);
            break;
        }        
        case ExecutionTypeQueuAwait:{
            queu->SetThreadInQueu_NSU(Caller, Self, FunctionParameters, true, Data);
            AtomicRelease(&queu->Lock);
            Caller->Pause_WL(Registers, true); // We can do this because we have already lock the scheduler
            return KSUCCESS;
        }        
        case ExecutionTypeOneshot:{
            if(!queu->TasksInQueu){
                queu->SetThreadInQueu(Caller, Self, FunctionParameters, false, Data);
            }else{
                AtomicRelease(&queu->Lock);
                return KFAIL;
            }
            break;
        }        
        case ExecutionTypeOneshotAwait:{
            if(!queu->TasksInQueu){
                queu->SetThreadInQueu_NSU(Caller, Self, FunctionParameters, true, Data);
                AtomicRelease(&queu->Lock);
                Caller->Pause_WL(Registers, true); // We can do this because we have already lock the scheduler
                return KSUCCESS;
            }else{
                AtomicRelease(&queu->Lock);
                return KFAIL;
            }
            break;
        }        
    }
    AtomicRelease(&queu->Lock);
    return KSUCCESS;
}

KResult TaskManager::Unpause(kthread_t* task){
    AcquireScheduler();
    Unpause_WL(task);
    ReleaseScheduler();
    return KSUCCESS;
} 

KResult TaskManager::Unpause_WL(kthread_t* task){
    if(task->IsClose) return KFAIL;

    if(task->IsPause){
        task->IsPause = false;
        task->IsBlock = false;
        EnqueueTask_WL(task);
        return KSUCCESS;
    }else{
        task->UnpauseOverflowCounter++;
        return KFAIL;
    }
} 

KResult TaskManager::Exit(ContextStack* Registers, kthread_t* task, uint64_t ReturnValue){   
    AtomicAquire(&task->Queu->Lock);
    if(task->CloseQueu(ReturnValue) == KSUCCESS){
        AtomicRelease(&task->Queu->Lock);
        ForceSelfDestruction(); /* Unlock MutexScheduler */
        /* noreturn */
        return KSUCCESS;
    }

    globalTaskManager->ReleaseScheduler();

    if(task->IsEvent){
        // Clear event
    }

    task->ThreadNode->Delete();

    /* TODO clear task data and stack */
    
    globalTaskManager->AcquireScheduler();
    AtomicRelease(&task->Queu->Lock);
    ForceSelfDestruction(); /* Unlock MutexScheduler */
    /* No return */

    return KSUCCESS;
}

KResult TaskManager::CreateProcess(kprocess_t** key, enum Priviledge priviledge, uint64_t externalData){
    kprocess_t* proc = (kprocess_t*)kcalloc(sizeof(kprocess_t));

    AtomicAquire(&CreateProcessLock);
    if(ProcessList == NULL){
        ProcessList = CreateNode((uintptr_t)0);
        proc->NodeParent = ProcessList->Add(proc);
    }else{
        proc->NodeParent = ProcessList->Add(proc);
    }
    AtomicRelease(&CreateProcessLock);

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
    proc->ExternalData_P = externalData;

    Keyhole_Create(&proc->ProcessKey, proc, proc, DataTypeProcess, (uint64_t)proc, DefaultFlagsKey, PriviledgeApp);

    proc->PID = PID; 
    PID++;
    NumberProcessTotal++;

    *key = proc;
    return KSUCCESS;
}

KResult TaskManager::CreateProcess(kthread_t* caller, kprocess_t** key, enum Priviledge priviledge, uint64_t externalData){
    KResult status = CreateProcess(key, priviledge, externalData);
    kprocess_t* proc = *key;
    proc->PID_PCI = caller->Parent->PID;
    proc->TID_PCI = caller->TID;
    proc->ExternalData_P_PCI = caller->Parent->ExternalData_P;
    proc->Priviledge_PCI = caller->Priviledge;
    return status;
}

kthread_t* kprocess_t::Createthread(uintptr_t entryPoint, uint64_t externalData){
    return Createthread(entryPoint, DefaultPriviledge, externalData);
}

kthread_t* kprocess_t::Createthread(uintptr_t entryPoint, enum Priviledge priviledge, uint64_t externalData){
    kthread_t* thread = (kthread_t*)kcalloc(sizeof(kthread_t));

    AtomicAquire(&CreateThreadLocker);
    if(Childs == NULL){
        Childs = CreateNode((uintptr_t)0);
        thread->ThreadNode = Childs->Add(thread);
    }else{
        thread->ThreadNode = Childs->Add(thread);
    }
    AtomicRelease(&CreateThreadLocker);

    /* Allocate context */
    thread->Regs = (ContextStack*)kcalloc(sizeof(ContextStack));

    /* Copy paging */
    thread->Paging = vmm_Setupthread(this->SharedPaging);

    /* Setup priviledge */
    thread->Priviledge = priviledge;

    thread->EntryPoint = entryPoint;

    /* Setup registers */
    SetupRegistersForTask(thread);

    /* Setup SIMD */
    thread->SIMDSaver = simdCreateSaveSpace();

    /* thread info for kernel */
    thread->Info = (threadInfo_t*)kmalloc(sizeof(threadInfo_t));
    thread->Info->CS = thread->Regs->cs;
    thread->Info->SS = thread->Regs->ss;
    thread->Info->thread = thread;
    thread->Regs->threadInfo = thread->Info;

    /* Load new stack */
    thread->SetupStack();

    /* Other data */
    thread->CreationTime = HPET::GetTime();
    thread->MemoryAllocated = 0;
    thread->TimeAllocate = 0;
    thread->IsBlock = true;
    thread->IsClose = true;
    thread->Parent = this;
    thread->Queu = (ThreadQueu_t*)kcalloc(sizeof(ThreadQueu_t));
    thread->ExternalData_T = externalData;

    /* Thread Data */
    uintptr_t threadDataPA = Pmm_RequestPage();
    thread->MemoryAllocated += PAGE_SIZE;
    thread->threadData = (SelfData*)vmm_GetVirtualAddress(threadDataPA);
    
    Keyhole_Create(&thread->threadData->ThreadKey, this, this, DataTypeThread, (uint64_t)thread, DefaultFlagsKey, PriviledgeApp);
    vmm_Map(thread->Paging, (uintptr_t)SelfDataStartAddress, threadDataPA, thread->RingPL == UserAppRing);
    
    /* ID */
    thread->TID = this->TID; 
    TID++;
    NumberOfthread++;
    
    thread->threadData->ProcessKey = this->ProcessKey;
    thread->threadData->PID = this->PID;
    thread->threadData->TID = thread->TID;
    thread->threadData->ExternalData_T = thread->ExternalData_T;
    thread->threadData->ExternalData_P = this->ExternalData_P;
    thread->threadData->Priviledge = thread->Priviledge;
    thread->threadData->PID_PCI = this->PID_PCI;
    thread->threadData->TID_PCI = this->TID_PCI;
    thread->threadData->ExternalData_P_PCI = this->ExternalData_P_PCI;
    thread->threadData->Priviledge_PCI = this->Priviledge_PCI;


    return thread;
}

kthread_t* kprocess_t::Duplicatethread(kthread_t* source){
    kthread_t* thread = (kthread_t*)kcalloc(sizeof(kthread_t));

    AtomicAquire(&CreateThreadLocker);
    if(Childs == NULL){
        Childs = CreateNode((uintptr_t)0);
        thread->ThreadNode = Childs->Add(thread);
    }else{
        thread->ThreadNode = Childs->Add(thread);
    }
    AtomicRelease(&CreateThreadLocker);

    /* Allocate context */
    thread->Regs = (ContextStack*)kcalloc(sizeof(ContextStack));

    /* Copy paging */
    thread->Paging = vmm_Setupthread(this->SharedPaging);

    /* Setup SIMD */
    thread->SIMDSaver = simdCreateSaveSpace();

    /* thread info for kernel */
    thread->Info = (threadInfo_t*)kmalloc(sizeof(threadInfo_t));
    thread->Info->CS = source->Regs->cs;
    thread->Info->SS = source->Regs->ss;
    thread->Info->thread = thread;
    thread->EntryPoint = source->EntryPoint;

    /* Load new stack */
    thread->SetupStack();

    /* Setup priviledge */
    thread->Priviledge = source->Priviledge;
    thread->RingPL = source->RingPL;

    /* Setup registers */
    SetupRegistersForTask(thread);
    thread->Regs->threadInfo = thread->Info;

    /* Other data */
    thread->CreationTime = HPET::GetTime();
    thread->MemoryAllocated = 0;
    thread->TimeAllocate = 0;
    thread->IsBlock = true;
    thread->IsClose = true;
    thread->Parent = this;
    thread->Queu = (ThreadQueu_t*)kcalloc(sizeof(ThreadQueu_t)); 

    /* Thread Data */
    uintptr_t threadDataPA = Pmm_RequestPage();
    thread->MemoryAllocated += PAGE_SIZE;
    thread->threadData = (SelfData*)vmm_GetVirtualAddress(threadDataPA);
    
    Keyhole_Create(&thread->threadData->ThreadKey, this, this, DataTypeThread, (uint64_t)thread, DefaultFlagsKey, PriviledgeApp);
    vmm_Map(thread->Paging, (uintptr_t)SelfDataStartAddress, threadDataPA, thread->RingPL == UserAppRing);
    
    thread->threadData->ProcessKey = ProcessKey;
    thread->threadData->PID = PID;
    thread->threadData->TID = TID;
    thread->threadData->ExternalData_P = ExternalData_P;
    thread->threadData->Priviledge = thread->Priviledge;
    thread->threadData->PID_PCI = PID_PCI;
    thread->threadData->TID_PCI = TID_PCI;
    thread->threadData->ExternalData_P_PCI = ExternalData_P_PCI;
    thread->threadData->Priviledge_PCI = Priviledge_PCI;
    
    /* ID */
    thread->TID = TID; 
    TID++;
    NumberOfthread++;

    return thread;
}


void kthread_t::SetupStack(){
    uint64_t StackLocation = StackTop;
    this->Regs->rsp = StackLocation;
    this->Stack = (StackInfo*)kmalloc(sizeof(StackInfo));
    this->Stack->StackStart = StackLocation;
    this->Stack->StackEndMax = StackBottom;
    
    this->KernelInternalStack = (uintptr_t)((uint64_t)stackalloc(KERNEL_STACK_SIZE) + KERNEL_STACK_SIZE);

    this->Info->SyscallStack = (uint64_t)stackalloc(KERNEL_STACK_SIZE) + KERNEL_STACK_SIZE;

    /* Clear stack */
    vmm_page_table* PML4VirtualAddress = (vmm_page_table*)vmm_GetVirtualAddress((uintptr_t)Paging);
    PML4VirtualAddress->entries[0xff] = NULL;

}



void TaskManager::SwitchTask(ContextStack* Registers, uint64_t CoreID, kthread_t* task){
    if(task == NULL) return;

    AcquireScheduler();

    //Update time
    TimeByCore[CoreID] = HPET::GetTime();

    //Load new task
    task->CreateContext(Registers, CoreID);

    ReleaseScheduler();
}

void TaskManager::CreateIddleTask(){
    kthread_t* thread = KernelProc->Createthread(IddleTaskPointer, NULL);

    IdleNode[IddleTaskNumber] = thread;
    IddleTaskNumber++;
    
    thread->Launch();
}
void TaskManager::InitScheduler(uint8_t NumberOfCores, uintptr_t IddleTaskFunction){
    uintptr_t physcialMemory = Pmm_RequestPage();
    uintptr_t virtualMemory = (uintptr_t)vmm_GetVirtualAddress(physcialMemory);
    vmm_Map(vmm_PageTable, virtualMemory, physcialMemory, true, false);
    memcpy(virtualMemory, IddleTaskFunction, PAGE_SIZE);

    IddleTaskPointer = virtualMemory; 

    CreateProcess(&KernelProc, PriviledgeApp, 0);

    for(int i = 0; i < NumberOfCores; i++){
        CreateIddleTask();
    } 

    NumberOfCPU = NumberOfCores;
    TaskManagerInit = true;
}

void TaskManager::EnabledScheduler(uint64_t CoreID){ 
    if(TaskManagerInit){
        ThreadExecutePerCore[CoreID] = NULL;

        CPU::SetCPUGSKernelBase((uint64_t)SelfDataStartAddress); // keys position

        CPU::SetCPUFSBase((uint64_t)SelfDataEndAddress); // thread Local Storage

        SyscallEnable(GDTInfoSelectorsRing[KernelRing].Code, GDTInfoSelectorsRing[UserAppRing].Code); 

        IsSchedulerEnable[CoreID] = true;
    }
}

kthread_t* TaskManager::GetCurrentthread(uint64_t CoreID){
    return ThreadExecutePerCore[CoreID];
}

void TaskManager::AcquireScheduler(){
    AtomicAquireCli(&SchedulerLock);
}

void TaskManager::ReleaseScheduler(){
    AtomicRelease(&SchedulerLock);
    CPU::EnableInterrupts();
}

void kthread_t::SaveContext(ContextStack* Registers, uint64_t CoreID){
    uint64_t actualTime = HPET::GetTime();
    TimeAllocate += actualTime - Parent->TaskManagerParent->TimeByCore[CoreID];
    this->SaveContext(Registers);
}

void kthread_t::SaveContext(ContextStack* Registers){
    simdSave(SIMDSaver);
    memcpy(Regs, Registers, sizeof(ContextStack));
}

void kthread_t::CreateContext(ContextStack* Registers, uint64_t CoreID){
    this->CoreID = CoreID;
    Parent->TaskManagerParent->ThreadExecutePerCore[CoreID] = this;
    simdSave(SIMDSaver);
    memcpy(Registers, Regs, sizeof(ContextStack));
    TSSSetIST(CoreID, IST_Interrupts, (uint64_t)KernelInternalStack);
}

void kthread_t::ResetContext(ContextStack* Registers){
    Registers->rsp = (uint64_t)StackTop;
    Registers->rip = (uint64_t)EntryPoint;
    Registers->cs = (uint64_t)Registers->threadInfo->CS;
    Registers->ss = (uint64_t)Registers->threadInfo->SS;
    Registers->rflags.IF = true;
}

void kthread_t::SetParameters(arguments_t* FunctionParameters){
    Regs->arg0 = FunctionParameters->arg[0];
    Regs->arg1 = FunctionParameters->arg[1];
    Regs->arg2 = FunctionParameters->arg[2];
    Regs->arg3 = FunctionParameters->arg[3];
    Regs->arg4 = FunctionParameters->arg[4];
    Regs->arg5 = FunctionParameters->arg[5];
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
        uintptr_t PhysicalAddress = Pmm_RequestPage();
        MemoryAllocated += PAGE_SIZE;
        vmm_Map(Paging, (uintptr_t)address, PhysicalAddress, true, true, true);
        return true;
    }
    return false;
}

bool kthread_t::ExtendStack(uint64_t address, size64_t size){
    if(this->Stack == NULL) return false;

    size += address % PAGE_SIZE;
    address -= address % PAGE_SIZE;
    if(this->Stack->StackStart <= address) return false;
    if(address <= this->Stack->StackEndMax) return false;
    
    uint64_t pageCount = DivideRoundUp(size, PAGE_SIZE);
    
    for(uint64_t i = 0; i < pageCount; i++){
        if(!vmm_GetFlags(Paging, (uintptr_t)(address + i * PAGE_SIZE), vmm_PhysicalStorage) || !vmm_GetFlags(Paging, (uintptr_t)(address + i * PAGE_SIZE), vmm_Present)){
            uintptr_t PhysicalAddress = Pmm_RequestPage();
            MemoryAllocated += PAGE_SIZE;
            vmm_Map(Paging, (uintptr_t)(address + i * PAGE_SIZE), PhysicalAddress, true, true, true);
        }        
    }

    return true;
}

KResult kthread_t::ShareDataUsingStackSpace(uintptr_t data, size64_t size, uintptr_t* location){
    uintptr_t address = (uintptr_t)((uint64_t)*location - size);

    uint64_t pageCount = DivideRoundUp(size, PAGE_SIZE);

    uint64_t virtualAddressParentIterator = (uint64_t)data;
    uint64_t virtualAddressIterator = (uint64_t)address;
    uint64_t i = 0;
    if(virtualAddressIterator % PAGE_SIZE){
        uint64_t sizeToCopy = 0;
        uint64_t alignement = virtualAddressIterator % PAGE_SIZE;
        if(size > PAGE_SIZE - alignement){
            sizeToCopy = PAGE_SIZE - alignement;
        }else{
            sizeToCopy = size;
        }

        uintptr_t physicalPage = NULL;
        if(!vmm_GetFlags(Paging, (uintptr_t)virtualAddressIterator, vmm_PhysicalStorage)){
            physicalPage = Pmm_RequestPage();
            MemoryAllocated += PAGE_SIZE;
            vmm_Map(Paging, (uintptr_t)((uint64_t)virtualAddressIterator), physicalPage, true, true, true);
            physicalPage = (uintptr_t)((uint64_t)physicalPage + alignement);
        }else{
            physicalPage = vmm_GetPhysical(Paging, (uintptr_t)virtualAddressIterator);
        }
        memcpy((uintptr_t)(vmm_GetVirtualAddress(physicalPage)), (uintptr_t)virtualAddressParentIterator, sizeToCopy);

        virtualAddressParentIterator += sizeToCopy;
        virtualAddressIterator += sizeToCopy;
        size -= sizeToCopy;
        i++;
    }
    for(; i < pageCount; i++){
        uint64_t sizeToCopy;
        if(size > PAGE_SIZE){
            sizeToCopy = PAGE_SIZE;
        }else{
            sizeToCopy = size;
        }
    
        uintptr_t physicalPage = NULL;
        if(!vmm_GetFlags(Paging, (uintptr_t)virtualAddressIterator, vmm_PhysicalStorage)){
            physicalPage = Pmm_RequestPage();
            MemoryAllocated += PAGE_SIZE;
            vmm_Map(Paging, (uintptr_t)((uint64_t)virtualAddressIterator), physicalPage, true, true, true);
        }else{
            physicalPage = vmm_GetPhysical(Paging, (uintptr_t)virtualAddressIterator);
        }
        memcpy((uintptr_t)vmm_GetVirtualAddress(physicalPage), (uintptr_t)virtualAddressParentIterator, sizeToCopy);
        virtualAddressIterator += sizeToCopy;
        virtualAddressParentIterator += sizeToCopy;
        size -= sizeToCopy;
    }     

    *location = address;
    return KSUCCESS;
}

bool kthread_t::Launch(arguments_t* FunctionParameters){
    globalTaskManager->AcquireScheduler();
    Launch_WL(FunctionParameters);
    globalTaskManager->ReleaseScheduler();
    return true;
}

bool kthread_t::Launch_WL(arguments_t* FunctionParameters){
    if(FunctionParameters != NULL){
        SetParameters(FunctionParameters);
    }
    Launch_WL();
    return true;
}

bool kthread_t::Launch(){
    globalTaskManager->AcquireScheduler();
    Launch_WL();
    globalTaskManager->ReleaseScheduler();
    return true;
}

bool kthread_t::Launch_WL(){
    IsBlock = false;
    IsClose = false;
    Parent->TaskManagerParent->EnqueueTask_WL(this);
    return true;
}

bool kthread_t::Pause(ContextStack* Registers, bool force){
    globalTaskManager->AcquireScheduler();
    KResult Status = Pause_WL(Registers, force);
    globalTaskManager->ReleaseScheduler();
    return Status;
}

bool kthread_t::Pause_WL(ContextStack* Registers, bool force){
    if(force){
        UnpauseOverflowCounter = NULL;
    }else if(UnpauseOverflowCounter > 0){
        UnpauseOverflowCounter--;
        return false;
    }

    IsBlock = true;
    IsPause = true;
    
    // Save context
    SaveContext(Registers);
    
    ForceSelfDestruction(); /* Unlock MutexScheduler */

    /* No return */

    return true;
}

KResult kthread_t::Close(ContextStack* Registers, uint64_t ReturnValue){
    if(IsEvent) return KFAIL;

    AtomicAquire(&Queu->Lock);
    
    KResult Status = CloseQueu(ReturnValue);

    AtomicRelease(&Queu->Lock);
    
    ForceSelfDestruction(); /* Unlock MutexScheduler */
    return Status;
}

/// @brief Warning before calling the function please lock the queu and you should ResetContext(Regs) if the return success
/// @param Registers 
/// @param ReturnValue 
/// @return KResult 
KResult kthread_t::CloseQueu(uint64_t ReturnValue){
    ThreadQueuData_t* CurrentDataQueu = Queu->CurrentData;
    
    if(CurrentDataQueu->IsAwaitTask){
        CurrentDataQueu->AwaitTask->Regs->GlobalPurpose = ReturnValue;
        globalTaskManager->Unpause(CurrentDataQueu->AwaitTask);
    }

    Queu->NextThreadInQueu_WL();
    KResult Status = Queu->ExecuteThreadInQueu();

    if(Status != KSUCCESS){
        IsBlock = true;
        IsClose = true;
        IsPause = false;
    }
    return Status;
}

void SetParameters(ContextStack* Registers, arguments_t* FunctionParameters){
    Registers->arg0 = FunctionParameters->arg[0];
    Registers->arg1 = FunctionParameters->arg[1];
    Registers->arg2 = FunctionParameters->arg[2];
    Registers->arg3 = FunctionParameters->arg[3];
    Registers->arg4 = FunctionParameters->arg[4];
    Registers->arg5 = FunctionParameters->arg[5];    
}