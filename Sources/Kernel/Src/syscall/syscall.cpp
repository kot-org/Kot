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
KResult Sys_CreateMemoryField(SyscallStack* Registers, kthread_t* thread){
    kprocess_t* processkey;
    uint64_t flags;
    uint64_t data;
    if(Keyhole_Get(thread, (key_t)Registers->arg0, DataTypeProcess, (uint64_t*)&processkey, &flags) != KSUCCESS) return KKEYVIOLATION;
    if(!Keyhole_GetFlag(flags, KeyholeFlagDataTypeProcessMemoryAccessible)) return KKEYVIOLATION;
    if(CreateMemoryField(processkey, Registers->arg1, (uint64_t*)Registers->arg2, &data, (enum MemoryFieldType)Registers->arg4) != KSUCCESS) return KFAIL;
    return Keyhole_Create((key_t*)Registers->arg3, thread->Parent, NULL, DataTypeSharedMemory, data, KeyholeFlagFullPermissions, PriviledgeApp);
}

/* Sys_AcceptMemoryField :
    Arguments : 
*/
KResult Sys_AcceptMemoryField(SyscallStack* Registers, kthread_t* thread){
    kprocess_t* processkey;
    MemoryShareInfo* memoryKey;
    uint64_t flags;
    if(CheckAddress((uintptr_t)Registers->arg2, sizeof(uint64_t)) != KSUCCESS) return KMEMORYVIOLATION;
    uint64_t* virtualAddressPointer = (uint64_t*)Registers->arg2;

    if(Keyhole_Get(thread, (key_t)Registers->arg0, DataTypeProcess, (uint64_t*)&processkey, &flags) != KSUCCESS) return KKEYVIOLATION;
    if(!Keyhole_GetFlag(flags, KeyholeFlagDataTypeProcessMemoryAccessible)) return KKEYVIOLATION;
    if(Keyhole_Get(thread, (key_t)Registers->arg1, DataTypeSharedMemory, (uint64_t*)&memoryKey, &flags) != KSUCCESS) return KKEYVIOLATION;
    return AcceptMemoryField(processkey, memoryKey, virtualAddressPointer);
}

/* Sys_CloseMemoryField :
    Arguments : 
*/
KResult Sys_CloseMemoryField(SyscallStack* Registers, kthread_t* thread){
    kprocess_t* processkey;
    MemoryShareInfo* memoryKey;
    uint64_t flags;
    if(Keyhole_Get(thread, (key_t)Registers->arg0, DataTypeProcess, (uint64_t*)&processkey, &flags) != KSUCCESS) return KKEYVIOLATION;
    if(!Keyhole_GetFlag(flags, KeyholeFlagDataTypeProcessMemoryAccessible)) return KKEYVIOLATION;
    if(Keyhole_Get(thread, (key_t)Registers->arg1, DataTypeSharedMemory, (uint64_t*)&memoryKey, &flags) != KSUCCESS) return KKEYVIOLATION;
    return CloseMemoryField(processkey, memoryKey, (uintptr_t)Registers->arg2);    
}

/* Sys_GetInfoMemoryField :
    Arguments : 
*/
KResult Sys_GetInfoMemoryField(SyscallStack* Registers, kthread_t* thread){
    MemoryShareInfo* memoryKey;
    uint64_t flags;
    if(Keyhole_Get(thread, (key_t)Registers->arg0, DataTypeSharedMemory, (uint64_t*)&memoryKey, &flags) != KSUCCESS) return KKEYVIOLATION;
    if(!CheckAddress((uintptr_t)Registers->arg1, sizeof(uint64_t))) return KMEMORYVIOLATION;
    if(!CheckAddress((uintptr_t)Registers->arg2, sizeof(uint64_t))) return KMEMORYVIOLATION;
    uint64_t* TypePointer = (uint64_t*)Registers->arg1;
    size64_t* SizePointer = (size64_t*)Registers->arg2;
    *TypePointer = (uint64_t)memoryKey->Type;
    *SizePointer = (uint64_t)memoryKey->InitialSize;
    return KSUCCESS;
}

/* Sys_CreateProc :
    Arguments : 
*/
KResult Sys_CreateProc(SyscallStack* Registers, kthread_t* thread){
    kprocess_t* data;
    if(Registers->arg4 > PriviledgeApp){
        Registers->arg4 = PriviledgeApp;
    }
    if(globalTaskManager->CreateProcess(&data, (enum Priviledge)Registers->arg1, Registers->arg2) != KSUCCESS) return KFAIL;
    return Keyhole_Create((key_t*)Registers->arg0, data, thread->Parent, DataTypeProcess, (uint64_t)data, KeyholeFlagFullPermissions, PriviledgeApp);
}

/* Sys_CloseProc :
    Arguments : 
*/
KResult Sys_CloseProc(SyscallStack* Registers, kthread_t* thread){
    //TODO
    return KFAIL;
}

/* Sys_Close :
    Arguments : 
*/
KResult Sys_Close(SyscallStack* Registers, kthread_t* thread){
    KResult statu = thread->Close((ContextStack*)Registers, Registers->arg0);
    return statu;
}

/* Sys_Exit :
    Arguments : 
*/
KResult Sys_Exit(SyscallStack* Registers, kthread_t* thread){
    KResult statu = globalTaskManager->Exit((ContextStack*)Registers, thread, Registers->arg0);
    return statu;
}

/* Sys_Pause :
    Arguments : 
*/
KResult Sys_Pause(SyscallStack* Registers, kthread_t* thread){
    KResult statu = thread->Pause((ContextStack*)Registers, Registers->arg0);
    return statu;
    /* No return */
}

/* Sys_UnPause :
    Arguments : 
*/
KResult Sys_UnPause(SyscallStack* Registers, kthread_t* thread){
    kthread_t* threadkey;
    uint64_t flags;
    if(Keyhole_Get(thread, (key_t)Registers->arg0, DataTypethread, (uint64_t*)&threadkey, &flags) != KSUCCESS) return KKEYVIOLATION;
    if(!Keyhole_GetFlag(flags, KeyholeFlagDataTypethreadIsUnpauseable)) return KKEYVIOLATION;
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
KResult Sys_Map(SyscallStack* Registers, kthread_t* thread){
    if(Registers->arg2 && thread->Priviledge != PriviledgeDriver) return KNOTALLOW;
    kprocess_t* processkey;
    uint64_t flags;
    if(Keyhole_Get(thread, (key_t)Registers->arg0, DataTypeProcess, (uint64_t*)&processkey, &flags) != KSUCCESS) return KKEYVIOLATION;
    if(!Keyhole_GetFlag(flags, KeyholeFlagDataTypeProcessMemoryAccessible)) return KKEYVIOLATION;
    
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

    if(!CheckAddress((uintptr_t)addressVirtual, sizeof(uint64_t))) return KMEMORYVIOLATION;
    *addressVirtual = *addressVirtual - ((uint64_t)*addressVirtual % PAGE_SIZE);


    bool IsPhysicalAddress = false;
    if(CheckAddress((uintptr_t)addressPhysical, sizeof(uint64_t))){
        IsPhysicalAddress = true;
    }else{
        if(type == AllocationTypePhysical){
            return KMEMORYVIOLATION;
        }
    }

    if(!CheckAddress((uintptr_t)size, sizeof(uint64_t))) return KMEMORYVIOLATION;


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
        if(type == AllocationTypePhysical){
            for(uint64_t i = 0; i < pageCount; i++){
                if(vmm_GetFlags(pageTable, (uintptr_t)(*addressVirtual + i * PAGE_SIZE), vmm_flag::vmm_PhysicalStorage) && vmm_GetFlags(pageTable, (uintptr_t)(*addressVirtual + i * PAGE_SIZE), vmm_flag::vmm_Master)){
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
                    vmm_Map(pageTable, virtualAddress, physicalAddress, true, true, true);
                    vmm_SetFlags(pageTable, virtualAddress, vmm_flag::vmm_Master, true); //set master state
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
                    vmm_Map(pageTable, virtualAddress, (uintptr_t)((uint64_t)*addressPhysical + i * PAGE_SIZE), true, true, true);
                    vmm_SetFlags(pageTable, virtualAddress, vmm_flag::vmm_Master, false); //remove master state
                }else if(!vmm_GetFlags(pageTable, (uintptr_t)(*addressVirtual + i * PAGE_SIZE), vmm_flag::vmm_Master)){
                    uintptr_t physicalAddressAllocated = (uintptr_t)Pmm_RequestPage();
                    if(IsPhysicalAddress){
                        *addressPhysical = physicalAddressAllocated;
                        /* write only the first physicall page */
                        IsPhysicalAddress = false; 
                    }
                    vmm_Map(pageTable, virtualAddress, physicalAddressAllocated, true, true, true);
                    vmm_SetFlags(pageTable, virtualAddress, vmm_flag::vmm_Master, true); //set master state
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
KResult Sys_Unmap(SyscallStack* Registers, kthread_t* thread){
    kprocess_t* processkey;
    uint64_t flags;
    if(Keyhole_Get(thread, (key_t)Registers->arg0, DataTypeProcess, (uint64_t*)&processkey, &flags) != KSUCCESS) return KKEYVIOLATION;
    if(!Keyhole_GetFlag(flags, KeyholeFlagDataTypeProcessMemoryAccessible)) return KKEYVIOLATION;
    pagetable_t pageTable = processkey->SharedPaging;
    uintptr_t addressVirtual = (uintptr_t)Registers->arg1;
    size64_t size = Registers->arg2;

    addressVirtual = (uintptr_t)((uint64_t)addressVirtual - (uint64_t)addressVirtual % PAGE_SIZE);
    uint64_t pageCount = DivideRoundUp(size, PAGE_SIZE);
    if((uint64_t)addressVirtual + pageCount * PAGE_SIZE < vmm_HHDMAdress){
        for(uint64_t i = 0; i < pageCount; i += PAGE_SIZE){
            if(vmm_GetFlags(pageTable, (uintptr_t)addressVirtual, vmm_flag::vmm_Master)){
                if(vmm_GetFlags(pageTable, (uintptr_t)addressVirtual, vmm_flag::vmm_PhysicalStorage)){
                    Pmm_FreePage(vmm_GetPhysical(pageTable, addressVirtual));
                    processkey->MemoryAllocated -= PAGE_SIZE;
                }
                vmm_Unmap(pageTable, addressVirtual);                
            }
        }
    }
    return KSUCCESS;
}

/* Sys_Event_Create :
    Arguments : 
*/
KResult Sys_Event_Create(SyscallStack* Registers, kthread_t* thread){
    uint64_t data;
    if(Event::Create((kevent_t**)&data, EventTypeIPC, Registers->arg0) != KSUCCESS) return KFAIL;
    return Keyhole_Create((key_t*)Registers->arg0, thread->Parent, thread->Parent, DataTypeEvent, data, KeyholeFlagFullPermissions, PriviledgeApp);
}

/* Sys_Event_Bind :
    Arguments : 
*/
KResult Sys_Event_Bind(SyscallStack* Registers, kthread_t* thread){
    kevent_t* event; 
    kthread_t* threadkey;
    uint64_t flags;
    if(Keyhole_Get(thread, (key_t)Registers->arg0, DataTypeEvent, (uint64_t*)&event, &flags) != KSUCCESS) return KKEYVIOLATION;
    if(Keyhole_Get(thread, (key_t)Registers->arg1, DataTypethread, (uint64_t*)&threadkey, &flags) != KSUCCESS) return KKEYVIOLATION;
    if(!Keyhole_GetFlag(flags, KeyholeFlagDataTypethreadIsEventable)) return KKEYVIOLATION;
    return Event::Bind(threadkey, event, (bool)Registers->arg2);
}

/* Sys_Event_Unbind :
    Arguments : 
*/
KResult Sys_Event_Unbind(SyscallStack* Registers, kthread_t* thread){
    kevent_t* event;
    kthread_t* threadkey;
    uint64_t flags;
    if(Keyhole_Get(thread, (key_t)Registers->arg0, DataTypeEvent, (uint64_t*)&event, &flags) != KSUCCESS) return KKEYVIOLATION;
    if(Keyhole_Get(thread, (key_t)Registers->arg1, DataTypethread, (uint64_t*)&threadkey, &flags) != KSUCCESS) return KKEYVIOLATION;
    if(!Keyhole_GetFlag(flags, KeyholeFlagDataTypethreadIsBindable)) return KKEYVIOLATION;
    if(!Keyhole_GetFlag(flags, KeyholeFlagDataTypethreadIsEventable)) return KKEYVIOLATION;
    return Event::Unbind(thread, event);
}

/* Sys_kevent_trigger :
    Arguments : 
*/
KResult Sys_kevent_trigger(SyscallStack* Registers, kthread_t* thread){
    kevent_t* event; 
    uint64_t flags;
    if(Keyhole_Get(thread, (key_t)Registers->arg0, DataTypeEvent, (uint64_t*)&event, &flags) != KSUCCESS) return KKEYVIOLATION;
    if(!Keyhole_GetFlag(flags, KeyholeFlagDataTypethreadIsTriggerable)) return KKEYVIOLATION;
    if(CheckAddress((uintptr_t)Registers->arg1, sizeof(arguments_t))) return KMEMORYVIOLATION;
    return Event::Trigger(thread, event, (arguments_t*)Registers->arg1);
}

/* Sys_Event_Close :
    Arguments : 
*/
KResult Sys_Event_Close(SyscallStack* Registers, kthread_t* thread){
    return Event::Close((ContextStack*)Registers, thread);
}

/* Sys_Createthread :
    Arguments : 
*/
KResult Sys_CreateThread(SyscallStack* Registers, kthread_t* thread){
    kprocess_t* processkey;
    uint64_t flags;
    kthread_t* threadData;
    if(Keyhole_Get(thread, (key_t)Registers->arg0, DataTypeProcess, (uint64_t*)&processkey, &flags) != KSUCCESS) return KKEYVIOLATION;
    if(!Keyhole_GetFlag(flags, KeyholeFlagDataTypeProcessIsThreadCreateable)) return KKEYVIOLATION;
    if(globalTaskManager->Createthread(&threadData, processkey, (uintptr_t)Registers->arg1, Registers->arg2) != KSUCCESS) return KFAIL;
    return Keyhole_Create((key_t*)Registers->arg3, thread->Parent, thread->Parent, DataTypethread, (uint64_t)threadData, KeyholeFlagFullPermissions, PriviledgeApp);
}

/* Sys_Duplicatethread :
    Arguments : 
*/
KResult Sys_DuplicateThread(SyscallStack* Registers, kthread_t* thread){
    kprocess_t* processkey;
    kthread_t* threadkey;
    uint64_t flags;
    if(Keyhole_Get(thread, (key_t)Registers->arg0, DataTypeProcess, (uint64_t*)&processkey, &flags) != KSUCCESS) return KKEYVIOLATION;
    if(!Keyhole_GetFlag(flags, KeyholeFlagDataTypeProcessIsThreadCreateable)) return KKEYVIOLATION;
    if(Keyhole_Get(thread, (key_t)Registers->arg1, DataTypethread, (uint64_t*)&threadkey, &flags) != KSUCCESS) return KKEYVIOLATION;
    if(!Keyhole_GetFlag(flags, KeyholeFlagDataTypethreadIsDuplicable)) return KKEYVIOLATION;
    if(globalTaskManager->Duplicatethread(&thread, processkey, threadkey) != KSUCCESS) return KFAIL;     
    return Keyhole_Create((key_t*)Registers->arg2, thread->Parent, thread->Parent, DataTypethread, (uint64_t)thread, KeyholeFlagFullPermissions, PriviledgeApp);
}

/* Sys_Execthread :
    Arguments : 
*/
KResult Sys_ExecThread(SyscallStack* Registers, kthread_t* thread){
    if(thread->TID == 0x3){
        asm("nop");
    }
    kthread_t* threadkey;
    uint64_t flags;
    if(Keyhole_Get(thread, (key_t)Registers->arg0, DataTypethread, (uint64_t*)&threadkey, &flags) != KSUCCESS) return KKEYVIOLATION;
    enum ExecutionType Type = (enum ExecutionType)(Registers->arg2 & 0b11); // only the two first bits can be handle
    if(Type == ExecutionTypeQueu || Type == ExecutionTypeQueuAwait){
        if(!Keyhole_GetFlag(flags, KeyholeFlagDataTypethreadIsExecutableWithQueue)){
            return KKEYVIOLATION;
        }
    }
    if(Type == ExecutionTypeOneshot || Type == ExecutionTypeOneshotAwait){
        if(!Keyhole_GetFlag(flags, KeyholeFlagDataTypethreadIsExecutableOneshot)){
            return KKEYVIOLATION;
        }
    }

    if(Registers->arg1 != NULL){
        if(CheckAddress((uintptr_t)Registers->arg1, sizeof(arguments_t)) != KSUCCESS) return KMEMORYVIOLATION;    
    }
    ThreadShareData_t* Data = (ThreadShareData_t*)Registers->arg3;
    if(Data != NULL){
        if(CheckAddress((uintptr_t)Data, sizeof(ThreadShareData_t)) != KSUCCESS) return KMEMORYVIOLATION;    
        if(CheckAddress((uintptr_t)Data->Data, Data->Size) != KSUCCESS) return KMEMORYVIOLATION;    
    }
    KResult statu = globalTaskManager->Execthread(thread, threadkey, Type, (arguments_t*)Registers->arg1, Data, (ContextStack*)Registers);
    return statu;
}

/* Sys_Keyhole_CloneModify :
    Arguments : 
    0 -> string             > char*
    1 -> size               > size64_t
*/
KResult Sys_Keyhole_CloneModify(SyscallStack* Registers, kthread_t* thread){
    kprocess_t* processkey;
    uint64_t flags;
    if(Registers->arg2 != NULL){
        if(Keyhole_Get(thread, (key_t)Registers->arg2, DataTypeProcess, (uint64_t*)&processkey, &flags) != KSUCCESS) return KKEYVIOLATION;     
    }else{
        processkey = NULL;
    }
    if(CheckAddress((uintptr_t)Registers->arg1, sizeof(uint64_t)) != KSUCCESS) return KMEMORYVIOLATION;
    if(Registers->arg4 > PriviledgeApp){
        Registers->arg4 = PriviledgeApp;
    }
    return Keyhole_CloneModify(thread, (key_t)Registers->arg0, (key_t*)Registers->arg1, processkey, Registers->arg3, (enum Priviledge)Registers->arg4);
}

/* Sys_Keyhole_Verify :
    Arguments : 

*/
KResult Sys_Keyhole_Verify(SyscallStack* Registers, kthread_t* thread){
    if(CheckAddress((uintptr_t)Registers->arg2, sizeof(uint64_t)) != KSUCCESS) return KMEMORYVIOLATION;
    if(CheckAddress((uintptr_t)Registers->arg3, sizeof(uint64_t)) != KSUCCESS) return KMEMORYVIOLATION;
    if(CheckAddress((uintptr_t)Registers->arg4, sizeof(uint64_t)) != KSUCCESS) return KMEMORYVIOLATION;
    key_t key = Registers->arg0;
    uint64_t Statu = Keyhole_Verify(thread, key, (enum DataType)Registers->arg1);
    if(Statu != KSUCCESS) return Statu;
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

/* Sys_Logs :
    Arguments : 
    0 -> string             > char*
    1 -> size               > size64_t
*/
KResult Sys_Logs(SyscallStack* Registers, kthread_t* thread){
    if(CheckAddress((uintptr_t)Registers->arg0, Registers->arg1) != KSUCCESS) return KMEMORYVIOLATION;
    MessageProcess((char*)Registers->arg0, Registers->arg1, thread->Parent->PID, thread->TID);
    return KSUCCESS;
}

static SyscallHandler SyscallHandlers[Syscall_Count] = { 
    [KSys_CreateMemoryField] = Sys_CreateMemoryField,
    [KSys_AcceptMemoryField] = Sys_AcceptMemoryField,
    [KSys_CloseMemoryField] = Sys_CloseMemoryField,
    [KSys_GetTypeMemoryField] = Sys_GetInfoMemoryField,
    [KSys_CreateProc] = Sys_CreateProc,
    [KSys_CloseProc] = Sys_CloseProc,
    [KSys_Close] = Sys_Close,
    [KSys_Exit] = Sys_Exit,
    [KSys_Pause] = Sys_Pause,
    [KSys_UnPause] = Sys_UnPause,
    [KSys_Map] = Sys_Map,
    [KSys_Unmap] = Sys_Unmap,
    [KSys_Event_Create] = Sys_Event_Create,
    [KSys_Event_Bind] = Sys_Event_Bind,
    [KSys_Event_Unbind] = Sys_Event_Unbind,
    [KSys_kevent_trigger] = Sys_kevent_trigger,
    [KSys_Event_Close] = Sys_Event_Close,
    [KSys_CreateThread] = Sys_CreateThread,
    [KSys_DuplicateThread] = Sys_DuplicateThread,
    [KSys_ExecThread] = Sys_ExecThread,
    [KSys_Keyhole_CloneModify] = Sys_Keyhole_CloneModify,
    [KSys_Keyhole_Verify] = Sys_Keyhole_Verify,
    [KSys_Logs] = Sys_Logs,
};

extern "C" void SyscallDispatch(SyscallStack* Registers, kthread_t* Self){
    if(Registers->GlobalPurpose >= Syscall_Count){
        Registers->arg0 = KFAIL;
        return;
    }

    Registers->GlobalPurpose = SyscallHandlers[Registers->GlobalPurpose](Registers, Self);
    return;
}