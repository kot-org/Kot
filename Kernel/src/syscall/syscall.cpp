#include <syscall/syscall.h>

static uint64_t mutexSyscall;

extern "C" uint64_t SyscallHandler(InterruptStack* Registers, uint64_t CoreID){
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
            returnValue = CreatSharing(thread->Paging, arg0, (uint64_t*)arg1, (uint64_t*)arg2, (bool)arg3, thread->RingPL);
            thread->MemoryAllocated += (uint64_t)returnValue;
            //this function return the allocated size
            break;
        case Sys_GetShareMemory:
            //get share memory
            returnValue = GetSharing(thread->Paging, (MemoryShareInfo*)arg0, (uint64_t*)arg1, thread->RingPL);
            break;
        case Sys_FreeShareMemory:
            returnValue = FreeSharing((void*)arg0);
            break;
        case Sys_Fork: 
            thread->Fork(Registers, CoreID, (thread_t*)arg0, (Parameters*)arg1);
            Atomic::atomicUnlock(&mutexSyscall, 0);
            return 0xff;
        case Sys_Exit:
            //exit
            globalLogs->Warning("Thread %x exit with error code : %x", thread->TID, arg0);
            thread->Exit(Registers, CoreID);
            Atomic::atomicUnlock(&mutexSyscall, 0);
            return 0xff;
        case Sys_Pause:
            globalLogs->Warning("Thread %x is paused in process %x", thread->TID, thread->Parent->PID);
            thread->Pause(Registers, CoreID);
            globalTaskManager->Scheduler(Registers, CoreID);
            
            Atomic::atomicUnlock(&mutexSyscall, 0);
            return 0xff;            
        case Sys_Map:
            //mmap
            returnValue = mmap(thread->Paging, (void*)arg0, (bool)arg1, (void*)arg2);    
            break;
        case Sys_Unmap:
            //munmap
            returnValue = munmap(thread->Paging, (void*)arg0);  
            break;
        case Sys_GetPhysicallAddress:
            returnValue = (uint64_t)thread->Paging->GetPhysicalAddress((void*)arg0);
            break;
        case Sys_IRQRedirect:
            //Redirect IRQ to driver / device
            if(thread->RingPL <= DevicesRing){
                //returnValue = SetIrq(thread->Parent, (void*)arg0, (uint8_t)arg1);
            }else{
                returnValue = 0;
            }
            break;
        case Sys_IRQDefault:
            //Set default redirection IRQ
            if(thread->RingPL <= DevicesRing){
                //returnValue = SetIrqDefault((uint8_t)arg0);
            }else{
                returnValue = 0;
            }
            break;

        case Sys_CreatThread:
            returnValue = (uint64_t)thread->Parent->CreatThread(arg0, (void*)arg1);
            break;
        case Sys_ExecThread:
            returnValue = thread->Parent->TaskManagerParent->ExecThread((thread_t*)arg0, (Parameters*)arg1);
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

uint64_t mmap(PageTableManager* pageTable, void* addressVirtual, bool usePhysicallAddress, void* addressPhysical){
    addressVirtual = (void*)(addressVirtual - (uint64_t)addressVirtual % 0x1000);
    if((uint64_t)addressVirtual < HigherHalfAddress){
        if(usePhysicallAddress){
            pageTable->MapMemory(addressVirtual, addressPhysical);
        }else{
            pageTable->MapMemory(addressVirtual, globalAllocator.RequestPage());
        }        
    }
    return 1;
}

uint64_t munmap(PageTableManager* pageTable, void* addressVirtual){
    addressVirtual = (void*)(addressVirtual - (uint64_t)addressVirtual % 0x1000);
    if((uint64_t)addressVirtual < HigherHalfAddress){
        pageTable->UnmapMemory(addressVirtual);
    }
    return 1;
}
