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

            if(threadEnd != NULL){
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

/* To call destroy self you should acuqire mutex sheduler before */
void TaskManager::PauseSelf(ContextStack* Registers, uint64_t CoreID){
    /* Update time */
    uint64_t actualTime = HPET::GetTime();
    TimeByCore[CoreID] = actualTime;

    /* Save thread */
    kthread_t* threadEnd = ThreadExecutePerCore[CoreID];

    if(threadEnd != NULL){
        threadEnd->SaveContext(Registers, CoreID);
        threadEnd->IsPause = true;
    }

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

KResult TaskManager::Createthread(kthread_t** self, kprocess_t* proc, void* entryPoint, uint64_t externalData){
    *self = proc->Createthread(entryPoint, externalData);
    return KSUCCESS;
}

KResult TaskManager::Createthread(kthread_t** self, kprocess_t* proc, void* entryPoint, enum Priviledge priviledge, uint64_t externalData){
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
            CurrentData->Task->ShareDataUsingStackSpace(CurrentData->Data->Data, CurrentData->Data->Size, (void**)&DataLocation);
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

        CurrentData->Task->Regs->rsp = StackAlignToJmp(DataLocation);
        
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

KResult TaskManager::Execthread(kthread_t* Caller, kthread_t* Self, execution_type_t Type, arguments_t* FunctionParameters, ThreadShareData_t* Data, ContextStack* Registers){
    if(Self->IsEvent) return KFAIL;

    ThreadQueu_t* queu = Self->Queu;
    AtomicAcquire(&queu->Lock);
    if(Type & ExecutionTypeQueu){
        if(Type & ExecutionTypeAwait){
            queu->SetThreadInQueu_NSU(Caller, Self, FunctionParameters, true, Data);
            AtomicRelease(&queu->Lock);
            Caller->Pause_WL(Registers, true); // We can do this because we have already lock the scheduler
            return KSUCCESS;
        }else{
            queu->SetThreadInQueu(Caller, Self, FunctionParameters, false, Data);
        }
    }else{
        if(Type & ExecutionTypeAwait){
            if(!queu->TasksInQueu){
                queu->SetThreadInQueu_NSU(Caller, Self, FunctionParameters, true, Data);
                AtomicRelease(&queu->Lock);
                Caller->Pause_WL(Registers, true); // We can do this because we have already lock the scheduler
                return KSUCCESS;
            }else{
                AtomicRelease(&queu->Lock);
                return KFAIL;
            }
        }else{
            if(!queu->TasksInQueu){
                queu->SetThreadInQueu(Caller, Self, FunctionParameters, false, Data);
            }else{
                AtomicRelease(&queu->Lock);
                return KFAIL;
            }
        }
    }
    AtomicRelease(&queu->Lock);
    if(Type & ExecutionTypeClose){
        Caller->Close(Registers, Registers->arg0);
    }
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
    AtomicAcquire(&task->Queu->Lock);
    if(task->CloseQueu(ReturnValue) == KSUCCESS){
        AtomicRelease(&task->Queu->Lock);
        ForceSelfDestruction(); /* Unlock MutexScheduler */
        /* noreturn */
        return KSUCCESS;
    }

    globalTaskManager->ReleaseScheduler();

    kprocess_t* ProcParent = task->Parent;
    while(ProcParent){
        AtomicAcquire(&ProcParent->WaitPIDLock);
        for(uint64_t i = 0; i < ProcParent->WaitPIDCount; i++){
            wait_pid_t* WaitPIDInfo = (wait_pid_t*)ProcParent->WaitPIDList->pop64();
            if(WaitPIDInfo->Pid <= 0){
                if(WaitPIDInfo->Pid == -1 || WaitPIDInfo->Pid == 0){
                    WaitPIDInfo->Thread->PIDWaitStatus = static_cast<int>(ReturnValue);
                    WaitPIDInfo->Thread->PIDWaitPIDChild = task->Parent->PID;
                    ProcParent->ProcessChildCount--;
                    Unpause(WaitPIDInfo->Thread);
                }else{
                    if(-(WaitPIDInfo->Pid) == task->Parent->PID){
                        WaitPIDInfo->Thread->PIDWaitStatus = static_cast<int>(ReturnValue);
                        WaitPIDInfo->Thread->PIDWaitPIDChild = task->Parent->PID;
                        ProcParent->ProcessChildCount--;
                        Unpause(WaitPIDInfo->Thread);
                    }
                }
                ProcParent->WaitPIDCount--;
                kfree(WaitPIDInfo);
            }
        }
        AtomicRelease(&ProcParent->WaitPIDLock);
        ProcParent = ProcParent->Parent;
    }


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

KResult TaskManager::CreateProcessWithoutPaging(kprocess_t** key, enum Priviledge priviledge, uint64_t externalData){
    kprocess_t* proc = (kprocess_t*)kcalloc(sizeof(kprocess_t));

    /* Setup default priviledge */
    proc->DefaultPriviledge = priviledge;

    /* Save time */
    proc->CreationTime = HPET::GetTime();

    /* Other data */
    proc->TaskManagerParent = this;
    proc->Locks = (lock_t*)LOCK_ADDRESS;
    proc->LockLimit = SELF_DATA_START_ADDRESS;
    proc->LockIndex = 0;
    proc->ExternalData_P = externalData;

    proc->Parent = NULL;

    proc->StackIterator = STACK_TOP;
    AtomicClearLock(&proc->StackIteratorLock);

    proc->WaitPIDList = KStackInitialize(0x50);
    proc->WaitPIDCount = 0;

    proc->PID = PID; 
    proc->PIDKey = PID; 
    proc->PPID = 0; 
    proc->HaveForkPaging = false; 
    PID++;
    NumberProcessTotal++;

    *key = proc;

    AddProcessList(proc);
    
    return KSUCCESS;
}

KResult TaskManager::CreateProcessWithoutPaging(kthread_t* caller, kprocess_t** key, enum Priviledge priviledge, uint64_t externalData){
    KResult status = CreateProcessWithoutPaging(key, priviledge, externalData);
    kprocess_t* proc = *key;
    proc->PID_PCI = caller->Parent->PID;
    proc->PPID_PCI = caller->Parent->PPID;
    proc->TID_PCI = caller->TID;
    proc->ExternalData_P_PCI = caller->Parent->ExternalData_P;
    proc->Priviledge_PCI = caller->Priviledge;
    return status;
}

KResult TaskManager::CreateProcess(kprocess_t** key, enum Priviledge priviledge, uint64_t externalData){
    KResult status = CreateProcessWithoutPaging(key, priviledge, externalData);
    if(status == KSUCCESS){
        (*key)->SharedPaging = vmm_SetupProcess();
        (*key)->MemoryManager = MMCreateHandler((*key)->SharedPaging, (void*)PAGE_SIZE, FREE_MEMORY_SPACE_ADDRESS - PAGE_SIZE, (void*)PAGE_SIZE, USERSPACE_TOP_ADDRESS - PAGE_SIZE);
        Keyhole_Create(&(*key)->ProcessKey, (*key), (*key), DataTypeProcess, (uint64_t)(*key), DEFAULT_FLAGS_KEY, PriviledgeApp);
    }
    return status;
}

KResult TaskManager::CreateProcess(kthread_t* caller, kprocess_t** key, enum Priviledge priviledge, uint64_t externalData){
    KResult status = CreateProcess(key, priviledge, externalData);
    kprocess_t* proc = *key;
    proc->PID_PCI = caller->Parent->PID;
    proc->PPID_PCI = caller->Parent->PPID;
    proc->TID_PCI = caller->TID;
    proc->ExternalData_P_PCI = caller->Parent->ExternalData_P;
    proc->Priviledge_PCI = caller->Priviledge;
    return status;
}

KResult TaskManager::AddProcessList(kprocess_t* process){
    AtomicAcquire(&CreateProcessLock);
    process->ListIndex = kot_vector_push(ProcessList, process);
    AtomicRelease(&CreateProcessLock);
    return KSUCCESS;
}

KResult TaskManager::RemoveProcessList(kprocess_t* process){
    AtomicAcquire(&CreateProcessLock);
    kot_vector_remove(ProcessList, process->ListIndex);
    AtomicRelease(&CreateProcessLock);
    return KSUCCESS;    
}

kprocess_t* TaskManager::GetProcessList(pid_t pid){
    kprocess_t* Process = NULL;
    AtomicAcquire(&CreateProcessLock);
    for(uint64_t i = 0; i < ProcessList->length; i++){
        kprocess_t* Tmp = (kprocess_t*)kot_vector_get(ProcessList, i);;
        if(Tmp->PID == pid){
            Process = Tmp;
            break;
        }
    }
    AtomicRelease(&CreateProcessLock);
    return Process;    
}

kthread_t* kprocess_t::Createthread(void* entryPoint, uint64_t externalData){
    return Createthread(entryPoint, DefaultPriviledge, externalData);
}

kthread_t* kprocess_t::Createthread(void* entryPoint, enum Priviledge priviledge, uint64_t externalData){
    kthread_t* thread = (kthread_t*)kcalloc(sizeof(kthread_t));

    AtomicAcquire(&CreateThreadLocker);
    if(Childs == NULL){
        Childs = CreateNode((void*)0);
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

    thread->Parent = this;

    /* Load new stack */
    if(thread->SetupStack() != KSUCCESS){
        kfree(thread->SIMDSaver);
        kfree(thread->Info);
        kfree(thread->Regs);
        kfree(thread);
        return NULL;
    }

    /* Other data */
    thread->CreationTime = HPET::GetTime();
    thread->MemoryAllocated = 0;
    thread->TimeAllocate = 0;
    thread->IsBlock = true;
    thread->IsClose = true;
    thread->IsFork = false;
    thread->SignalMask = 0;
    thread->Queu = (ThreadQueu_t*)kcalloc(sizeof(ThreadQueu_t));
    thread->ExternalData_T = externalData;

    /* Thread Data */
    void* threadDataPA = Pmm_RequestPage();
    thread->MemoryAllocated += PAGE_SIZE;
    thread->threadData = (SelfData*)vmm_GetVirtualAddress(threadDataPA);
    
    Keyhole_Create(&thread->threadData->ThreadKey, this, this, DataTypeThread, (uint64_t)thread, DEFAULT_FLAGS_KEY, PriviledgeApp);
    vmm_Map(thread->Paging, (void*)SELF_DATA_START_ADDRESS, threadDataPA, thread->RingPL == UserAppRing);
    
    /* ID */
    thread->TID = this->TID; 
    TID++;
    NumberOfthread++;
    
    thread->threadData->ProcessKey = this->ProcessKey;
    thread->threadData->PID = this->PID;
    thread->threadData->PPID = this->PPID;
    thread->threadData->TID = thread->TID;
    thread->threadData->ExternalData_T = thread->ExternalData_T;
    thread->threadData->ExternalData_P = this->ExternalData_P;
    thread->threadData->Priviledge = thread->Priviledge;
    thread->threadData->PID_PCI = this->PID_PCI;
    thread->threadData->PPID_PCI = this->PPID_PCI;
    thread->threadData->TID_PCI = this->TID_PCI;
    thread->threadData->ExternalData_P_PCI = this->ExternalData_P_PCI;
    thread->threadData->Priviledge_PCI = this->Priviledge_PCI;


    return thread;
}

kthread_t* kprocess_t::Duplicatethread(kthread_t* source){
    kthread_t* thread = (kthread_t*)kcalloc(sizeof(kthread_t));

    AtomicAcquire(&CreateThreadLocker);
    if(Childs == NULL){
        Childs = CreateNode((void*)0);
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

    thread->Parent = this;

    /* Load new stack */
    if(thread->SetupStack() != KSUCCESS){
        kfree(thread->SIMDSaver);
        kfree(thread->Info);
        kfree(thread->Regs);
        kfree(thread);
        return NULL;
    }

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
    thread->IsFork = false;
    thread->SignalMask = 0;
    thread->Queu = (ThreadQueu_t*)kcalloc(sizeof(ThreadQueu_t)); 

    /* Thread Data */
    void* threadDataPA = Pmm_RequestPage();
    thread->MemoryAllocated += PAGE_SIZE;
    thread->threadData = (SelfData*)vmm_GetVirtualAddress(threadDataPA);
    
    Keyhole_Create(&thread->threadData->ThreadKey, this, this, DataTypeThread, (uint64_t)thread, DEFAULT_FLAGS_KEY, PriviledgeApp);
    vmm_Map(thread->Paging, (void*)SELF_DATA_START_ADDRESS, threadDataPA, thread->RingPL == UserAppRing);
    
    thread->threadData->ProcessKey = ProcessKey;
    thread->threadData->PID = PID;
    thread->threadData->PPID = PPID;
    thread->threadData->TID = TID;
    thread->threadData->ExternalData_P = ExternalData_P;
    thread->threadData->Priviledge = thread->Priviledge;
    thread->threadData->PID_PCI = PID_PCI;
    thread->threadData->PPID_PCI = PPID_PCI;
    thread->threadData->TID_PCI = TID_PCI;
    thread->threadData->ExternalData_P_PCI = ExternalData_P_PCI;
    thread->threadData->Priviledge_PCI = Priviledge_PCI;
    
    /* ID */
    thread->TID = TID; 
    TID++;
    NumberOfthread++;

    return thread;
}

KResult kprocess_t::Fork(ContextStack* Registers, kthread_t* Caller, kprocess_t** Child, kthread_t** ChildThread){ 
    *Child = (kprocess_t*)kcalloc(sizeof(kprocess_t));

    AtomicAcquire(&StackIteratorLock);
    (*Child)->StackIterator = StackIterator;
    AtomicClearLock(&(*Child)->StackIteratorLock);
    AtomicRelease(&StackIteratorLock);

    /* Setup default priviledge */
    (*Child)->DefaultPriviledge = DefaultPriviledge;

    /* Save time */
    (*Child)->CreationTime = HPET::GetTime();

    /* Other data */
    (*Child)->TaskManagerParent = TaskManagerParent;
    (*Child)->Locks = (lock_t*)LOCK_ADDRESS;
    (*Child)->LockLimit = SELF_DATA_START_ADDRESS;
    (*Child)->LockIndex = 0;
    (*Child)->ExternalData_P = ExternalData_P;
    (*Child)->PID = TaskManagerParent->PID; 
    TaskManagerParent->PID++;

    (*Child)->PPID = this->PID;
    (*Child)->PIDKey = this->PIDKey;
    TaskManagerParent->NumberProcessTotal++;

    (*Child)->PID_PCI = PID;
    (*Child)->PPID_PCI = PPID;
    (*Child)->TID_PCI = Caller->TID;
    (*Child)->ExternalData_P_PCI = ExternalData_P;
    (*Child)->Priviledge_PCI = Caller->Priviledge;

    (*Child)->Parent = this;

    kprocess_t* ProcParent = this;
    while(ProcParent->Parent){
        ProcParent = ProcParent->Parent;
    }
    ProcParent->ProcessChildCount++;

    vmm_ForkMemory(this, *Child);
    (*Child)->MemoryManager = MMCloneHandler((*Child)->SharedPaging, this->MemoryManager);

    TaskManagerParent->AddProcessList(*Child);

    Keyhole_Create(&(*Child)->ProcessKey, (*Child), (*Child), DataTypeProcess, (uint64_t)(*Child), DEFAULT_FLAGS_KEY, PriviledgeApp);
    
    *ChildThread = Caller->ForkThread(Registers, *Child);

    return KSUCCESS;
}

KResult kthread_t::SetupStack(){
    AtomicAcquire(&Parent->StackIteratorLock);
    if(Parent->StackIterator - APP_STACK_MAX_SIZE < SELF_DATA_END_ADDRESS){
        AtomicRelease(&Parent->StackIteratorLock);
        return KFAIL;
    } 
    uint64_t StackLocation = Parent->StackIterator;
    Parent->StackIterator -= APP_STACK_MAX_SIZE;
    AtomicRelease(&Parent->StackIteratorLock);
    this->Regs->rsp = StackAlignToJmp(StackLocation);
    this->Stack = (StackInfo*)kmalloc(sizeof(StackInfo));
    this->Stack->StackStart = StackLocation;
    this->Stack->StackEndMax = StackLocation - APP_STACK_MAX_SIZE;
    this->Stack->LastStackUsed = StackLocation;
    
    this->KernelInternalStack = (void*)((uint64_t)stackalloc(KERNEL_STACK_SIZE) + KERNEL_STACK_SIZE);

    this->Info->SyscallStack = (uint64_t)stackalloc(KERNEL_STACK_SIZE) + KERNEL_STACK_SIZE;
    return KSUCCESS;
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
void TaskManager::InitScheduler(uint8_t NumberOfCores, void* IddleTaskFunction){
    void* physcialMemory = Pmm_RequestPage();
    void* virtualMemory = (void*)vmm_GetVirtualAddress(physcialMemory);
    vmm_Map(vmm_PageTable, virtualMemory, physcialMemory, true, false);
    memcpy(virtualMemory, IddleTaskFunction, PAGE_SIZE);

    ProcessList = kot_vector_create();

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

        CPU::SetCPUGSKernelBase((uint64_t)SELF_DATA_START_ADDRESS); // keys position

        SyscallEnable(GDTInfoSelectorsRing[KernelRing].Code, GDTInfoSelectorsRing[UserAppRing].Code); 

        IsSchedulerEnable[CoreID] = true;
    }
}

kthread_t* TaskManager::GetCurrentthread(uint64_t CoreID){
    return ThreadExecutePerCore[CoreID];
}

void TaskManager::AcquireScheduler(){
    AtomicAcquireCli(&SchedulerLock);
}

void TaskManager::ReleaseScheduler(){
    AtomicRelease(&SchedulerLock);
    CPU::EnableInterrupts();
}

void TaskManager::PauseSelf(){
    AcquireScheduler();
    ForceSelfPause();
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
    CPU::SetCPUFSBase((uint64_t)FSBase);
    TSSSetIST(CoreID, IST_Interrupts, (uint64_t)KernelInternalStack);
}

void kthread_t::ResetContext(ContextStack* Registers){
    Registers->rsp = StackAlignToJmp((uint64_t)Stack->StackStart);
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

bool kthread_t::PageFaultHandler(bool IsWriting, uint64_t Address){
    if(ExtendStack(Address)){
       return true; 
    }else if(IsWriting){
        if(Parent->HaveForkPaging){
            return vmm_MapFork(Paging, Address);
        }else{
            return false;
        }
    }else{
        return false;
    }
}

bool kthread_t::ExtendStack(uint64_t address){
    if(this->Stack == NULL) return false;

    address -= address % PAGE_SIZE;
    if(this->Stack->StackStart <= address) return false;
    if(address <= this->Stack->StackEndMax) return false;
    
    if(address < this->Stack->LastStackUsed){
        this->Stack->LastStackUsed = address;
    }

    if(!vmm_GetFlags(Paging, (void*)address, vmm_Present)){
        void* PhysicalAddress = Pmm_RequestPage();
        MemoryAllocated += PAGE_SIZE;
        vmm_Map(Paging, (void*)address, PhysicalAddress, true, true, true);
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

    if(address < this->Stack->LastStackUsed){
        this->Stack->LastStackUsed = address;
    }
    
    uint64_t pageCount = DivideRoundUp(size, PAGE_SIZE);
    
    for(uint64_t i = 0; i < pageCount; i++){
        if(!vmm_GetFlags(Paging, (void*)(address + i * PAGE_SIZE), vmm_Present)){
            void* PhysicalAddress = Pmm_RequestPage();
            MemoryAllocated += PAGE_SIZE;
            vmm_Map(Paging, (void*)(address + i * PAGE_SIZE), PhysicalAddress, true, true, true);
        }        
    }

    return true;
}

KResult kthread_t::ShareDataUsingStackSpace(void* data, size64_t size, void** location){
    void* address = (void*)((uint64_t)*location - size);

    uint64_t RoundAddress = (uint64_t)address;

    RoundAddress -= RoundAddress % PAGE_SIZE;

    if((uint64_t)address < this->Stack->LastStackUsed){
        this->Stack->LastStackUsed = (uint64_t)address;
    }

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

        void* physicalPage = NULL;
        if(!vmm_GetFlags(Paging, (void*)virtualAddressIterator, vmm_Present)){
            physicalPage = Pmm_RequestPage();
            MemoryAllocated += PAGE_SIZE;
            vmm_Map(Paging, (void*)((uint64_t)virtualAddressIterator), physicalPage, true, true, true);
            physicalPage = (void*)((uint64_t)physicalPage + alignement);
        }else{
            physicalPage = vmm_GetPhysical(Paging, (void*)virtualAddressIterator);
        }
        memcpy((void*)(vmm_GetVirtualAddress(physicalPage)), (void*)virtualAddressParentIterator, sizeToCopy);

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
    
        void* physicalPage = NULL;
        if(!vmm_GetFlags(Paging, (void*)virtualAddressIterator, vmm_Present)){
            physicalPage = Pmm_RequestPage();
            MemoryAllocated += PAGE_SIZE;
            vmm_Map(Paging, (void*)((uint64_t)virtualAddressIterator), physicalPage, true, true, true);
        }else{
            physicalPage = vmm_GetPhysical(Paging, (void*)virtualAddressIterator);
        }
        memcpy((void*)vmm_GetVirtualAddress(physicalPage), (void*)virtualAddressParentIterator, sizeToCopy);
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

    AtomicAcquire(&Queu->Lock);
    
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
    if(IsFork){
        // Forked thread doesn't have queu
        IsBlock = true;
        IsClose = true;
        IsPause = false;
        globalTaskManager->AcquireScheduler();
        return KFAIL;
    }

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

kthread_t* kthread_t::ForkThread(ContextStack* Registers, kprocess_t* Child){
    kthread_t* ChildThread = Child->Createthread(this->EntryPoint, this->Priviledge, this->ExternalData_T);
    ChildThread->IsFork = true;

    memcpy(ChildThread->Stack, this->Stack, sizeof(StackInfo));

    // Copy stack and context
    uint64_t DataLocation = ChildThread->Stack->StackStart;
    ChildThread->SaveContext(Registers);
    ChildThread->FSBase = FSBase;

    // Set the correct threadInfo
    ChildThread->Regs->cr3 = (uint64_t)ChildThread->Paging;
    ChildThread->Regs->threadInfo = ChildThread->Info;

    // Set different results for parents and childs
    ChildThread->Regs->GlobalPurpose = 0;

    // Start child thread
    ChildThread->IsClose = false;
    ChildThread->IsBlock = false;
    Child->TaskManagerParent->EnqueueTask(ChildThread);

    return ChildThread;
}

void SetParameters(ContextStack* Registers, arguments_t* FunctionParameters){
    Registers->arg0 = FunctionParameters->arg[0];
    Registers->arg1 = FunctionParameters->arg[1];
    Registers->arg2 = FunctionParameters->arg[2];
    Registers->arg3 = FunctionParameters->arg[3];
    Registers->arg4 = FunctionParameters->arg[4];
    Registers->arg5 = FunctionParameters->arg[5];    
}