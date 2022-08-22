#include <scheduler/scheduler.h>

TaskManager* globalTaskManager;

void TaskManager::Scheduler(ContextStack* Registers, uint64_t CoreID){  
    if(Atomic::atomicLock(&MutexScheduler, 0)){
        if(IsSchedulerEnable[CoreID]){
            /* Update time */
            uint64_t actualTime = HPET::GetTime();
            TimeByCore[CoreID] = actualTime;

            /* Save thread */
            kthread_t* threadEnd = threadExecutePerCore[CoreID];
            if(threadExecutePerCore[CoreID] != NULL){
                threadEnd->SaveContext(Registers, CoreID);
                EnqueueTask_WL(threadEnd);
            }

            /* Find & restore thread */
            kthread_t* threadStart = GetTread_WL();
            threadStart->CreateContext(Registers, CoreID);
        } 
        Atomic::atomicUnlock(&MutexScheduler, 0);       
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
    Atomic::atomicUnlock(&MutexScheduler, 0);     
}

void TaskManager::EnqueueTask(kthread_t* thread){
    Atomic::atomicAcquire(&MutexScheduler, 0);
    EnqueueTask_WL(thread);
    Atomic::atomicUnlock(&MutexScheduler, 0);
}

void TaskManager::EnqueueTask_WL(kthread_t* thread){
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
    DequeueTask_WL(thread);
    Atomic::atomicUnlock(&MutexScheduler, 0);
}

void TaskManager::DequeueTask_WL(kthread_t* thread){
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

kthread_t* TaskManager::GetTread_WL(){
    kthread_t* ReturnValue = FirstNode;
    DequeueTask_WL(ReturnValue);
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

KResult ThreadQueu_t::SetThreadInQueu(kthread_t* Caller, kthread_t* Self, arguments_t* FunctionParameters, bool IsAwaitTask, ThreadShareData_t* Data){
    if(TasksInQueu && LastData != NULL){
        LastData->Next = (ThreadQueuData_t*)malloc(sizeof(ThreadQueuData_t));
        LastData = LastData->Next;
    }else{
        LastData = (ThreadQueuData_t*)malloc(sizeof(ThreadQueuData_t));
        CurrentData = LastData;
    }
    LastData->IsAwaitTask = IsAwaitTask;
    LastData->Data = NULL;
    LastData->Task = Self;
    if(FunctionParameters != NULL){
        memcpy(&LastData->Parameters, FunctionParameters, sizeof(arguments_t));
    }else{
        memset(&LastData->Parameters, 0, sizeof(arguments_t));
    }

    if(Data != NULL){
        LastData->Data = (ThreadShareData_t*)malloc(sizeof(ThreadShareData_t));
        LastData->Data->Size = Data->Size;
        LastData->Data->ParameterPosition = Data->ParameterPosition;
        LastData->Data->Data = malloc(Data->Size);
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
    }
    return KSUCCESS;
}

KResult ThreadQueu_t::ExecuteThreadInQueu_WL(){
    if(TasksInQueu){
        CurrentData->Task->ResetContext(CurrentData->Task->Regs);
        if(CurrentData->Data){
            CurrentData->Task->ShareDataUsingStackSpace(CurrentData->Data->Data, CurrentData->Data->Size, (uintptr_t*)&CurrentData->Parameters.arg[CurrentData->Data->ParameterPosition]);
            free(CurrentData->Data->Data);
            free(CurrentData->Data);
        }
        CurrentData->Task->Launch_WL(&CurrentData->Parameters);
        return KSUCCESS;
    }else{
        return KFAIL;
    }
}

KResult ThreadQueu_t::ExecuteThreadInQueu(){
    Atomic::atomicAcquire(&globalTaskManager->MutexScheduler, 0);
    KResult statu = ExecuteThreadInQueu_WL();
    Atomic::atomicUnlock(&globalTaskManager->MutexScheduler, 0);
    return statu;
}

KResult ThreadQueu_t::NextThreadInQueu(){
    ThreadQueuData_t* CurrentDataOld = CurrentData;
    CurrentData = CurrentData->Next;
    free(CurrentDataOld);
    TasksInQueu--;
    return KSUCCESS;
}

KResult TaskManager::Execthread(kthread_t* Caller, kthread_t* Self, enum ExecutionType Type, arguments_t* FunctionParameters, ThreadShareData_t* Data, ContextStack* Registers){
    ThreadQueu_t* queu = Self->Queu;
    Atomic::atomicAcquire(&queu->Lock, 0);
    switch (Type){
        case ExecutionTypeQueu:{
            queu->SetThreadInQueu(Caller, Self, FunctionParameters, false, Data);
            break;
        }        
        case ExecutionTypeQueuAwait:{
            queu->SetThreadInQueu(Caller, Self, FunctionParameters, true, Data);
            Atomic::atomicUnlock(&queu->Lock, 0);
            Caller->Pause(Registers, true);
            break;
        }        
        case ExecutionTypeOneshot:{
            if(!queu->TasksInQueu){
                queu->SetThreadInQueu(Caller, Self, FunctionParameters, false, Data);
            }else{
                return KFAIL;
            }
            break;
        }        
        case ExecutionTypeOneshotAwait:{
            if(!queu->TasksInQueu){
                queu->SetThreadInQueu(Caller, Self, FunctionParameters, true, Data);
                Atomic::atomicUnlock(&queu->Lock, 0);
                Caller->Pause(Registers, true);
            }else{
                return KFAIL;
            }
            break;
        }        
    }
    Atomic::atomicUnlock(&queu->Lock, 0);
    return KSUCCESS;
}

uint64_t TaskManager::Unpause(kthread_t* task){
    Atomic::atomicAcquire(&MutexScheduler, 0);
    Unpause_WL(task);
    Atomic::atomicUnlock(&MutexScheduler, 0);
    return KSUCCESS;
} 

uint64_t TaskManager::Unpause_WL(kthread_t* task){
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

uint64_t TaskManager::Exit(ContextStack* Registers, kthread_t* task){   
    Atomic::atomicAcquire(&MutexScheduler, 0);
    Atomic::atomicAcquire(&task->Queu->Lock, 0); 
    if(task->CloseQueu_WL(Registers) == KSUCCESS){
        Atomic::atomicUnlock(&MutexScheduler, 0);
        Atomic::atomicUnlock(&task->Queu->Lock, 0);
        ForceSelfDestruction();
        return KSUCCESS;
    } 
    if(task->IsEvent){
        // Clear event
    }

    task->threadNode->Delete();

    /* TODO clear task data and stack */
    free(task->Regs);
    task->Regs = (ContextStack*)0x42;
    free(task); 
    

    if(task->IsInQueue){
        DequeueTask(task);
        Atomic::atomicUnlock(&MutexScheduler, 0);
    }else{
        ForceSelfDestruction();
        /* noreturn */
    }

    return KSUCCESS;
}
uint64_t TaskManager::ShareDataUsingStackSpace(kthread_t* self, uintptr_t data, size64_t size, uintptr_t* location){
    return self->ShareDataUsingStackSpace(data, size, location);
}

uint64_t TaskManager::CreateProcess(kprocess_t** key, enum Priviledge priviledge, uint64_t externalData){
    Atomic::atomicAcquire(&MutexScheduler, 1);
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

    Keyhole_Create(&proc->ProcessKey, proc, proc, DataTypeProcess, (uint64_t)proc, DefaultFlagsKey, PriviledgeApp);

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

kthread_t* kprocess_t::Createthread(uintptr_t entryPoint, enum Priviledge priviledge, uint64_t externalData){
    Atomic::atomicAcquire(&globalTaskManager->MutexScheduler, 1);
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
    thread->IsClose = true;
    thread->Parent = this;
    thread->Queu = (ThreadQueu_t*)calloc(sizeof(ThreadQueu_t));

    /* ID */
    thread->TID = TID; 
    TID++;
    NumberOfthread++;

    /* thread data */
    uintptr_t threadDataPA = Pmm_RequestPage();
    thread->threadData = (SelfData*)vmm_GetVirtualAddress(threadDataPA);

    thread->threadData->ProcessKey = ProcessKey;
    Keyhole_Create(&thread->threadData->threadKey, this, this, DataTypethread, (uint64_t)thread, DefaultFlagsKey, PriviledgeApp);

    vmm_Map(thread->Paging, (uintptr_t)SelfDataStartAddress, threadDataPA, thread->RingPL == UserAppRing);


    Atomic::atomicUnlock(&globalTaskManager->MutexScheduler, 1);
    return thread;
}

kthread_t* kprocess_t::Duplicatethread(kthread_t* source, uint64_t externalData){
    Atomic::atomicAcquire(&globalTaskManager->MutexScheduler, 1);
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
    thread->IsClose = true;
    thread->Parent = this;
    thread->Queu = (ThreadQueu_t*)calloc(sizeof(ThreadQueu_t)); 
    
    /* ID */
    thread->TID = TID; 
    TID++;
    NumberOfthread++;

    /* thread data */
    uintptr_t threadDataPA = Pmm_RequestPage();
    thread->threadData = (SelfData*)vmm_GetVirtualAddress(threadDataPA);
    
    thread->threadData->ProcessKey = ProcessKey;
    Keyhole_Create(&thread->threadData->ProcessKey, this, this, DataTypeProcess, (uint64_t)this, KeyholeFlagFullPermissions, PriviledgeApp);

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
        CreateProcess(&IddleProc, PriviledgeApp, 0);
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

void kthread_t::ResetContext(ContextStack* Registers){
    Registers->rsp = (uint64_t)StackTop;
    Registers->rip = (uint64_t)EntryPoint;
    Registers->cs = (uint64_t)Registers->threadInfo->CS;
    Registers->ss = (uint64_t)Registers->threadInfo->SS;
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
        vmm_Map(Paging, (uintptr_t)address, Pmm_RequestPage(), true, true, true);
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
            vmm_Map(Paging, (uintptr_t)(address + i * PAGE_SIZE), Pmm_RequestPage(), true, true, true);
        }        
    }

    return true;
}

KResult kthread_t::ShareDataUsingStackSpace(uintptr_t data, size64_t size, uintptr_t* location){
    *location = 0;
    if(!IsClose){
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

    Regs->rsp -= size;

    uintptr_t address = (uintptr_t)Regs->rsp;

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
    Atomic::atomicAcquire(&globalTaskManager->MutexScheduler, 0);
    Launch_WL(FunctionParameters);
    Atomic::atomicUnlock(&globalTaskManager->MutexScheduler, 0);
    return true;
}

bool kthread_t::Launch(){
    Atomic::atomicAcquire(&globalTaskManager->MutexScheduler, 0);
    Launch_WL();
    Atomic::atomicUnlock(&globalTaskManager->MutexScheduler, 0);
    return true;
}

bool kthread_t::Launch_WL(arguments_t* FunctionParameters){
    if(FunctionParameters != NULL){
        SetParameters(FunctionParameters);
    }
    Launch_WL();
    return true;
}

bool kthread_t::Launch_WL(){
    IsBlock = false;
    IsClose = false;
    Parent->TaskManagerParent->EnqueueTask_WL(this);
    return true;
}

bool kthread_t::Pause(ContextStack* Registers, bool force){
    Atomic::atomicAcquire(&globalTaskManager->MutexScheduler, 0);
    return Pause_WL(Registers, force);
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
    //Save context
    SaveContext(Registers);
    
    ForceSelfDestruction();

    /* No return */

    return true;
}

KResult kthread_t::Close(ContextStack* Registers){
    Atomic::atomicAcquire(&Queu->Lock, 0);
    Atomic::atomicAcquire(&globalTaskManager->MutexScheduler, 0);

    CloseQueu_WL(Registers);

    Atomic::atomicUnlock(&globalTaskManager->MutexScheduler, 0);
    Atomic::atomicUnlock(&Queu->Lock, 0);
    
    ForceSelfDestruction();
    return KSUCCESS;
}

KResult kthread_t::CloseQueu(ContextStack* Registers){
    Atomic::atomicAcquire(&Queu->Lock, 0);
    Atomic::atomicAcquire(&globalTaskManager->MutexScheduler, 0);
    KResult statu = CloseQueu_WL(Registers);
    Atomic::atomicUnlock(&globalTaskManager->MutexScheduler, 0);
    Atomic::atomicUnlock(&Queu->Lock, 0);
    return statu;
}

KResult kthread_t::CloseQueu_WL(ContextStack* Registers){
    ResetContext(Regs);
    uint64_t ReturnValue = Registers->GlobalPurpose;
    ThreadQueuData_t* CurrentDataQueu = Queu->CurrentData;
    Queu->NextThreadInQueu();
    KResult statu = Queu->ExecuteThreadInQueu_WL();
    if(CurrentDataQueu->IsAwaitTask){
        CurrentDataQueu->AwaitTask->Regs->GlobalPurpose = ReturnValue;
        globalTaskManager->Unpause_WL(CurrentDataQueu->AwaitTask);
    }
    
    if(statu != KSUCCESS){
        Queu->LastData = NULL;
        IsBlock = true;
        IsClose = true;
        IsPause = false;
    }

    return statu;
}

void SetParameters(ContextStack* Registers, arguments_t* FunctionParameters){
    Registers->arg0 = FunctionParameters->arg[0];
    Registers->arg1 = FunctionParameters->arg[1];
    Registers->arg2 = FunctionParameters->arg[2];
    Registers->arg3 = FunctionParameters->arg[3];
    Registers->arg4 = FunctionParameters->arg[4];
    Registers->arg5 = FunctionParameters->arg[5];    
}