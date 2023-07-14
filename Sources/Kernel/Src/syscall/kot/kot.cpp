#include <syscall/kot/kot.h>
#include <abi-bits/errno.h>
#include <abi-bits/vm-flags.h>


/* -------------------------------Specs--------------------------------------- */

/* Sys_Kot_Function :
    Arguments : 
    0 -> description            > type
    1 -> description            > type
    2 -> description            > type
    3 -> description            > type
    4 -> description            > type
    5 -> description            > type
*/

/* -------------------------------Functions------------------------------------ */

/* Sys_Kot_CreateMemoryField :
    Arguments : 
    0 -> process taget                  > key process
    1 -> size                           > uint64_t
    2 -> pointer to virtual address     > uint64_t*
    3 -> return                         > key shared memory
    4 -> reserved                       > none
    5 -> reserved                       > none
*/
KResult Sys_Kot_CreateMemoryField(SyscallStack* Registers, kthread_t* Thread){
    kprocess_t* processkey;
    uint64_t flags;
    uint64_t data;
    if(Keyhole_Get(Thread, (key_t)SYSCALL_ARG0(Registers), DataTypeProcess, (uint64_t*)&processkey, &flags) != KSUCCESS) return KKEYVIOLATION;
    if(!(flags & KeyholeFlagDataTypeProcessMemoryAccessible)) return KKEYVIOLATION;
    if(MMCreateMemoryField(Thread, processkey, SYSCALL_ARG1(Registers), (uint64_t*)SYSCALL_ARG2(Registers), &data, (enum MemoryFieldType)SYSCALL_ARG4(Registers)) != KSUCCESS) return KFAIL;
    return Keyhole_Create((key_t*)SYSCALL_ARG3(Registers), Thread->Parent, NULL, DataTypeSharedMemory, data, KeyholeFlagFullPermissions, PriviledgeApp);
}

/* Sys_Kot_AcceptMemoryField :
    Arguments : 
*/
KResult Sys_Kot_AcceptMemoryField(SyscallStack* Registers, kthread_t* Thread){
    kprocess_t* processkey;
    MemoryShareInfo* memoryKey;
    uint64_t flags;

    if(CheckUserAddress((void*)SYSCALL_ARG2(Registers), sizeof(uint64_t)) != KSUCCESS) return KMEMORYVIOLATION;
    uint64_t* virtualAddressPointer = (uint64_t*)SYSCALL_ARG2(Registers);

    if(Keyhole_Get(Thread, (key_t)SYSCALL_ARG0(Registers), DataTypeProcess, (uint64_t*)&processkey, &flags) != KSUCCESS) return KKEYVIOLATION;
    if(!(flags & KeyholeFlagDataTypeProcessMemoryAccessible)) return KKEYVIOLATION;
    if(Keyhole_Get(Thread, (key_t)SYSCALL_ARG1(Registers), DataTypeSharedMemory, (uint64_t*)&memoryKey, &flags) != KSUCCESS) return KKEYVIOLATION;
    return MMAcceptMemoryField(Thread, processkey, memoryKey, virtualAddressPointer);
}

/* Sys_Kot_CloseMemoryField :
    Arguments : 
*/
KResult Sys_Kot_CloseMemoryField(SyscallStack* Registers, kthread_t* Thread){
    kprocess_t* processkey;
    MemoryShareInfo* memoryKey;
    uint64_t flags;
    if(Keyhole_Get(Thread, (key_t)SYSCALL_ARG0(Registers), DataTypeProcess, (uint64_t*)&processkey, &flags) != KSUCCESS) return KKEYVIOLATION;
    if(!(flags & KeyholeFlagDataTypeProcessMemoryAccessible)) return KKEYVIOLATION;
    if(Keyhole_Get(Thread, (key_t)SYSCALL_ARG1(Registers), DataTypeSharedMemory, (uint64_t*)&memoryKey, &flags) != KSUCCESS) return KKEYVIOLATION;
    return MMCloseMemoryField(Thread, processkey, memoryKey, (void*)SYSCALL_ARG2(Registers));    
}

/* Sys_Kot_GetInfoMemoryField :
    Arguments : 
*/
KResult Sys_Kot_GetInfoMemoryField(SyscallStack* Registers, kthread_t* Thread){
    MemoryShareInfo* memoryKey;
    uint64_t flags;

    if(Keyhole_Get(Thread, (key_t)SYSCALL_ARG0(Registers), DataTypeSharedMemory, (uint64_t*)&memoryKey, &flags) != KSUCCESS) return KKEYVIOLATION;
    if(CheckUserAddress((void*)SYSCALL_ARG1(Registers), sizeof(uint64_t)) == KSUCCESS){
        uint64_t* TypePointer = (uint64_t*)SYSCALL_ARG1(Registers);
        *TypePointer = (uint64_t)memoryKey->Type;
    }

    if(CheckUserAddress((void*)SYSCALL_ARG2(Registers), sizeof(uint64_t)) == KSUCCESS){
        size64_t* SizePointer = (size64_t*)SYSCALL_ARG2(Registers);
        *SizePointer = (uint64_t)memoryKey->InitialSize;
    }
    return KSUCCESS;
}

/* Sys_Kot_CreateProc :
    Arguments : 
*/
KResult Sys_Kot_CreateProc(SyscallStack* Registers, kthread_t* Thread){
    kprocess_t* data;
    if(SYSCALL_ARG4(Registers) > PriviledgeApp){
        SYSCALL_ARG4(Registers) = PriviledgeApp;
    }
    if(CheckUserAddress((void*)SYSCALL_ARG0(Registers), sizeof(key_t)) != KSUCCESS) return KMEMORYVIOLATION;
    if(globalTaskManager->CreateProcess(&data, (enum Priviledge)SYSCALL_ARG1(Registers), SYSCALL_ARG2(Registers)) != KSUCCESS) return KFAIL;
    return Keyhole_Create((key_t*)SYSCALL_ARG0(Registers), data, Thread->Parent, DataTypeProcess, (uint64_t)data, KeyholeFlagFullPermissions, PriviledgeApp);
}

/* Sys_Kot_Fork :
    Arguments : 
*/
KResult Sys_Kot_Fork(SyscallStack* Registers, kthread_t* Thread){
    uint64_t flags;
    if(CheckUserAddress((void*)SYSCALL_ARG0(Registers), sizeof(key_t)) != KSUCCESS) return KMEMORYVIOLATION;
    kprocess_t* Child;
    kthread_t* ChildThread;
    kprocess_t* Parent = Thread->Parent;

    Thread->Parent->Fork((ContextStack*)Registers, Thread, &Child, &ChildThread);
    
    return Child->PID;
}

/* Sys_Kot_CloseProc :
    Arguments : 
*/
KResult Sys_Kot_CloseProc(SyscallStack* Registers, kthread_t* Thread){
    //TODO
    return KFAIL;
}

/* Sys_Kot_Close :
    Arguments : 
*/
KResult Sys_Kot_Close(SyscallStack* Registers, kthread_t* Thread){
    if(Thread->IsEvent){
        return KFAIL;
    }else{
        return Thread->Close((ContextStack*)Registers, SYSCALL_ARG0(Registers));
    }
}

/* Sys_Kot_Exit :
    Arguments : 
*/
KResult Sys_Kot_Exit(SyscallStack* Registers, kthread_t* Thread){
    return globalTaskManager->Exit((ContextStack*)Registers, Thread, SYSCALL_ARG0(Registers));
}

/* Sys_Kot_Pause :
    Arguments : 
*/
KResult Sys_Kot_Pause(SyscallStack* Registers, kthread_t* Thread){
    return Thread->Pause((ContextStack*)Registers, SYSCALL_ARG0(Registers));
    /* No return */
}

/* Sys_Kot_UnPause :
    Arguments : 
*/
KResult Sys_Kot_UnPause(SyscallStack* Registers, kthread_t* Thread){
    kthread_t* threadkey;
    uint64_t flags;
    if(Keyhole_Get(Thread, (key_t)SYSCALL_ARG0(Registers), DataTypeThread, (uint64_t*)&threadkey, &flags) != KSUCCESS) return KKEYVIOLATION;
    if(!(flags & KeyholeFlagDataTypeThreadIsUnpauseable)) return KKEYVIOLATION;
    return globalTaskManager->Unpause(threadkey);
}

/* Sys_Kot_Map :
    Arguments : 
    0 -> process            > key
    1 -> virtual address    > uint64_t*
    2 -> physicall or not   > bool
    3 -> physical address   > void*
    4 -> size               > size64_t
    5 -> find free address  > bool
*/
KResult Sys_Kot_Map(SyscallStack* Registers, kthread_t* Thread){
    // Check priviledge
    if(SYSCALL_ARG2(Registers) > AllocationTypeBasic && Thread->Priviledge != PriviledgeDriver){
        return KNOTALLOW;
    } 
    kprocess_t* processkey;
    uint64_t flags;
    if(Keyhole_Get(Thread, (key_t)SYSCALL_ARG0(Registers), DataTypeProcess, (uint64_t*)&processkey, &flags) != KSUCCESS){
        return KKEYVIOLATION;
    } 
    if(!(flags & KeyholeFlagDataTypeProcessMemoryAccessible)){
       return KKEYVIOLATION; 
    } 
    
    /* Get arguments */
    uint64_t* AddressVirtual = (uint64_t*)SYSCALL_ARG1(Registers);
    enum AllocationType Type = (enum AllocationType)SYSCALL_ARG2(Registers);
    void** AddressPhysical = (void**)SYSCALL_ARG3(Registers);
    size_t* Size = (size_t*)SYSCALL_ARG4(Registers);
    bool IsNeedToBeFree = (bool)SYSCALL_ARG5(Registers); 

    if(!CheckUserAddress((void*)AddressVirtual, sizeof(uint64_t))){
        return KMEMORYVIOLATION;
    }

    *AddressVirtual = *AddressVirtual - ((uint64_t)*AddressVirtual % PAGE_SIZE);

    bool IsPhysicalAddress = false;
    if(CheckUserAddress((void*)AddressPhysical, sizeof(void*))){
        IsPhysicalAddress = true;
    }else{
        if(Type == AllocationTypePhysical){
            return KMEMORYVIOLATION;
        }
    }

    if(!CheckUserAddress((void*)Size, sizeof(size_t))){
        return KMEMORYVIOLATION;
    } 

    if(*Size % PAGE_SIZE){
        *Size -= *Size % PAGE_SIZE;
        *Size += PAGE_SIZE;
    }

    if(*AddressVirtual % PAGE_SIZE){
        *AddressVirtual -= *AddressVirtual % PAGE_SIZE;
    }

    int Errno = 0;
    size_t SizeResult = *Size;
    void* BaseResult = NULL;
    
    KResult Status = MMAllocateRegionVM(Thread->Parent->MemoryManager, (void*)*AddressVirtual, *Size, !IsNeedToBeFree, &BaseResult);

    if(Status != KSUCCESS){
        *Size = 0;
        return Status;
    }

    switch(Type){
        case AllocationTypePhysicalContiguous:
            Status = MMAllocateMemoryContigous(Thread->Parent->MemoryManager, BaseResult, SizeResult, PROT_READ | PROT_WRITE | PROT_EXEC, Size);
            break;
        case AllocationTypePhysical:
            Status = MMMapPhysical(Thread->Parent->MemoryManager, (void*)*AddressPhysical, BaseResult, SizeResult, PROT_READ | PROT_WRITE | PROT_EXEC);
            break;
        default:
            Status = MMAllocateMemoryBlock(Thread->Parent->MemoryManager, BaseResult, SizeResult, PROT_READ | PROT_WRITE | PROT_EXEC, Size);
            break;
    }

    if(Type == AllocationTypePhysicalContiguous && IsPhysicalAddress){
        *AddressPhysical = vmm_GetPhysical(Thread->Parent->SharedPaging, BaseResult);
    }

    return Status;
}

/* Sys_Kot_Unmap :
    Arguments : 
    0 -> process            > key
    1 -> virtual address    > uint64_t*
    4 -> size               > size64_t
*/
KResult Sys_Kot_Unmap(SyscallStack* Registers, kthread_t* Thread){
    kprocess_t* processkey;
    uint64_t flags;
    if(Keyhole_Get(Thread, (key_t)SYSCALL_ARG0(Registers), DataTypeProcess, (uint64_t*)&processkey, &flags) != KSUCCESS) return KKEYVIOLATION;
    if(!(flags & KeyholeFlagDataTypeProcessMemoryAccessible)) return KKEYVIOLATION;
    pagetable_t pageTable = processkey->SharedPaging;
    void* addressVirtual = (void*)SYSCALL_ARG1(Registers);
    size64_t size = SYSCALL_ARG2(Registers);

    addressVirtual = (void*)((uint64_t)addressVirtual - (uint64_t)addressVirtual % PAGE_SIZE);
    uint64_t pageCount = DivideRoundUp(size, PAGE_SIZE);
    if((uint64_t)addressVirtual + pageCount * PAGE_SIZE < vmm_HHDMAdress){
        for(uint64_t i = 0; i < pageCount; i += PAGE_SIZE){
            if(vmm_GetFlags(pageTable, (void*)addressVirtual, vmm_flag::vmm_Master)){
                if(vmm_GetFlags(pageTable, (void*)addressVirtual, vmm_flag::vmm_IsPureMemory)){
                    Pmm_FreePage(vmm_GetPhysical(pageTable, addressVirtual));
                    processkey->MemoryAllocated -= PAGE_SIZE;
                }
                vmm_Unmap(pageTable, addressVirtual);
            }
        }
    }
    return KSUCCESS;
}

/* Sys_Kot_GetPhysical :
    Arguments : 
*/
KResult Sys_Kot_GetPhysical(SyscallStack* Registers, kthread_t* Thread){
    if(SYSCALL_ARG0(Registers) < vmm_HHDMAdress){
        return (KResult)vmm_GetPhysical(Thread->Paging, (void*)SYSCALL_ARG0(Registers));
    }else{
        return NULL;
    }
}

/* Sys_Kot_Event_Create :
    Arguments : 
*/
KResult Sys_Kot_Event_Create(SyscallStack* Registers, kthread_t* Thread){
    uint64_t data;
    if(Event::Create((kevent_t**)&data, EventTypeIPC, SYSCALL_ARG0(Registers)) != KSUCCESS) return KFAIL;
    return Keyhole_Create((key_t*)SYSCALL_ARG0(Registers), Thread->Parent, Thread->Parent, DataTypeEvent, data, KeyholeFlagFullPermissions, PriviledgeApp);
}

/* Sys_Kot_Event_Bind :
    Arguments : 
*/
KResult Sys_Kot_Event_Bind(SyscallStack* Registers, kthread_t* Thread){
    kevent_t* event; 
    kthread_t* threadkey;
    uint64_t flags;
    if(Keyhole_Get(Thread, (key_t)SYSCALL_ARG0(Registers), DataTypeEvent, (uint64_t*)&event, &flags) != KSUCCESS) return KKEYVIOLATION;
    if(Keyhole_Get(Thread, (key_t)SYSCALL_ARG1(Registers), DataTypeThread, (uint64_t*)&threadkey, &flags) != KSUCCESS) return KKEYVIOLATION;
    if(!(flags & KeyholeFlagDataTypeThreadIsEventable)) return KKEYVIOLATION;
    return Event::Bind(threadkey, event, (bool)SYSCALL_ARG2(Registers));
}

/* Sys_Kot_Event_Unbind :
    Arguments : 
*/
KResult Sys_Kot_Event_Unbind(SyscallStack* Registers, kthread_t* Thread){
    kevent_t* event;
    kthread_t* threadkey;
    uint64_t flags;
    if(Keyhole_Get(Thread, (key_t)SYSCALL_ARG0(Registers), DataTypeEvent, (uint64_t*)&event, &flags) != KSUCCESS) return KKEYVIOLATION;
    if(Keyhole_Get(Thread, (key_t)SYSCALL_ARG1(Registers), DataTypeThread, (uint64_t*)&threadkey, &flags) != KSUCCESS) return KKEYVIOLATION;
    if(!(flags & KeyholeFlagDataTypeEventIsBindable)) return KKEYVIOLATION;
    if(!(flags & KeyholeFlagDataTypeThreadIsEventable)) return KKEYVIOLATION;
    return Event::Unbind(Thread, event);
}

/* Sys_Kot_Event_Trigger :
    Arguments : 
*/
KResult Sys_Kot_Event_Trigger(SyscallStack* Registers, kthread_t* Thread){
    kevent_t* event; 
    uint64_t flags;
    if(Keyhole_Get(Thread, (key_t)SYSCALL_ARG0(Registers), DataTypeEvent, (uint64_t*)&event, &flags) != KSUCCESS) return KKEYVIOLATION;
    if(!(flags & KeyholeFlagDataTypeEventIsTriggerable)) return KKEYVIOLATION;
    if(!CheckUserAddress((void*)SYSCALL_ARG1(Registers), sizeof(arguments_t))) return KMEMORYVIOLATION;
    return Event::Trigger(event, (arguments_t*)SYSCALL_ARG1(Registers));
}

/* Sys_Kot_Event_Close :
    Arguments : 
*/
KResult Sys_Kot_Event_Close(SyscallStack* Registers, kthread_t* Thread){
    if(!Thread->IsEvent) return KFAIL;
    return Event::Close((ContextStack*)Registers, Thread);
}

/* Sys_Kot_CreateThread :
    Arguments : 
*/
KResult Sys_Kot_CreateThread(SyscallStack* Registers, kthread_t* Thread){
    kprocess_t* processkey;
    uint64_t flags;
    kthread_t* threadData;
    if(Keyhole_Get(Thread, (key_t)SYSCALL_ARG0(Registers), DataTypeProcess, (uint64_t*)&processkey, &flags) != KSUCCESS) return KKEYVIOLATION;
    if(!(flags & KeyholeFlagDataTypeProcessIsThreadCreateable)) return KKEYVIOLATION;
    if(SYSCALL_ARG2(Registers) > PriviledgeApp){
        SYSCALL_ARG2(Registers) = Thread->Parent->DefaultPriviledge;
    }
    if(globalTaskManager->Createthread(&threadData, processkey, (void*)SYSCALL_ARG1(Registers), (enum Priviledge)SYSCALL_ARG2(Registers), SYSCALL_ARG3(Registers)) != KSUCCESS) return KFAIL;
    return Keyhole_Create((key_t*)SYSCALL_ARG4(Registers), Thread->Parent, Thread->Parent, DataTypeThread, (uint64_t)threadData, KeyholeFlagFullPermissions, PriviledgeApp);
}

/* Sys_Kot_Duplicatethread :
    Arguments : 
*/
KResult Sys_Kot_DuplicateThread(SyscallStack* Registers, kthread_t* Thread){
    kprocess_t* processkey;
    kthread_t* threadkey;
    uint64_t flags;
    if(Keyhole_Get(Thread, (key_t)SYSCALL_ARG0(Registers), DataTypeProcess, (uint64_t*)&processkey, &flags) != KSUCCESS) return KKEYVIOLATION;
    if(!(flags & KeyholeFlagDataTypeProcessIsThreadCreateable)) return KKEYVIOLATION;
    if(Keyhole_Get(Thread, (key_t)SYSCALL_ARG1(Registers), DataTypeThread, (uint64_t*)&threadkey, &flags) != KSUCCESS) return KKEYVIOLATION;
    if(!(flags & KeyholeFlagDataTypeThreadIsDuplicable)) return KKEYVIOLATION;
    if(globalTaskManager->Duplicatethread(&Thread, processkey, threadkey) != KSUCCESS) return KFAIL;     
    return Keyhole_Create((key_t*)SYSCALL_ARG2(Registers), Thread->Parent, Thread->Parent, DataTypeThread, (uint64_t)Thread, KeyholeFlagFullPermissions, PriviledgeApp);
}

/* Sys_Kot_ExecThread :
    Arguments : 
*/
KResult Sys_Kot_ExecThread(SyscallStack* Registers, kthread_t* Thread){
    kthread_t* threadkey;
    uint64_t flags;
    if(Keyhole_Get(Thread, (key_t)SYSCALL_ARG0(Registers), DataTypeThread, (uint64_t*)&threadkey, &flags) != KSUCCESS) return KKEYVIOLATION;
    execution_type_t Type = (execution_type_t)SYSCALL_ARG2(Registers);
    if(Type & ExecutionTypeQueu || Type & ExecutionTypeQueu){
        if(!(flags & KeyholeFlagDataTypeThreadIsExecutableWithQueue)){
            return KKEYVIOLATION;
        }
    }
    if(Type & ExecutionTypeOneshot || Type & ExecutionTypeOneshot){
        if(!(flags & KeyholeFlagDataTypeThreadIsExecutableOneshot)){
            return KKEYVIOLATION;
        }
    }

    if(SYSCALL_ARG1(Registers) != NULL){
        if(CheckUserAddress((void*)SYSCALL_ARG1(Registers), sizeof(arguments_t)) != KSUCCESS) return KMEMORYVIOLATION;    
    }
    ThreadShareData_t* Data = (ThreadShareData_t*)SYSCALL_ARG3(Registers);
    if(Data != NULL){
        if(CheckUserAddress((void*)Data, sizeof(ThreadShareData_t)) != KSUCCESS) return KMEMORYVIOLATION;    
        if(CheckUserAddress((void*)Data->Data, Data->Size) != KSUCCESS) return KMEMORYVIOLATION;    
    }
    KResult Status = globalTaskManager->Execthread(Thread, threadkey, Type, (arguments_t*)SYSCALL_ARG1(Registers), Data, (ContextStack*)Registers);
    return Status;
}

/* Sys_Kot_Keyhole_CloneModify :
    Arguments : 
    0 -> string             > char*
    1 -> size               > size64_t
*/
KResult Sys_Kot_Keyhole_CloneModify(SyscallStack* Registers, kthread_t* Thread){
    kprocess_t* processkey;
    uint64_t flags;
    if(SYSCALL_ARG2(Registers) != NULL){
        if(Keyhole_Get(Thread, (key_t)SYSCALL_ARG2(Registers), DataTypeProcess, (uint64_t*)&processkey, &flags) != KSUCCESS) return KKEYVIOLATION;     
    }else{
        processkey = NULL;
    }
    if(CheckUserAddress((void*)SYSCALL_ARG1(Registers), sizeof(uint64_t)) != KSUCCESS) return KMEMORYVIOLATION;
    if(SYSCALL_ARG4(Registers) > PriviledgeApp){
        SYSCALL_ARG4(Registers) = PriviledgeApp;
    }
    return Keyhole_CloneModify(Thread, (key_t)SYSCALL_ARG0(Registers), (key_t*)SYSCALL_ARG1(Registers), processkey, SYSCALL_ARG3(Registers), (enum Priviledge)SYSCALL_ARG4(Registers));
}

/* Sys_Kot_Keyhole_Verify :
    Arguments : 

*/
KResult Sys_Kot_Keyhole_Verify(SyscallStack* Registers, kthread_t* Thread){
    if(CheckUserAddress((void*)SYSCALL_ARG2(Registers), sizeof(uint64_t)) != KSUCCESS) return KMEMORYVIOLATION;
    if(CheckUserAddress((void*)SYSCALL_ARG3(Registers), sizeof(uint64_t)) != KSUCCESS) return KMEMORYVIOLATION;
    if(CheckUserAddress((void*)SYSCALL_ARG4(Registers), sizeof(uint64_t)) != KSUCCESS) return KMEMORYVIOLATION;
    key_t key = SYSCALL_ARG0(Registers);
    uint64_t Status = Keyhole_Verify(Thread, key, (enum DataType)SYSCALL_ARG1(Registers));
    if(Status != KSUCCESS) return Status;
    lock_t* lock = (lock_t*)key;
    uint64_t* target = (uint64_t*)SYSCALL_ARG2(Registers);
    uint64_t* flags = (uint64_t*)SYSCALL_ARG3(Registers);
    uint64_t* minpriviledge = (uint64_t*)SYSCALL_ARG4(Registers);
    if(lock->Target == NULL){
        *target = NULL;
    }else{
        *target = lock->Target->ProcessKey;
    }
    *flags = lock->Flags;
    *minpriviledge = lock->MinPriviledge;
    return KSUCCESS;
}

/* Sys_Kot_TCB_Set :
    Arguments : 

*/
KResult Sys_Kot_TCB_Set(SyscallStack* Registers, kthread_t* Thread){
    kthread_t* threadkey;
    uint64_t flags;
    if(Keyhole_Get(Thread, (key_t)SYSCALL_ARG0(Registers), DataTypeThread, (uint64_t*)&threadkey, &flags) != KSUCCESS) return KKEYVIOLATION;
    if(!(flags & KeyholeFlagDataTypeThreadAllowChangeTCB)) return KMEMORYVIOLATION;
    threadkey->FSBase = (void*)SYSCALL_ARG1(Registers);
    if(Thread->TID == threadkey->TID && Thread->Parent->PID == threadkey->Parent->PID){
        CPU::SetCPUFSBase((uint64_t)threadkey->FSBase);
    }
    return KSUCCESS;
}

/* Sys_Kot_Thread_Info_Get :
    Arguments : 

*/
KResult Sys_Kot_Thread_Info_Get(SyscallStack* Registers, kthread_t* Thread){
    kthread_t* threadkey;
    uint64_t flags;
    if(Keyhole_Get(Thread, (key_t)SYSCALL_ARG0(Registers), DataTypeThread, (uint64_t*)&threadkey, &flags) != KSUCCESS) return KKEYVIOLATION;
    switch (SYSCALL_ARG1(Registers)){
        case 0:{ // TID
            if(CheckUserAddress((void*)SYSCALL_ARG2(Registers), sizeof(pid_t)) != KSUCCESS) return KMEMORYVIOLATION;
            *(pid_t*)SYSCALL_ARG2(Registers) = threadkey->TID;
            break;

        }
        case 1:{ // Stack start
            if(CheckUserAddress((void*)SYSCALL_ARG2(Registers), sizeof(uint64_t)) != KSUCCESS) return KMEMORYVIOLATION;
            *(uint64_t*)SYSCALL_ARG2(Registers) = (uint64_t)STACK_TOP;
            break;
        }
        case 2:{ // Stack size
            if(CheckUserAddress((void*)SYSCALL_ARG2(Registers), sizeof(uint64_t)) != KSUCCESS) return KMEMORYVIOLATION;
            *(uint64_t*)SYSCALL_ARG2(Registers) = (uint64_t)STACK_TOP - (uint64_t)STACK_BOTTOM;
            break;
        }
        case 3:{ // Entry point
            if(CheckUserAddress((void*)SYSCALL_ARG2(Registers), sizeof(uint64_t)) != KSUCCESS) return KMEMORYVIOLATION;
            *(uint64_t*)SYSCALL_ARG2(Registers) = (uint64_t)threadkey->EntryPoint;
            break;
        }
        default:
            return KFAIL;
    }
    return KSUCCESS;
}

/* Sys_Kot_WaitPID :
    Arguments : 
    0 -> pid                > pid_t
    1 -> status             > int*
    1 -> flags              > int
*/
KResult Sys_Kot_WaitPID(SyscallStack* Registers, kthread_t* Thread){
    if(CheckUserAddress((void*)SYSCALL_ARG1(Registers), sizeof(int)) != KSUCCESS) return KMEMORYVIOLATION;

    pid_t Pid = static_cast<pid_t>(SYSCALL_ARG0(Registers));
    int* Status = reinterpret_cast<int*>(SYSCALL_ARG1(Registers));
    int Flags = static_cast<int>(SYSCALL_ARG2(Registers));

    wait_pid_t* WaitPIDInfo = (wait_pid_t*)kmalloc(sizeof(wait_pid_t));
    WaitPIDInfo->Pid = Pid;
    WaitPIDInfo->Thread = Thread;

    if(Pid <= 0){
        kprocess_t* ProcParent = Thread->Parent;
        while(ProcParent->Parent){
            ProcParent = ProcParent->Parent;
        }

        AtomicAcquire(&ProcParent->WaitPIDLock);
        if(ProcParent->ProcessChildCount){
            ProcParent->WaitPIDList->push64((uint64_t)WaitPIDInfo);
            ProcParent->WaitPIDCount++;
            AtomicRelease(&ProcParent->WaitPIDLock);
            Thread->Pause((ContextStack*)Registers, false);
            *Status = Thread->PIDWaitStatus;
        }else{
            AtomicRelease(&ProcParent->WaitPIDLock);
            kfree(WaitPIDInfo);
            *Status = NULL;
        }
    }else{
        kprocess_t* ProcessFromPid;
        uint64_t Flags;
        if(Keyhole_Get(Thread, (key_t)SYSCALL_ARG0(Registers), DataTypeProcess, (uint64_t*)&ProcessFromPid, &Flags) != KSUCCESS) return KKEYVIOLATION;
        
        kprocess_t* ProcParent = ProcessFromPid;
        while(ProcParent->Parent){
            ProcParent = ProcParent->Parent;
        }

        AtomicAcquire(&ProcParent->WaitPIDLock);
        if(ProcParent->ProcessChildCount){
            ProcParent->WaitPIDList->push64((uint64_t)WaitPIDInfo);
            ProcParent->WaitPIDCount++;
            AtomicRelease(&ProcParent->WaitPIDLock);
            Thread->Pause((ContextStack*)Registers, false);
            *Status = Thread->PIDWaitStatus;
        }else{
            AtomicRelease(&ProcParent->WaitPIDLock);
            kfree(WaitPIDInfo);
            *Status = NULL;
        }
    }

    return KSUCCESS;
}

/* Sys_Kot_Logs :
    Arguments : 
    0 -> string             > char*
    1 -> size               > size64_t
*/
KResult Sys_Kot_Logs(SyscallStack* Registers, kthread_t* Thread){
    if(CheckUserAddress((void*)SYSCALL_ARG0(Registers), SYSCALL_ARG1(Registers)) != KSUCCESS) return KMEMORYVIOLATION;
    MessageProcess((char*)SYSCALL_ARG0(Registers), SYSCALL_ARG1(Registers), Thread->Parent->PID, Thread->Parent->PPID, Thread->TID);
    return KSUCCESS;
}