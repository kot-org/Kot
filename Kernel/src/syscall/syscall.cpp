#include <syscall/syscall.h>

KResult Sys_CreatShareMemory(ContextStack* Registers, thread_t* Thread){
    thread_t* threadkey;
    uint64_t flags;
    uint64_t data;
    if(Keyhole::Get(Thread, (key_t)Registers->arg0, DataTypeThread, (uint64_t*)&threadkey, &flags) != KSUCCESS) return KFAIL;
    if(CreatSharing(threadkey, Registers->arg1, (uint64_t*)Registers->arg2, &data, (bool)Registers->arg4) != KSUCCESS) return KFAIL;
    return Keyhole::Creat((key_t*)Registers->arg3, Thread->Parent, NULL, DataTypeMemory, data, FlagFullPermissions);
}

KResult Sys_GetShareMemory(ContextStack* Registers, thread_t* Thread){
    thread_t* threadkey;
    uint64_t memoryKey;
    uint64_t flags;
    if(Keyhole::Get(Thread, (key_t)Registers->arg0, DataTypeThread, (uint64_t*)&threadkey, &flags) != KSUCCESS) return KFAIL;
    if(Keyhole::Get(Thread, (key_t)Registers->arg1, DataTypeMemory, (uint64_t*)&memoryKey, &flags) != KSUCCESS) return KFAIL;
    return GetSharing(threadkey, memoryKey, (uint64_t*)Registers->arg2);
}

KResult Sys_FreeShareMemory(ContextStack* Registers, thread_t* Thread){
    thread_t* threadkey;
    uint64_t memoryKey;
    uint64_t flags;
    if(Keyhole::Get(Thread, (key_t)Registers->arg0, DataTypeThread, (uint64_t*)&threadkey, &flags) != KSUCCESS) return KFAIL;
    if(Keyhole::Get(Thread, (key_t)Registers->arg1, DataTypeMemory, (uint64_t*)&memoryKey, &flags) != KSUCCESS) return KFAIL;
    return FreeSharing(threadkey, memoryKey);    
}

KResult Sys_Get_IOPL(ContextStack* Registers, thread_t* Thread){

}

KResult Sys_Fork(ContextStack* Registers, thread_t* Thread){
    thread_t* threadkey;
    uint64_t flags;
    if(Keyhole::Get(Thread, (key_t)Registers->arg0, DataTypeThread, (uint64_t*)&threadkey, &flags) != KSUCCESS) return KFAIL;
    Registers->InterruptNumber = 1;
    CPU::DisableInterrupts();
    Thread->Fork(Registers, Thread->CoreID, threadkey, (Parameters*)Registers->arg1);
    return KSUCCESS;
}

KResult Sys_CreatProc(ContextStack* Registers, thread_t* Thread){
    process_t* data;
    if(globalTaskManager->CreatProcess(&data, (uint8_t)Registers->arg1, (void*)Registers->arg2) != KSUCCESS) return KFAIL;
    return Keyhole::Creat((key_t*)Registers->arg0, data, Thread->Parent, DataTypeProcess, (uint64_t)data, FlagFullPermissions);
}

KResult Sys_CloseProc(ContextStack* Registers, thread_t* Thread){
    //TODO
    return KFAIL;
}

KResult Sys_Exit(ContextStack* Registers, thread_t* Thread){
    thread_t* threadkey;
    uint64_t flags;
    if(Keyhole::Get(Thread, (key_t)Registers->arg0, DataTypeThread, (uint64_t*)&threadkey, &flags) != KSUCCESS) return KFAIL;
    CPU::DisableInterrupts();
    Registers->InterruptNumber = 1;
    globalTaskManager->Exit(Registers, Thread->CoreID, threadkey);
    return KSUCCESS;
}

KResult Sys_Pause(ContextStack* Registers, thread_t* Thread){
    thread_t* threadkey;
    uint64_t flags;
    if(Keyhole::Get(Thread, (key_t)Registers->arg0, DataTypeThread, (uint64_t*)&threadkey, &flags) != KSUCCESS) return KFAIL;
    CPU::DisableInterrupts();
    Registers->InterruptNumber = 1;
    globalTaskManager->Pause(Registers, Thread->CoreID, threadkey);
    globalTaskManager->Scheduler(Registers, Thread->CoreID);
    return KSUCCESS;
}

KResult Sys_UnPause(ContextStack* Registers, thread_t* Thread){
    thread_t* threadkey;
    uint64_t flags;
    if(Keyhole::Get(Thread, (key_t)Registers->arg0, DataTypeThread, (uint64_t*)&threadkey, &flags) != KSUCCESS) return KFAIL;
    return globalTaskManager->Unpause(threadkey);
}

/* Sys_Map :
    Arguments : 
    0 -> process            > key
    1 -> virtual address    > uint64_t*
    2 -> physicall or not   > bool
    3 -> physical address   > void*
    4 -> size               > size_t
    5 -> find free address  > bool
*/
KResult Sys_Map(ContextStack* Registers, thread_t* Thread){
    process_t* processkey;
    uint64_t flags;
    if(Keyhole::Get(Thread, (key_t)Registers->arg0, DataTypeProcess, (uint64_t*)&processkey, &flags) != KSUCCESS) return KFAIL;
    pagetable_t pageTable = processkey->SharedPaging;
    
    uint64_t* addressVirtual = (uint64_t*)Registers->arg1;
    void* addressPhysical = (void*)Registers->arg3;
    size_t size = Registers->arg4;
    bool IsNeedToBeFree = (bool)Registers->arg5; 

    *addressVirtual = *addressVirtual - ((uint64_t)*addressVirtual % PAGE_SIZE);
    uint64_t pages = Divide(size, PAGE_SIZE);
    /* find free page */
    if(IsNeedToBeFree){
        uint64_t FreeSize = 0;
        for(uint64_t FreeSize = 0; FreeSize < size;){
            while(vmm_GetFlags(pageTable, (void*)*addressVirtual, vmm_flag::vmm_Present)){
                *addressVirtual += PAGE_SIZE;
                FreeSize = 0;
            }
            FreeSize += PAGE_SIZE;
        }
    }

    if((uint64_t)addressVirtual + pages * PAGE_SIZE < vmm_HHDMAdress){
        for(uint64_t i = 0; i < pages; i += PAGE_SIZE){
            if(vmm_GetFlags(pageTable, (void*)addressVirtual, vmm_flag::vmm_Custom1)){
                Pmm_FreePage(vmm_GetPhysical(pageTable, addressVirtual));
            }
            vmm_Unmap(pageTable, addressVirtual);
        }
        
        for(uint64_t i = 0; i < pages; i += PAGE_SIZE){
            if((bool)Registers->arg2){
                vmm_Map(pageTable, (void*)*addressVirtual + i, addressPhysical + i);
            }else{
                vmm_Map(pageTable, (void*)*addressVirtual + i, Pmm_RequestPage());
                vmm_SetFlags(pageTable, (void*)*addressVirtual, vmm_flag::vmm_Custom1, true); //set master state
            }        
        } 
    }

    return KSUCCESS;
}

/* Sys_Unmap :
    Arguments : 
    0 -> process            > key
    1 -> virtual address    > uint64_t*
    4 -> size               > size_t
*/
KResult Sys_Unmap(ContextStack* Registers, thread_t* Thread){
    process_t* processkey;
    uint64_t flags;
    if(Keyhole::Get(Thread, (key_t)Registers->arg0, DataTypeProcess, (uint64_t*)&processkey, &flags) != KSUCCESS) return KFAIL;
    pagetable_t pageTable = processkey->SharedPaging;
    void* addressVirtual = (void*)Registers->arg1;
    size_t size = Registers->arg2;

    addressVirtual = (void*)(addressVirtual - (uint64_t)addressVirtual % PAGE_SIZE);
    uint64_t pages = Divide(size, PAGE_SIZE);
    if((uint64_t)addressVirtual + pages * PAGE_SIZE < vmm_HHDMAdress){
        for(uint64_t i = 0; i < pages; i += PAGE_SIZE){
            if(vmm_GetFlags(pageTable, (void*)addressVirtual, vmm_flag::vmm_Custom1)){
                Pmm_FreePage(vmm_GetPhysical(pageTable, addressVirtual));
            }
            vmm_Unmap(pageTable, addressVirtual);
        }
    }
    return KSUCCESS;
}

KResult Sys_Event_Creat(ContextStack* Registers, thread_t* Thread){
    uint64_t data;
    if(Event::Creat((event_t**)&data, EventTypeIPC, Registers->arg1) != KSUCCESS) return KFAIL;
    return Keyhole::Creat((key_t*)Registers->arg0, Thread->Parent, Thread->Parent, DataTypeEvent, data, FlagFullPermissions);
}

KResult Sys_Event_Bind(ContextStack* Registers, thread_t* Thread){
    event_t* event; 
    uint64_t flags;
    if(Keyhole::Get(Thread, (key_t)Registers->arg0, DataTypeEvent, (uint64_t*)&event, &flags) != KSUCCESS) return KFAIL;
    return Event::Bind(Thread, event);
}

KResult Sys_Event_Unbind(ContextStack* Registers, thread_t* Thread){

}

KResult Sys_Event_Trigger(ContextStack* Registers, thread_t* Thread){

}

KResult Sys_CreatThread(ContextStack* Registers, thread_t* Thread){

}

KResult Sys_DuplicateThread(ContextStack* Registers, thread_t* Thread){

}

KResult Sys_ExecThread(ContextStack* Registers, thread_t* Thread){

}


static SyscallHandler SyscallHandlers[Syscall_Count] = { 
    [KSys_CreatShareMemory] = Sys_CreatShareMemory,
    [KSys_GetShareMemory] = Sys_GetShareMemory,
    [KSys_FreeShareMemory] = Sys_FreeShareMemory,
    [KSys_Get_IOPL] = Sys_Get_IOPL,
    [KSys_Fork] = Sys_CreatProc,
    [KSys_CreatProc] = Sys_CreatProc,
    [KSys_CloseProc] = Sys_CloseProc,
    [KSys_Exit] = Sys_Exit,
    [KSys_Pause] = Sys_Pause,
    [KSys_UnPause] = Sys_UnPause,
    [KSys_Map] = Sys_Map,
    [KSys_Unmap] = Sys_Unmap,
    [KSys_Event_Creat] = Sys_Event_Creat,
    [KSys_Event_Bind] = Sys_Event_Bind,
    [KSys_Event_Unbind] = Sys_Event_Unbind,
    [KSys_Event_Trigger] = Sys_Event_Trigger,
    [KSys_CreatThread] = Sys_CreatThread,
    [KSys_DuplicateThread] = Sys_DuplicateThread,
    [KSys_ExecThread] = Sys_ExecThread,
};

extern "C" uint64_t SyscallDispatch(ContextStack* Registers, thread_t* Self){
    globalLogs->Warning("Syscall %x", Registers->GlobalPurpose);
    if(Registers->GlobalPurpose >= Syscall_Count){
        Registers->arg0 = KFAIL;
        return GDTInfoSelectorsRing[UserAppRing].Code;        
    }

    Registers->GlobalPurpose = SyscallHandlers[Registers->GlobalPurpose](Registers, Self);

    return GDTInfoSelectorsRing[UserAppRing].Code; 

    /*switch(syscall){
        case Sys_Event_Bind:
            returnValue = Event::Bind((thread_t*)arg0, (event_t*)arg1);
            break;
        case Sys_Event_Unbind:
            returnValue = Event::Unbind((thread_t*)arg0, (event_t*)arg1);
            break;
        case Sys_Event_Trigger:
            returnValue = Event::Trigger(threadkey, (event_t*)arg0, (void*)arg1, (size_t)arg2);
            break;
        case Sys_CreatThread:
            returnValue = globalTaskManager->CreatThread((thread_t**)arg0, (process_t*)arg1, arg2, (void*)arg3);
            break;
        case Sys_DuplicateThread:
            returnValue = globalTaskManager->DuplicateThread((thread_t**)arg0, (process_t*)arg1, (thread_t*)arg2);
            break;
        case Sys_ExecThread:
            returnValue = globalTaskManager->ExecThread((thread_t*)arg0, (Parameters*)arg1);
            break;
        case Sys_Get_IOPL:
            if(threadkey->RingPL <= DevicesRing){
                returnValue = threadkey->SetIOPriviledge((ContextStack*)Registers, (uint8_t)arg0);
            }
            break;
        default:
            globalLogs->Error("Unknown syscall %x", syscall);
            break;
    }*/   
}