#include <syscall/syscall.h>

/* -------------------------------Specs--------------------------------------- */

/* Sys_Function :
    Arguments : 
    0 -> description            > type
    1 -> description            > type
    2 -> description            > type
    3 -> description            > type
    4 -> description            > type
    5 -> description            > type
*/

/* -------------------------------Functions------------------------------------ */

/* Sys_CreateMemoryField :
    Arguments : 
    0 -> process taget                  > key process
    1 -> size                           > uint64_t
    2 -> pointer to virtual address     > uint64_t*
    3 -> return                         > key shared memory
    4 -> reserved                       > none
    5 -> reserved                       > none
*/
KResult Sys_CreateMemoryField(SyscallStack* Registers, kthread_t* Thread){
    kprocess_t* processkey;
    uint64_t flags;
    uint64_t data;
    if(Keyhole_Get(Thread, (key_t)Registers->arg0, DataTypeProcess, (uint64_t*)&processkey, &flags) != KSUCCESS) return KKEYVIOLATION;
    if(!(flags & KeyholeFlagDataTypeProcessMemoryAccessible)) return KKEYVIOLATION;
    if(CreateMemoryField(Thread, processkey, Registers->arg1, (uint64_t*)Registers->arg2, &data, (enum MemoryFieldType)Registers->arg4) != KSUCCESS) return KFAIL;
    return Keyhole_Create((key_t*)Registers->arg3, Thread->Parent, NULL, DataTypeSharedMemory, data, KeyholeFlagFullPermissions, PriviledgeApp);
}

/* Sys_AcceptMemoryField :
    Arguments : 
*/
KResult Sys_AcceptMemoryField(SyscallStack* Registers, kthread_t* Thread){
    kprocess_t* processkey;
    MemoryShareInfo* memoryKey;
    uint64_t flags;

    if(CheckUserAddress((uintptr_t)Registers->arg2, sizeof(uint64_t)) != KSUCCESS) return KMEMORYVIOLATION;
    uint64_t* virtualAddressPointer = (uint64_t*)Registers->arg2;

    if(Keyhole_Get(Thread, (key_t)Registers->arg0, DataTypeProcess, (uint64_t*)&processkey, &flags) != KSUCCESS) return KKEYVIOLATION;
    if(!(flags & KeyholeFlagDataTypeProcessMemoryAccessible)) return KKEYVIOLATION;
    if(Keyhole_Get(Thread, (key_t)Registers->arg1, DataTypeSharedMemory, (uint64_t*)&memoryKey, &flags) != KSUCCESS) return KKEYVIOLATION;
    return AcceptMemoryField(Thread, processkey, memoryKey, virtualAddressPointer);
}

/* Sys_CloseMemoryField :
    Arguments : 
*/
KResult Sys_CloseMemoryField(SyscallStack* Registers, kthread_t* Thread){
    kprocess_t* processkey;
    MemoryShareInfo* memoryKey;
    uint64_t flags;
    if(Keyhole_Get(Thread, (key_t)Registers->arg0, DataTypeProcess, (uint64_t*)&processkey, &flags) != KSUCCESS) return KKEYVIOLATION;
    if(!(flags & KeyholeFlagDataTypeProcessMemoryAccessible)) return KKEYVIOLATION;
    if(Keyhole_Get(Thread, (key_t)Registers->arg1, DataTypeSharedMemory, (uint64_t*)&memoryKey, &flags) != KSUCCESS) return KKEYVIOLATION;
    return CloseMemoryField(Thread, processkey, memoryKey, (uintptr_t)Registers->arg2);    
}

/* Sys_GetInfoMemoryField :
    Arguments : 
*/
KResult Sys_GetInfoMemoryField(SyscallStack* Registers, kthread_t* Thread){
    MemoryShareInfo* memoryKey;
    uint64_t flags;
    if(Keyhole_Get(Thread, (key_t)Registers->arg0, DataTypeSharedMemory, (uint64_t*)&memoryKey, &flags) != KSUCCESS) return KKEYVIOLATION;
    if(CheckUserAddress((uintptr_t)Registers->arg1, sizeof(uint64_t)) == KSUCCESS){
        uint64_t* TypePointer = (uint64_t*)Registers->arg1;
        *TypePointer = (uint64_t)memoryKey->Type;
    }

    if(CheckUserAddress((uintptr_t)Registers->arg2, sizeof(uint64_t)) == KSUCCESS){
        size64_t* SizePointer = (size64_t*)Registers->arg2;
        *SizePointer = (uint64_t)memoryKey->InitialSize;
    }
    return KSUCCESS;
}

/* Sys_CreateProc :
    Arguments : 
*/
KResult Sys_CreateProc(SyscallStack* Registers, kthread_t* Thread){
    kprocess_t* data;
    if(Registers->arg4 > PriviledgeApp){
        Registers->arg4 = PriviledgeApp;
    }
    if(CheckUserAddress((uintptr_t)Registers->arg0, sizeof(key_t)) != KSUCCESS) return KMEMORYVIOLATION;
    if(globalTaskManager->CreateProcess(&data, (enum Priviledge)Registers->arg1, Registers->arg2) != KSUCCESS) return KFAIL;
    return Keyhole_Create((key_t*)Registers->arg0, data, Thread->Parent, DataTypeProcess, (uint64_t)data, KeyholeFlagFullPermissions, PriviledgeApp);
}

/* Sys_Fork :
    Arguments : 
*/
KResult Sys_Fork(SyscallStack* Registers, kthread_t* Thread){
    uint64_t flags;
    if(CheckUserAddress((uintptr_t)Registers->arg0, sizeof(key_t)) != KSUCCESS) return KMEMORYVIOLATION;
    kprocess_t* Child;
    kthread_t* ChildThread;
    kprocess_t* Parent = Thread->Parent;
    // Select non fork process
    while(Parent->Parent){
        Parent = Parent->Parent;
    }
    KResult Status = Thread->Parent->Fork((ContextStack*)Registers, Thread, &Child, &ChildThread);
    if(Status != KSUCCESS) return Status;
    
    return Keyhole_Create((key_t*)Registers->arg0, Child, Thread->Parent, DataTypeProcess, (uint64_t)Child, KeyholeFlagFullPermissions, PriviledgeApp);
}

/* Sys_CloseProc :
    Arguments : 
*/
KResult Sys_CloseProc(SyscallStack* Registers, kthread_t* Thread){
    //TODO
    return KFAIL;
}

/* Sys_Close :
    Arguments : 
*/
KResult Sys_Close(SyscallStack* Registers, kthread_t* Thread){
    if(Thread->IsEvent){
        return KFAIL;
    }else{
        return Thread->Close((ContextStack*)Registers, Registers->arg0);
    }
}

/* Sys_Exit :
    Arguments : 
*/
KResult Sys_Exit(SyscallStack* Registers, kthread_t* Thread){
    return globalTaskManager->Exit((ContextStack*)Registers, Thread, Registers->arg0);
}

/* Sys_Pause :
    Arguments : 
*/
KResult Sys_Pause(SyscallStack* Registers, kthread_t* Thread){
    return Thread->Pause((ContextStack*)Registers, Registers->arg0);
    /* No return */
}

/* Sys_UnPause :
    Arguments : 
*/
KResult Sys_UnPause(SyscallStack* Registers, kthread_t* Thread){
    kthread_t* threadkey;
    uint64_t flags;
    if(Keyhole_Get(Thread, (key_t)Registers->arg0, DataTypeThread, (uint64_t*)&threadkey, &flags) != KSUCCESS) return KKEYVIOLATION;
    if(!(flags & KeyholeFlagDataTypeThreadIsUnpauseable)) return KKEYVIOLATION;
    return globalTaskManager->Unpause(threadkey);
}

/* Sys_Map :
    Arguments : 
    0 -> process            > key
    1 -> virtual address    > uint64_t*
    2 -> physicall or not   > bool
    3 -> physical address   > uintptr_t
    4 -> size               > size64_t
    5 -> find free address  > bool
*/
KResult Sys_Map(SyscallStack* Registers, kthread_t* Thread){
    // Check priviledge
    if(Registers->arg2 > AllocationTypeBasic && Thread->Priviledge != PriviledgeDriver){
        return KNOTALLOW;
    } 
    kprocess_t* processkey;
    uint64_t flags;
    if(Keyhole_Get(Thread, (key_t)Registers->arg0, DataTypeProcess, (uint64_t*)&processkey, &flags) != KSUCCESS){
        return KKEYVIOLATION;
    } 
    if(!(flags & KeyholeFlagDataTypeProcessMemoryAccessible)){
       return KKEYVIOLATION; 
    } 
    
    pagetable_t pageTable = processkey->SharedPaging;
    
    /* Get arguments */
    if(Registers->arg2 > AllocationTypePhysicalContiguous){
        Registers->arg2 = AllocationTypeBasic;
    }

    uint64_t* addressVirtual = (uint64_t*)Registers->arg1;
    enum AllocationType type = (enum AllocationType)Registers->arg2;
    uintptr_t* addressPhysical = (uintptr_t*)Registers->arg3;
    size64_t* size = (size64_t*)Registers->arg4;
    bool IsNeedToBeFree = (bool)Registers->arg5; 

    if(!CheckUserAddress((uintptr_t)addressVirtual, sizeof(uint64_t))){
        return KMEMORYVIOLATION;
    } 
    *addressVirtual = *addressVirtual - ((uint64_t)*addressVirtual % PAGE_SIZE);


    bool IsPhysicalAddress = false;
    if(CheckUserAddress((uintptr_t)addressPhysical, sizeof(uint64_t))){
        IsPhysicalAddress = true;
    }else{
        if(type == AllocationTypePhysical){
            return KMEMORYVIOLATION;
        }
    }

    if(!CheckUserAddress((uintptr_t)size, sizeof(uint64_t))){
        return KMEMORYVIOLATION;
    } 


    uint64_t pageCount = DivideRoundUp(*size, PAGE_SIZE);
    /* find free page */
    if(IsNeedToBeFree){
        for(uint64_t FreeSize = 0; FreeSize < *size;){
            bool IsPresent = vmm_GetFlags(pageTable, (uintptr_t)*addressVirtual, vmm_flag::vmm_Present);
            while(IsPresent){
                *addressVirtual += PAGE_SIZE;
                IsPresent = vmm_GetFlags(pageTable, (uintptr_t)*addressVirtual, vmm_flag::vmm_Present);
                FreeSize = 0;
            }
            FreeSize += PAGE_SIZE;
        }
    }

    *size = NULL;

    if(*addressVirtual + pageCount * PAGE_SIZE < vmm_HHDMAdress){
        if(type == AllocationTypePhysical || type == AllocationTypePhysicalContiguous){
            for(uint64_t i = 0; i < pageCount; i++){
                if(vmm_GetFlags(pageTable, (uintptr_t)(*addressVirtual + i * PAGE_SIZE), vmm_flag::vmm_IsPureMemory) && vmm_GetFlags(pageTable, (uintptr_t)(*addressVirtual + i * PAGE_SIZE), vmm_flag::vmm_Master)){
                    Pmm_FreePage(vmm_GetPhysical(pageTable, (uintptr_t)(*addressVirtual + i * PAGE_SIZE)));
                }
                vmm_Unmap(pageTable, (uintptr_t)(*addressVirtual + i * PAGE_SIZE));
            }
        }
        
        if(type == AllocationTypePhysicalContiguous){
            uintptr_t physicalAddressAllocated = (uintptr_t)Pmm_RequestPages(pageCount);
            if(physicalAddressAllocated != NULL){
                if(IsPhysicalAddress){
                    *addressPhysical = physicalAddressAllocated;
                    /* write only the first physicall page */
                    IsPhysicalAddress = false; 
                }

                for(uint64_t i = 0; i < pageCount; i++){
                    uintptr_t virtualAddress = (uintptr_t)(*addressVirtual + i * PAGE_SIZE);
                    uintptr_t physicalAddress = (uintptr_t)((uint64_t)physicalAddressAllocated + i * PAGE_SIZE);
                    vmm_Map(pageTable, virtualAddress, physicalAddress, true, true, false);
                    vmm_SetFlags(pageTable, virtualAddress, vmm_flag::vmm_Master, true); //set master state
                    vmm_SetFlags(pageTable, virtualAddress, vmm_flag::vmm_CacheDisabled, true); //disable cache
                }
                processkey->MemoryAllocated += pageCount * PAGE_SIZE; 
                *size = pageCount * PAGE_SIZE;
                return KSUCCESS;               
            }
            return KFAIL;
        }else{
            for(uint64_t i = 0; i < pageCount; i++){
                uintptr_t virtualAddress = (uintptr_t)(*addressVirtual + i * PAGE_SIZE);
                if(type == AllocationTypePhysical){
                    vmm_Map(pageTable, virtualAddress, (uintptr_t)((uint64_t)*addressPhysical + i * PAGE_SIZE), true, true, false);
                    vmm_SetFlags(pageTable, virtualAddress, vmm_flag::vmm_IsPureMemory, false); // remove master state
                    vmm_SetFlags(pageTable, virtualAddress, vmm_flag::vmm_CacheDisabled, true); //disable cache
                }else if(!vmm_GetFlags(pageTable, (uintptr_t)(*addressVirtual + i * PAGE_SIZE), vmm_flag::vmm_IsPureMemory)){
                    uintptr_t physicalAddressAllocated = (uintptr_t)Pmm_RequestPage();
                    if(IsPhysicalAddress){
                        *addressPhysical = physicalAddressAllocated;
                        /* write only the first physicall page */
                        IsPhysicalAddress = false; 
                    }
                    vmm_Map(pageTable, virtualAddress, physicalAddressAllocated, true, true, true);
                    vmm_SetFlags(pageTable, virtualAddress, vmm_flag::vmm_IsPureMemory, true); // set master state
                    processkey->MemoryAllocated += PAGE_SIZE;
                    *size += PAGE_SIZE;      
                }
            }             
        }
        return KSUCCESS;
    }
    return KFAIL;
}

/* Sys_Unmap :
    Arguments : 
    0 -> process            > key
    1 -> virtual address    > uint64_t*
    4 -> size               > size64_t
*/
KResult Sys_Unmap(SyscallStack* Registers, kthread_t* Thread){
    kprocess_t* processkey;
    uint64_t flags;
    if(Keyhole_Get(Thread, (key_t)Registers->arg0, DataTypeProcess, (uint64_t*)&processkey, &flags) != KSUCCESS) return KKEYVIOLATION;
    if(!(flags & KeyholeFlagDataTypeProcessMemoryAccessible)) return KKEYVIOLATION;
    pagetable_t pageTable = processkey->SharedPaging;
    uintptr_t addressVirtual = (uintptr_t)Registers->arg1;
    size64_t size = Registers->arg2;

    addressVirtual = (uintptr_t)((uint64_t)addressVirtual - (uint64_t)addressVirtual % PAGE_SIZE);
    uint64_t pageCount = DivideRoundUp(size, PAGE_SIZE);
    if((uint64_t)addressVirtual + pageCount * PAGE_SIZE < vmm_HHDMAdress){
        for(uint64_t i = 0; i < pageCount; i += PAGE_SIZE){
            if(vmm_GetFlags(pageTable, (uintptr_t)addressVirtual, vmm_flag::vmm_Master)){
                if(vmm_GetFlags(pageTable, (uintptr_t)addressVirtual, vmm_flag::vmm_IsPureMemory)){
                    Pmm_FreePage(vmm_GetPhysical(pageTable, addressVirtual));
                    processkey->MemoryAllocated -= PAGE_SIZE;
                }
                vmm_Unmap(pageTable, addressVirtual);
            }
        }
    }
    return KSUCCESS;
}

/* Sys_GetPhysical :
    Arguments : 
*/
KResult Sys_GetPhysical(SyscallStack* Registers, kthread_t* Thread){
    if(Registers->arg0 < vmm_HHDMAdress){
        return (KResult)vmm_GetPhysical(Thread->Paging, (uintptr_t)Registers->arg0);
    }else{
        return NULL;
    }
}

/* Sys_Event_Create :
    Arguments : 
*/
KResult Sys_Event_Create(SyscallStack* Registers, kthread_t* Thread){
    uint64_t data;
    if(Event::Create((kevent_t**)&data, EventTypeIPC, Registers->arg0) != KSUCCESS) return KFAIL;
    return Keyhole_Create((key_t*)Registers->arg0, Thread->Parent, Thread->Parent, DataTypeEvent, data, KeyholeFlagFullPermissions, PriviledgeApp);
}

/* Sys_Event_Bind :
    Arguments : 
*/
KResult Sys_Event_Bind(SyscallStack* Registers, kthread_t* Thread){
    kevent_t* event; 
    kthread_t* threadkey;
    uint64_t flags;
    if(Keyhole_Get(Thread, (key_t)Registers->arg0, DataTypeEvent, (uint64_t*)&event, &flags) != KSUCCESS) return KKEYVIOLATION;
    if(Keyhole_Get(Thread, (key_t)Registers->arg1, DataTypeThread, (uint64_t*)&threadkey, &flags) != KSUCCESS) return KKEYVIOLATION;
    if(!(flags & KeyholeFlagDataTypeThreadIsEventable)) return KKEYVIOLATION;
    return Event::Bind(threadkey, event, (bool)Registers->arg2);
}

/* Sys_Event_Unbind :
    Arguments : 
*/
KResult Sys_Event_Unbind(SyscallStack* Registers, kthread_t* Thread){
    kevent_t* event;
    kthread_t* threadkey;
    uint64_t flags;
    if(Keyhole_Get(Thread, (key_t)Registers->arg0, DataTypeEvent, (uint64_t*)&event, &flags) != KSUCCESS) return KKEYVIOLATION;
    if(Keyhole_Get(Thread, (key_t)Registers->arg1, DataTypeThread, (uint64_t*)&threadkey, &flags) != KSUCCESS) return KKEYVIOLATION;
    if(!(flags & KeyholeFlagDataTypeEventIsBindable)) return KKEYVIOLATION;
    if(!(flags & KeyholeFlagDataTypeThreadIsEventable)) return KKEYVIOLATION;
    return Event::Unbind(Thread, event);
}

/* Sys_Event_Trigger :
    Arguments : 
*/
KResult Sys_Event_Trigger(SyscallStack* Registers, kthread_t* Thread){
    kevent_t* event; 
    uint64_t flags;
    if(Keyhole_Get(Thread, (key_t)Registers->arg0, DataTypeEvent, (uint64_t*)&event, &flags) != KSUCCESS) return KKEYVIOLATION;
    if(!(flags & KeyholeFlagDataTypeEventIsTriggerable)) return KKEYVIOLATION;
    if(!CheckUserAddress((uintptr_t)Registers->arg1, sizeof(arguments_t))) return KMEMORYVIOLATION;
    return Event::Trigger(event, (arguments_t*)Registers->arg1);
}

/* Sys_Event_Close :
    Arguments : 
*/
KResult Sys_Event_Close(SyscallStack* Registers, kthread_t* Thread){
    if(!Thread->IsEvent) return KFAIL;
    return Event::Close((ContextStack*)Registers, Thread);
}

/* Sys_CreateThread :
    Arguments : 
*/
KResult Sys_CreateThread(SyscallStack* Registers, kthread_t* Thread){
    kprocess_t* processkey;
    uint64_t flags;
    kthread_t* threadData;
    if(Keyhole_Get(Thread, (key_t)Registers->arg0, DataTypeProcess, (uint64_t*)&processkey, &flags) != KSUCCESS) return KKEYVIOLATION;
    if(!(flags & KeyholeFlagDataTypeProcessIsThreadCreateable)) return KKEYVIOLATION;
    if(Registers->arg2 > PriviledgeApp){
        Registers->arg2 = Thread->Parent->DefaultPriviledge;
    }
    if(globalTaskManager->Createthread(&threadData, processkey, (uintptr_t)Registers->arg1, (enum Priviledge)Registers->arg2, Registers->arg3) != KSUCCESS) return KFAIL;
    return Keyhole_Create((key_t*)Registers->arg4, Thread->Parent, Thread->Parent, DataTypeThread, (uint64_t)threadData, KeyholeFlagFullPermissions, PriviledgeApp);
}

/* Sys_Duplicatethread :
    Arguments : 
*/
KResult Sys_DuplicateThread(SyscallStack* Registers, kthread_t* Thread){
    kprocess_t* processkey;
    kthread_t* threadkey;
    uint64_t flags;
    if(Keyhole_Get(Thread, (key_t)Registers->arg0, DataTypeProcess, (uint64_t*)&processkey, &flags) != KSUCCESS) return KKEYVIOLATION;
    if(!(flags & KeyholeFlagDataTypeProcessIsThreadCreateable)) return KKEYVIOLATION;
    if(Keyhole_Get(Thread, (key_t)Registers->arg1, DataTypeThread, (uint64_t*)&threadkey, &flags) != KSUCCESS) return KKEYVIOLATION;
    if(!(flags & KeyholeFlagDataTypeThreadIsDuplicable)) return KKEYVIOLATION;
    if(globalTaskManager->Duplicatethread(&Thread, processkey, threadkey) != KSUCCESS) return KFAIL;     
    return Keyhole_Create((key_t*)Registers->arg2, Thread->Parent, Thread->Parent, DataTypeThread, (uint64_t)Thread, KeyholeFlagFullPermissions, PriviledgeApp);
}

/* Sys_ExecThread :
    Arguments : 
*/
KResult Sys_ExecThread(SyscallStack* Registers, kthread_t* Thread){
    kthread_t* threadkey;
    uint64_t flags;
    if(Keyhole_Get(Thread, (key_t)Registers->arg0, DataTypeThread, (uint64_t*)&threadkey, &flags) != KSUCCESS) return KKEYVIOLATION;
    enum ExecutionType Type = (enum ExecutionType)(Registers->arg2 & 0b11); // only the two first bits can be handle
    if(Type == ExecutionTypeQueu || Type == ExecutionTypeQueuAwait){
        if(!(flags & KeyholeFlagDataTypeThreadIsExecutableWithQueue)){
            return KKEYVIOLATION;
        }
    }
    if(Type == ExecutionTypeOneshot || Type == ExecutionTypeOneshotAwait){
        if(!(flags & KeyholeFlagDataTypeThreadIsExecutableOneshot)){
            return KKEYVIOLATION;
        }
    }

    if(Registers->arg1 != NULL){
        if(CheckUserAddress((uintptr_t)Registers->arg1, sizeof(arguments_t)) != KSUCCESS) return KMEMORYVIOLATION;    
    }
    ThreadShareData_t* Data = (ThreadShareData_t*)Registers->arg3;
    if(Data != NULL){
        if(CheckUserAddress((uintptr_t)Data, sizeof(ThreadShareData_t)) != KSUCCESS) return KMEMORYVIOLATION;    
        if(CheckUserAddress((uintptr_t)Data->Data, Data->Size) != KSUCCESS) return KMEMORYVIOLATION;    
    }
    KResult Status = globalTaskManager->Execthread(Thread, threadkey, Type, (arguments_t*)Registers->arg1, Data, (ContextStack*)Registers);
    return Status;
}

/* Sys_Keyhole_CloneModify :
    Arguments : 
    0 -> string             > char*
    1 -> size               > size64_t
*/
KResult Sys_Keyhole_CloneModify(SyscallStack* Registers, kthread_t* Thread){
    kprocess_t* processkey;
    uint64_t flags;
    if(Registers->arg2 != NULL){
        if(Keyhole_Get(Thread, (key_t)Registers->arg2, DataTypeProcess, (uint64_t*)&processkey, &flags) != KSUCCESS) return KKEYVIOLATION;     
    }else{
        processkey = NULL;
    }
    if(CheckUserAddress((uintptr_t)Registers->arg1, sizeof(uint64_t)) != KSUCCESS) return KMEMORYVIOLATION;
    if(Registers->arg4 > PriviledgeApp){
        Registers->arg4 = PriviledgeApp;
    }
    return Keyhole_CloneModify(Thread, (key_t)Registers->arg0, (key_t*)Registers->arg1, processkey, Registers->arg3, (enum Priviledge)Registers->arg4);
}

/* Sys_Keyhole_Verify :
    Arguments : 

*/
KResult Sys_Keyhole_Verify(SyscallStack* Registers, kthread_t* Thread){
    if(CheckUserAddress((uintptr_t)Registers->arg2, sizeof(uint64_t)) != KSUCCESS) return KMEMORYVIOLATION;
    if(CheckUserAddress((uintptr_t)Registers->arg3, sizeof(uint64_t)) != KSUCCESS) return KMEMORYVIOLATION;
    if(CheckUserAddress((uintptr_t)Registers->arg4, sizeof(uint64_t)) != KSUCCESS) return KMEMORYVIOLATION;
    key_t key = Registers->arg0;
    uint64_t Status = Keyhole_Verify(Thread, key, (enum DataType)Registers->arg1);
    if(Status != KSUCCESS) return Status;
    lock_t* lock = (lock_t*)key;
    uint64_t* target = (uint64_t*)Registers->arg2;
    uint64_t* flags = (uint64_t*)Registers->arg3;
    uint64_t* minpriviledge = (uint64_t*)Registers->arg4;
    if(lock->Target == NULL){
        *target = NULL;
    }else{
        *target = lock->Target->ProcessKey;
    }
    *flags = lock->Flags;
    *minpriviledge = lock->MinPriviledge;
    return KSUCCESS;
}

/* Sys_TCB_Set :
    Arguments : 

*/
KResult Sys_TCB_Set(SyscallStack* Registers, kthread_t* Thread){
    kthread_t* threadkey;
    uint64_t flags;
    if(Keyhole_Get(Thread, (key_t)Registers->arg0, DataTypeThread, (uint64_t*)&threadkey, &flags) != KSUCCESS) return KKEYVIOLATION;
    if(!(flags & KeyholeFlagDataTypeThreadAllowChangeTCB)) return KMEMORYVIOLATION;
    threadkey->FSBase = (uintptr_t)Registers->arg1;
    CPU::SetCPUFSBase((uint64_t)threadkey->FSBase);
    return KSUCCESS;
}

/* Sys_Thread_Info_Get :
    Arguments : 

*/
KResult Sys_Thread_Info_Get(SyscallStack* Registers, kthread_t* Thread){
    kthread_t* threadkey;
    uint64_t flags;
    if(Keyhole_Get(Thread, (key_t)Registers->arg0, DataTypeThread, (uint64_t*)&threadkey, &flags) != KSUCCESS) return KKEYVIOLATION;
    switch (Registers->arg1){
        case 0:{ // TID
            if(CheckUserAddress((uintptr_t)Registers->arg2, sizeof(uint64_t)) != KSUCCESS) return KMEMORYVIOLATION;
            *(uint64_t*)Registers->arg2 = threadkey->TID;
            break;

        }
        case 1:{ // Stack start
            if(CheckUserAddress((uintptr_t)Registers->arg2, sizeof(uint64_t)) != KSUCCESS) return KMEMORYVIOLATION;
            *(uint64_t*)Registers->arg2 = (uint64_t)StackTop;
            break;
        }
        case 2:{ // Stack size
            if(CheckUserAddress((uintptr_t)Registers->arg2, sizeof(uint64_t)) != KSUCCESS) return KMEMORYVIOLATION;
            *(uint64_t*)Registers->arg2 = (uint64_t)StackTop - (uint64_t)StackBottom;
            break;
        }
        case 3:{ // Entry point
            if(CheckUserAddress((uintptr_t)Registers->arg2, sizeof(uint64_t)) != KSUCCESS) return KMEMORYVIOLATION;
            *(uint64_t*)Registers->arg2 = (uint64_t)threadkey->EntryPoint;
            break;
        }
        default:
            return KFAIL;
    }
    return KSUCCESS;
}

/* Sys_Logs :
    Arguments : 
    0 -> string             > char*
    1 -> size               > size64_t
*/
KResult Sys_Logs(SyscallStack* Registers, kthread_t* Thread){
    if(CheckUserAddress((uintptr_t)Registers->arg0, Registers->arg1) != KSUCCESS) return KMEMORYVIOLATION;
    MessageProcess((char*)Registers->arg0, Registers->arg1, Thread->Parent->PID, Thread->Parent->PPID, Thread->TID);
    return KSUCCESS;
}

static SyscallHandler SyscallHandlers[Syscall_Count] = { 
    [KSys_CreateMemoryField] = Sys_CreateMemoryField,
    [KSys_AcceptMemoryField] = Sys_AcceptMemoryField,
    [KSys_CloseMemoryField] = Sys_CloseMemoryField,
    [KSys_GetTypeMemoryField] = Sys_GetInfoMemoryField,
    [KSys_CreateProc] = Sys_CreateProc,
    [KSys_Fork] = Sys_Fork,
    [KSys_CloseProc] = Sys_CloseProc,
    [KSys_Close] = Sys_Close,
    [KSys_Exit] = Sys_Exit,
    [KSys_Pause] = Sys_Pause,
    [KSys_UnPause] = Sys_UnPause,
    [KSys_Map] = Sys_Map,
    [KSys_Unmap] = Sys_Unmap,
    [KSys_GetPhysical] = Sys_GetPhysical,
    [KSys_Event_Create] = Sys_Event_Create,
    [KSys_Event_Bind] = Sys_Event_Bind,
    [KSys_Event_Unbind] = Sys_Event_Unbind,
    [KSys_Event_Trigger] = Sys_Event_Trigger,
    [KSys_Event_Close] = Sys_Event_Close,
    [KSys_CreateThread] = Sys_CreateThread,
    [KSys_DuplicateThread] = Sys_DuplicateThread,
    [KSys_ExecThread] = Sys_ExecThread,
    [KSys_Keyhole_CloneModify] = Sys_Keyhole_CloneModify,
    [KSys_Keyhole_Verify] = Sys_Keyhole_Verify,
    [KSys_TCB_Set] = Sys_TCB_Set,
    [KSys_Thread_Info_Get] = Sys_Thread_Info_Get,
    [KSys_Logs] = Sys_Logs,
};

extern "C" void SyscallDispatch(SyscallStack* Registers, kthread_t* Self){
    if(Self->Parent->PID == 0x10){
        asm("nop");
    }
    if(Registers->GlobalPurpose >= Syscall_Count){
        Registers->arg0 = KFAIL;
        return;
    }

    Registers->GlobalPurpose = SyscallHandlers[Registers->GlobalPurpose](Registers, Self);

    return;
}