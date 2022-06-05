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

/* Sys_CreatShareMemory :
    Arguments : 
    0 -> process taget                  > key process
    1 -> size                           > uint64_t
    2 -> pointer to virtual address     > uint64_t*
    3 -> return                         > key shared memory
    4 -> reserved                       > none
    5 -> reserved                       > none
*/
KResult Sys_CreatShareMemory(ContextStack* Registers, thread_t* Thread){
    process_t* processkey;
    uint64_t flags;
    uint64_t data;
    if(Keyhole_Get(Thread, (key_t)Registers->arg0, DataTypeProcess, (uint64_t*)&processkey, &flags) != KSUCCESS) return KKEYVIOLATION;
    if(CreatSharing(processkey, Registers->arg1, (uint64_t*)Registers->arg2, &data, Registers->arg4) != KSUCCESS) return KFAIL;
    return Keyhole_Creat((key_t*)Registers->arg3, Thread->Parent, NULL, DataTypeSharedMemory, data, FlagFullPermissions);
}

/* Sys_GetShareMemory :
    Arguments : 
*/
KResult Sys_GetShareMemory(ContextStack* Registers, thread_t* Thread){
    process_t* processkey;
    MemoryShareInfo* memoryKey;
    uint64_t flags;
    if(Keyhole_Get(Thread, (key_t)Registers->arg0, DataTypeProcess, (uint64_t*)&processkey, &flags) != KSUCCESS) return KKEYVIOLATION;
    if(Keyhole_Get(Thread, (key_t)Registers->arg1, DataTypeSharedMemory, (uint64_t*)&memoryKey, &flags) != KSUCCESS) return KKEYVIOLATION;
    return GetSharing(processkey, memoryKey, (uint64_t*)Registers->arg2);
}

/* Sys_FreeShareMemory :
    Arguments : 
*/
KResult Sys_FreeShareMemory(ContextStack* Registers, thread_t* Thread){
    process_t* processkey;
    MemoryShareInfo* memoryKey;
    uint64_t flags;
    if(Keyhole_Get(Thread, (key_t)Registers->arg0, DataTypeProcess, (uint64_t*)&processkey, &flags) != KSUCCESS) return KKEYVIOLATION;
    if(Keyhole_Get(Thread, (key_t)Registers->arg1, DataTypeSharedMemory, (uint64_t*)&memoryKey, &flags) != KSUCCESS) return KKEYVIOLATION;
    return FreeSharing(processkey, memoryKey, (uintptr_t)Registers->arg2);    
}

/* Sys_ShareDataUsingStackSpace :
    Arguments :
    0 -> thread taget                   > key thread
    1 -> data address                   > uint64_t
    2 -> size of data                   > size_t
    3 -> location of data for client    > uint64_t*
    4 -> reserved                       > none
    5 -> reserved                       > none

*/
KResult Sys_ShareDataUsingStackSpace(ContextStack* Registers, thread_t* Thread){
    thread_t* threadkey;
    uint64_t flags;
    if(Keyhole_Get(Thread, (key_t)Registers->arg0, DataTypeThread, (uint64_t*)&threadkey, &flags) != KSUCCESS) return KKEYVIOLATION;
    if(CheckAddress((uintptr_t)Registers->arg1, Registers->arg2) != KSUCCESS) return KMEMORYVIOLATION;
    if(CheckAddress((uintptr_t)Registers->arg3, sizeof(uint64_t)) != KSUCCESS) return KMEMORYVIOLATION;
    return globalTaskManager->ShareDataUsingStackSpace(threadkey, (uintptr_t)Registers->arg1, Registers->arg2, (uint64_t*)Registers->arg3);
}

/* Sys_CIP :
    Arguments : 
*/
KResult Sys_CIP(ContextStack* Registers, thread_t* Thread){
    thread_t* threadkey;
    uint64_t flags;
    if(Keyhole_Get(Thread, (key_t)Registers->arg0, DataTypeThread, (uint64_t*)&threadkey, &flags) != KSUCCESS) return KKEYVIOLATION;
    Registers->InterruptNumber = 1;
    CPU::DisableInterrupts();
    Thread->CIP(Registers, Thread->CoreID, threadkey, (parameters_t*)Registers->arg1);
    return KSUCCESS;
}

/* Sys_CreatProc :
    Arguments : 
*/
KResult Sys_CreatProc(ContextStack* Registers, thread_t* Thread){
    process_t* data;
    if(globalTaskManager->CreatProcess(&data, (uint8_t)Registers->arg1, Registers->arg2) != KSUCCESS) return KFAIL;
    return Keyhole_Creat((key_t*)Registers->arg0, data, Thread->Parent, DataTypeProcess, (uint64_t)data, FlagFullPermissions);
}

/* Sys_CloseProc :
    Arguments : 
*/
KResult Sys_CloseProc(ContextStack* Registers, thread_t* Thread){
    //TODO
    return KFAIL;
}

/* Sys_Exit :
    Arguments : 
*/
KResult Sys_Exit(ContextStack* Registers, thread_t* Thread){
    thread_t* threadkey;
    uint64_t flags;
    if(Keyhole_Get(Thread, (key_t)Registers->arg0, DataTypeThread, (uint64_t*)&threadkey, &flags) != KSUCCESS) return KKEYVIOLATION;
    Registers->InterruptNumber = 1;
    globalTaskManager->Exit(Registers, Thread->CoreID, threadkey);
    globalLogs->Message("Thread 0x%x exit with error code : 0x%x", threadkey->TID, Registers->arg1);
    return KSUCCESS;
}

/* Sys_Pause :
    Arguments : 
*/
KResult Sys_Pause(ContextStack* Registers, thread_t* Thread){
    thread_t* threadkey;
    uint64_t flags;
    if(Keyhole_Get(Thread, (key_t)Registers->arg0, DataTypeThread, (uint64_t*)&threadkey, &flags) != KSUCCESS) return KKEYVIOLATION;
    CPU::DisableInterrupts();
    Registers->InterruptNumber = 1;
    globalTaskManager->Pause(Registers, Thread->CoreID, threadkey);
    return KSUCCESS;
}

/* Sys_UnPause :
    Arguments : 
*/
KResult Sys_UnPause(ContextStack* Registers, thread_t* Thread){
    thread_t* threadkey;
    uint64_t flags;
    if(Keyhole_Get(Thread, (key_t)Registers->arg0, DataTypeThread, (uint64_t*)&threadkey, &flags) != KSUCCESS) return KKEYVIOLATION;
    return globalTaskManager->Unpause(threadkey);
}

/* Sys_Map :
    Arguments : 
    0 -> process            > key
    1 -> virtual address    > uint64_t*
    2 -> physicall or not   > bool
    3 -> physical address   > uintptr_t
    4 -> size               > size_t
    5 -> find free address  > bool
*/
KResult Sys_Map(ContextStack* Registers, thread_t* Thread){
    if(Registers->arg2 && Thread->Priviledge != Priviledge_Driver) return KNOTALLOW;
    process_t* processkey;
    uint64_t flags;
    if(Keyhole_Get(Thread, (key_t)Registers->arg0, DataTypeProcess, (uint64_t*)&processkey, &flags) != KSUCCESS) return KKEYVIOLATION;
    pagetable_t pageTable = processkey->SharedPaging;
    
    uint64_t* addressVirtual = (uint64_t*)Registers->arg1;
    if(!CheckAddress((uintptr_t)addressVirtual, sizeof(uint64_t))) return KMEMORYVIOLATION;
    uintptr_t addressPhysical = (uintptr_t)Registers->arg3;
    size_t size = Registers->arg4;
    bool IsNeedToBeFree = (bool)Registers->arg5; 

    *addressVirtual = *addressVirtual - ((uint64_t)*addressVirtual % PAGE_SIZE);
    uint64_t pages = DivideRoundUp(size, PAGE_SIZE);
    /* find free page */
    if(IsNeedToBeFree){
        for(uint64_t FreeSize = 0; FreeSize < size;){
            bool IsPresent = vmm_GetFlags(pageTable, (uintptr_t)*addressVirtual, vmm_flag::vmm_Present);
            while(IsPresent){
                *addressVirtual += PAGE_SIZE;
                IsPresent = vmm_GetFlags(pageTable, (uintptr_t)*addressVirtual, vmm_flag::vmm_Present);
                FreeSize = 0;
            }
            FreeSize += PAGE_SIZE;
        }
    }

    if(*addressVirtual + pages * PAGE_SIZE < vmm_HHDMAdress){
        for(uint64_t i = 0; i < pages; i++){
            if(vmm_GetFlags(pageTable, (uintptr_t)*addressVirtual + i * PAGE_SIZE, vmm_flag::vmm_PhysicalStorage)){
                Pmm_FreePage(vmm_GetPhysical(pageTable, (uintptr_t)*addressVirtual + i * PAGE_SIZE));
            }
            vmm_Unmap(pageTable, (uintptr_t)*addressVirtual + i * PAGE_SIZE);
        }
        
        for(uint64_t i = 0; i < pages; i++){
            uintptr_t virtualAddress = (uintptr_t)(*addressVirtual + i * PAGE_SIZE);
            if((bool)Registers->arg2){
                vmm_Map(pageTable, virtualAddress, addressPhysical + i * PAGE_SIZE);
            }else{
                vmm_Map(pageTable, virtualAddress, Pmm_RequestPage(), true);
                vmm_SetFlags(pageTable, virtualAddress, vmm_flag::vmm_PhysicalStorage, true); //set master state
                processkey->MemoryAllocated += PAGE_SIZE;
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
    4 -> size               > size_t
*/
KResult Sys_Unmap(ContextStack* Registers, thread_t* Thread){
    process_t* processkey;
    uint64_t flags;
    if(Keyhole_Get(Thread, (key_t)Registers->arg0, DataTypeProcess, (uint64_t*)&processkey, &flags) != KSUCCESS) return KKEYVIOLATION;
    pagetable_t pageTable = processkey->SharedPaging;
    uintptr_t addressVirtual = (uintptr_t)Registers->arg1;
    size_t size = Registers->arg2;

    addressVirtual = (uintptr_t)(addressVirtual - (uint64_t)addressVirtual % PAGE_SIZE);
    uint64_t pages = DivideRoundUp(size, PAGE_SIZE);
    if((uint64_t)addressVirtual + pages * PAGE_SIZE < vmm_HHDMAdress){
        for(uint64_t i = 0; i < pages; i += PAGE_SIZE){
            if(vmm_GetFlags(pageTable, (uintptr_t)addressVirtual, vmm_flag::vmm_PhysicalStorage)){
                Pmm_FreePage(vmm_GetPhysical(pageTable, addressVirtual));
                processkey->MemoryAllocated -= PAGE_SIZE;
            }
            vmm_Unmap(pageTable, addressVirtual);
        }
    }
    return KSUCCESS;
}

/* Sys_Event_Creat :
    Arguments : 
*/
KResult Sys_Event_Creat(ContextStack* Registers, thread_t* Thread){
    uint64_t data;
    if(Event::Creat((event_t**)&data, (EventType)Registers->arg1, Registers->arg2) != KSUCCESS) return KFAIL;
    return Keyhole_Creat((key_t*)Registers->arg0, Thread->Parent, Thread->Parent, DataTypeEvent, data, FlagFullPermissions);
}

/* Sys_Event_Bind :
    Arguments : 
*/
KResult Sys_Event_Bind(ContextStack* Registers, thread_t* Thread){
    event_t* event; 
    uint64_t flags;
    if(Keyhole_Get(Thread, (key_t)Registers->arg0, DataTypeEvent, (uint64_t*)&event, &flags) != KSUCCESS) return KKEYVIOLATION;
    return Event::Bind(Thread, event);
}

/* Sys_Event_Unbind :
    Arguments : 
*/
KResult Sys_Event_Unbind(ContextStack* Registers, thread_t* Thread){
    event_t* event; 
    uint64_t flags;
    if(Keyhole_Get(Thread, (key_t)Registers->arg0, DataTypeEvent, (uint64_t*)&event, &flags) != KSUCCESS) return KKEYVIOLATION;
    return Event::Unbind(Thread, event);
}

/* Sys_Event_Trigger :
    Arguments : 
*/
KResult Sys_Event_Trigger(ContextStack* Registers, thread_t* Thread){
    event_t* event; 
    uint64_t flags;
    if(Keyhole_Get(Thread, (key_t)Registers->arg0, DataTypeEvent, (uint64_t*)&event, &flags) != KSUCCESS) return KKEYVIOLATION;
    if(CheckAddress((uintptr_t)Registers->arg1, Registers->arg2) != KSUCCESS) return KMEMORYVIOLATION;
    return Event::Trigger(Thread, event, (uintptr_t)Registers->arg1, (size_t)Registers->arg2);
}

/* Sys_CreatThread :
    Arguments : 
*/
KResult Sys_CreatThread(ContextStack* Registers, thread_t* Thread){
    process_t* processkey;
    uint64_t flags;
    thread_t* thread;
    if(Keyhole_Get(Thread, (key_t)Registers->arg0, DataTypeProcess, (uint64_t*)&processkey, &flags) != KSUCCESS) return KKEYVIOLATION;
    if(globalTaskManager->CreatThread(&thread, processkey, (uintptr_t)Registers->arg1, Registers->arg2, Registers->arg3) != KSUCCESS) return KFAIL;
    return Keyhole_Creat((key_t*)Registers->arg4, Thread->Parent, Thread->Parent, DataTypeThread, (uint64_t)thread, FlagFullPermissions);
}

/* Sys_DuplicateThread :
    Arguments : 
*/
KResult Sys_DuplicateThread(ContextStack* Registers, thread_t* Thread){
    process_t* processkey;
    thread_t* threadkey;
    uint64_t flags;
    thread_t* thread;
    if(Keyhole_Get(Thread, (key_t)Registers->arg0, DataTypeProcess, (uint64_t*)&processkey, &flags) != KSUCCESS) return KKEYVIOLATION;
    if(Keyhole_Get(Thread, (key_t)Registers->arg1, DataTypeThread, (uint64_t*)&threadkey, &flags) != KSUCCESS) return KKEYVIOLATION;
    if(globalTaskManager->DuplicateThread(&thread, processkey, threadkey, Registers->arg2) != KSUCCESS) return KFAIL;     
    return Keyhole_Creat((key_t*)Registers->arg3, Thread->Parent, Thread->Parent, DataTypeThread, (uint64_t)thread, FlagFullPermissions);
}

/* Sys_ExecThread :
    Arguments : 
*/
KResult Sys_ExecThread(ContextStack* Registers, thread_t* Thread){
    thread_t* threadkey;
    uint64_t flags;
    if(Keyhole_Get(Thread, (key_t)Registers->arg0, DataTypeThread, (uint64_t*)&threadkey, &flags) != KSUCCESS) return KKEYVIOLATION;
    if(CheckAddress((uintptr_t)Registers->arg1, sizeof(Parameters)) != KSUCCESS) return KMEMORYVIOLATION;    
    return globalTaskManager->ExecThread(threadkey, (parameters_t*)Registers->arg1);
}

/* KSys_Logs :
    Arguments : 
    0 -> string             > char*
    1 -> size               > size_t
*/
KResult Sys_Logs(ContextStack* Registers, thread_t* Thread){
    if(CheckAddress((uintptr_t)Registers->arg0, Registers->arg1) != KSUCCESS) return KMEMORYVIOLATION;
    char* message = (char*)malloc(Registers->arg1 + 1);
    memcpy(message, (uintptr_t)Registers->arg0, Registers->arg1);
    message[Registers->arg1] = NULL;
    globalLogs->Message("[Process 0x%x] '%s'", Thread->Parent->PID, message);
    free(message);
    return KSUCCESS;
}

static SyscallHandler SyscallHandlers[Syscall_Count] = { 
    [KSys_CreatShareMemory] = Sys_CreatShareMemory,
    [KSys_GetShareMemory] = Sys_GetShareMemory,
    [KSys_FreeShareMemory] = Sys_FreeShareMemory,
    [KSys_ShareDataUsingStackSpace] = Sys_ShareDataUsingStackSpace,
    [KSys_CIP] = Sys_CIP,
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
    [KSys_Logs] = Sys_Logs,
};

extern "C" void SyscallDispatch(ContextStack* Registers, thread_t* Self){
    if(Registers->GlobalPurpose >= Syscall_Count){
        Registers->arg0 = KFAIL;
        return;        
    }

    Registers->GlobalPurpose = SyscallHandlers[Registers->GlobalPurpose](Registers, Self);

    return;
}