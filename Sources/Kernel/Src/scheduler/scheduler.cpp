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
    LastData = LastData->Next;
    LastData->Next = (ThreadQueuData_t*)malloc(sizeof(ThreadQueuData_t));
    LastData->IsAwaitTask = IsAwaitTask;
    LastData->Data = NULL;
    LastData->Task = Self;
    if(FunctionParameters != NULL){
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
        memcpy(&LastData->Parameters, FunctionParameters, sizeof(arguments_t));
    }else{
        memset(&LastData->Parameters, 0, sizeof(arguments_t));
    }

    if(IsAwaitTask){
        LastData->AwaitTask = Caller;
    }

    if(!TasksInQueu){
        TasksInQueu++;
        CurrentData = LastData;
        ExecuteThreadInQueu();
    }else{
        TasksInQueu++;
    }
    return KSUCCESS;
}

KResult ThreadQueu_t::ExecuteThreadInQueu(){
    if(TasksInQueu){
        Atomic::atomicAcquire(&globalTaskManager->MutexScheduler, 0);
        if(CurrentData->Data){
            CurrentData->Task->ShareDataUsingStackSpace(CurrentData->Data->Data, CurrentData->Data->Size, &CurrentData->Parameters.arg[CurrentData->Data->ParameterPosition]);
            free(CurrentData->Data->Data);
            free(CurrentData->Data);
        }
        CurrentData->Task->Launch_WL(&CurrentData->Parameters);
        Atomic::atomicUnlock(&globalTaskManager->MutexScheduler, 0);
        return KSUCCESS;
    }else{
        return KFAIL;
    }
}

KResult ThreadQueu_t::ExecuteThreadInQueuFromItself_WL(ContextStack* Registers, uint64_t CoreID){
    if(TasksInQueu){
        if(CurrentData->Data){
            CurrentData->Task->ShareDataUsingStackSpace(CurrentData->Data->Data, CurrentData->Data->Size, &CurrentData->Parameters.arg[CurrentData->Data->ParameterPosition]);
            free(CurrentData->Data->Data);
            free(CurrentData->Data);
        }
        Registers->rsp = (uint64_t)CurrentData->Task->Regs->rsp;
        Registers->rip = (uint64_t)CurrentData->Task->Regs->rip;
        Registers->cs = (uint64_t)CurrentData->Task->Regs->cs;
        Registers->ss = (uint64_t)CurrentData->Task->Regs->ss;
        return KSUCCESS;
    }else{
        return KFAIL;
    }
}

KResult ThreadQueu_t::NextThreadInQueu(){
    ThreadQueuData_t* CurrentDataOld = CurrentData;
    CurrentData = CurrentData->Next;
    free(CurrentDataOld);
    TasksInQueu--;
    return KSUCCESS;
}

KResult TaskManager::Execthread(kthread_t* Caller, kthread_t* Self, enum ExecutionType Type, arguments_t* FunctionParameters, ThreadShareData_t* Data, ContextStack* Registers, uint64_t CoreID){
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
            Caller->Pause(Registers, CoreID);
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
                Caller->Pause(Registers, CoreID);
            }else{
                return KFAIL;
            }
            break;
        }        
    }
    Atomic::atomicUnlock(&queu->Lock, 0);
    return KSUCCESS;
}

uint64_t TaskManager::Pause(ContextStack* Registers, uint64_t CoreID, kthread_t* task){
    if(task->IsInQueue){
        DequeueTask(task);
    }else if(CoreID == task->CoreID){
        task->Pause(Registers, CoreID);
    }else{
        APIC::GenerateInterruption(task->CoreID, INT_DestroySelf);
    }

    Atomic::atomicAcquire(&MutexScheduler, 0);
    task->IsBlock = true;
    Atomic::atomicUnlock(&MutexScheduler, 0);

    return KSUCCESS;
}

uint64_t TaskManager::Unpause(kthread_t* task){
    Atomic::atomicAcquire(&MutexScheduler, 0);
    task->IsBlock = false;
    EnqueueTask_WL(task);
    Atomic::atomicUnlock(&MutexScheduler, 0);
    return KSUCCESS;
} 

uint64_t TaskManager::Exit(ContextStack* Registers, uint64_t CoreID, kthread_t* task){   
    Atomic::atomicAcquire(&MutexScheduler, 0);
    Atomic::atomicAcquire(&task->Queu->Lock, 0); 
    if(task->CloseQueu_WL(Registers, CoreID) == KSUCCESS){
        Atomic::atomicUnlock(&MutexScheduler, 0);
        Atomic::atomicUnlock(&task->Queu->Lock, 0);
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
        globalTaskManager->IsSchedulerEnable[task->CoreID] = true;
        ForceSelfDestruction();
        /* noreturn */
    }

    return KSUCCESS;
}
uint64_t TaskManager::ShareDataUsingStackSpace(kthread_t* self, uintptr_t data, size_t size, uint64_t* location){
    return self->ShareDataUsingStackSpace(data, size, location);
}

uint64_t TaskManager::CreateProcess(kprocess_t** key, uint8_t priviledge, uint64_t externalData){
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

    Keyhole_Create(&proc->ProcessKey, proc, proc, DataTypeProcess, (uint64_t)proc, DefaultFlagsKey);

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
    thread->IsExit = true;
    thread->Parent = this;
    thread->Queu = (ThreadQueu_t*)calloc(sizeof(ThreadQueu_t));
    thread->Queu->LastData = (ThreadQueuData_t*)malloc(sizeof(ThreadQueuData_t));
    thread->Queu->LastData->Next = (ThreadQueuData_t*)malloc(sizeof(ThreadQueuData_t));
    thread->Queu->CurrentData = thread->Queu->LastData;    

    /* ID */
    thread->TID = TID; 
    TID++;
    NumberOfthread++;

    /* thread data */
    uintptr_t threadDataPA = Pmm_RequestPage();
    thread->threadData = (SelfData*)vmm_GetVirtualAddress(threadDataPA);

    thread->threadData->ProcessKey = ProcessKey;
    Keyhole_Create(&thread->threadData->threadKey, this, this, DataTypethread, (uint64_t)thread, DefaultFlagsKey);

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
    thread->IsExit = true;
    thread->Parent = this;
    thread->Queu = (ThreadQueu_t*)calloc(sizeof(ThreadQueu_t));
    thread->Queu->LastData = (ThreadQueuData_t*)malloc(sizeof(ThreadQueuData_t));
    thread->Queu->LastData->Next = (ThreadQueuData_t*)malloc(sizeof(ThreadQueuData_t));
    thread->Queu->CurrentData = thread->Queu->LastData;  
    
    /* ID */
    thread->TID = TID; 
    TID++;
    NumberOfthread++;

    /* thread data */
    uintptr_t threadDataPA = Pmm_RequestPage();
    thread->threadData = (SelfData*)vmm_GetVirtualAddress(threadDataPA);
    
    thread->threadData->ProcessKey = ProcessKey;
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
    IsExit = false;
    Parent->TaskManagerParent->EnqueueTask_WL(this);
    return true;
}

bool kthread_t::Pause(ContextStack* Registers, uint64_t CoreID){
    Atomic::atomicAcquire(&globalTaskManager->MutexScheduler, 0);
    //Save context
    SaveContext(Registers, CoreID);

    //Update time
    Parent->TaskManagerParent->TimeByCore[CoreID] = HPET::GetTime();

    IsBlock = true;
    globalTaskManager->IsSchedulerEnable[CoreID] = true;
    ForceSelfDestruction();

    /* No return */

    return true;
}

KResult kthread_t::Close(ContextStack* Registers, uint64_t CoreID){
    if(CloseQueu(Registers, CoreID) != KSUCCESS){
        Atomic::atomicAcquire(&globalTaskManager->MutexScheduler, 0);
        ForceSelfDestruction();
    }
    return KSUCCESS;
}

KResult kthread_t::CloseQueu(ContextStack* Registers, uint64_t CoreID){
    Atomic::atomicAcquire(&Queu->Lock, 0);
    Atomic::atomicAcquire(&globalTaskManager->MutexScheduler, 0);
    KResult statu = CloseQueu_WL(Registers, CoreID);
    Atomic::atomicUnlock(&globalTaskManager->MutexScheduler, 0);
    Atomic::atomicUnlock(&Queu->Lock, 0);
    return statu;
}

KResult kthread_t::CloseQueu_WL(ContextStack* Registers, uint64_t CoreID){
    Regs->rsp = (uint64_t)StackTop;
    Regs->rip = (uint64_t)EntryPoint;
    Regs->cs = Registers->threadInfo->CS;
    Regs->ss = Registers->threadInfo->SS;
    uint64_t ReturnValue = Registers->GlobalPurpose;
    ThreadQueuData_t* CurrentDataQueu = Queu->CurrentData;
    Queu->NextThreadInQueu();
    KResult statu = Queu->ExecuteThreadInQueuFromItself_WL(Registers, CoreID);
    if(CurrentDataQueu->IsAwaitTask){
        CurrentDataQueu->AwaitTask->Regs->GlobalPurpose = ReturnValue;
        Atomic::atomicUnlock(&globalTaskManager->MutexScheduler, 0);
        globalTaskManager->Unpause(CurrentDataQueu->AwaitTask);
        Atomic::atomicAcquire(&globalTaskManager->MutexScheduler, 0);
    }
    return statu;
}