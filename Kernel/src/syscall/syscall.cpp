#include <syscall/syscall.h>

static uint64_t mutexSyscall;

extern "C" uint64_t SyscallHandler(ContextStack* Registers, uint64_t CoreID){
    Atomic::atomicSpinlock(&mutexSyscall, 0);
    Atomic::atomicLock(&mutexSyscall, 0);

    uint64_t syscall = Registers->rax;
    uint64_t arg0 = Registers->rdi;
    uint64_t arg1 = Registers->rsi;
    uint64_t arg2 = Registers->rdx;
    uint64_t arg3 = Registers->r10;
    uint64_t arg4 = Registers->r8;
    uint64_t arg5 = Registers->r9;
    uint64_t returnValue = 0;
    thread_t* thread = globalTaskManager->ThreadExecutePerCore[CoreID];

    switch(syscall){
        case Sys_CreatShareMemory:
            //creat share memory
            returnValue = CreatSharing((thread_t*)arg0, arg1, (uint64_t*)arg2, (uint64_t*)arg3, (bool)arg4);
            //this function return the allocated size
            break;
        case Sys_GetShareMemory:
            //get share memory
            returnValue = GetSharing((thread_t*)arg0, (MemoryShareInfo*)arg1, (uint64_t*)arg2);
            break;
        case Sys_FreeShareMemory:
            returnValue = FreeSharing((thread_t*)arg0, (void*)arg1);
            break;
        case Sys_StackShareMemory:
            returnValue = globalTaskManager->ShareDataInStack((uint64_t**)arg0, (thread_t*)arg1, (void*)arg2, (size_t)arg3);
            break;
        case Sys_Fork: 
            thread->Fork(Registers, CoreID, (thread_t*)arg0, (Parameters*)arg1);
            Atomic::atomicUnlock(&mutexSyscall, 0);
            return 0xff;
        case Sys_CreatProc: 
            returnValue = globalTaskManager->CreatProcess((process_t**)arg0, (uint8_t)arg1, (void*)arg2);
            break;
        case Sys_CloseProc:
            returnValue = KFAIL; 
            //TODO
            return 0xff;
        case Sys_Exit:
            //exit
            globalLogs->Warning("Thread %x exit with error code : %x", thread->TID, arg0);
            globalTaskManager->Exit(Registers, CoreID, (thread_t*)arg0);
            Atomic::atomicUnlock(&mutexSyscall, 0);
            return 0xff;
        case Sys_Pause:
            globalLogs->Warning("Thread %x is paused in process %x", thread->TID, thread->Parent->PID);
            globalTaskManager->Pause(Registers, CoreID, (thread_t*)arg0);
            globalTaskManager->Scheduler(Registers, CoreID);
            
            Atomic::atomicUnlock(&mutexSyscall, 0);
            return 0xff;            
        case Sys_UnPause:
            returnValue = globalTaskManager->Unpause((thread_t*)arg0);
            break;
        case Sys_Map:
            //mmap
            returnValue = mmap((thread_t*)arg0), (void*)arg1, (bool)arg2, (void*)arg3);    
            break;
        case Sys_Unmap:
            //munmap
            returnValue = munmap((thread_t*)arg0), (void*)arg1);  
            break;
        case Sys_Event_Creat:
            returnValue = Event::Creat((event_t**)arg0, EventTypeIPC, arg1);
            break;
        case Sys_Event_Bind:
            returnValue = Event::Bind((thread_t*)arg0, (event_t*)arg1);
            break;
        case Sys_Event_Unbind:
            returnValue = Event::Bind((thread_t*)arg0, (event_t*)arg1);
            break;
        case Sys_Event_Trigger:
            returnValue = Event::Trigger(thread, (event_t*)arg0, (void*)arg1, (size_t)arg2);
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
            if(thread->RingPL <= DevicesRing){
                returnValue = thread->SetIOPriviledge((ContextStack*)Registers, (uint8_t)arg0);
            }
            break;
        default:
            globalLogs->Error("Unknown syscall %x", syscall);
            break;
    }

    Registers->rdi = returnValue;

    Atomic::atomicUnlock(&mutexSyscall, 0); 

    if(Registers->cs != GDTInfoSelectorsRing[UserAppRing].Code){
        return 0xff; 
    }else{
        return 0x0; 
    }    
}



uint64_t mmap(thread_t* task, void* addressVirtual, bool usePhysicallAddress, void* addressPhysical){
    PageTableManager* pageTable = task->Paging;
    addressVirtual = (void*)(addressVirtual - (uint64_t)addressVirtual % 0x1000);
    if((uint64_t)addressVirtual < HigherHalfAddress){
        if(usePhysicallAddress){
            pageTable->MapMemory(addressVirtual, addressPhysical);
        }else{
            pageTable->MapMemory(addressVirtual, globalAllocator.RequestPage());
            pageTable->SetFlags((void*)addressVirtual, PT_Flag::Custom1, true); //set slave state
        }        
    }
    return KSUCCESS;
}

uint64_t munmap(thread_t* task, void* addressVirtual){
    PageTableManager* pageTable = task->Paging;
    addressVirtual = (void*)(addressVirtual - (uint64_t)addressVirtual % 0x1000);
    if((uint64_t)addressVirtual < HigherHalfAddress){
        if(pageTable->GetFlags((void*)addressVirtual, PT_Flag::Custom1)){
            globalAllocator.FreePage(pageTable->GetPhysicalAddress(addressVirtual));
        }
        pageTable->UnmapMemory(addressVirtual);
    }
    return KSUCCESS;
}
