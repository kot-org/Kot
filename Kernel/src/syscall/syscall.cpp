#include "syscall.h"

static uint64_t mutexSyscall;

extern "C" void SyscallInt_Handler(InterruptStack* Registers, uint64_t CoreID){
    Atomic::atomicSpinlock(&mutexSyscall, 0);
    Atomic::atomicLock(&mutexSyscall, 0);

    uint64_t syscall = (uint64_t)Registers->rax;
    uint64_t arg0 = (uint64_t)Registers->rdi;
    uint64_t arg1 = (uint64_t)Registers->rsi;
    uint64_t arg2 = (uint64_t)Registers->rdx;
    uint64_t arg3 = (uint64_t)Registers->r10;
    uint64_t arg4 = (uint64_t)Registers->r8;
    uint64_t arg5 = (uint64_t)Registers->r9;
    void* returnValue = 0;
    Task* task = globalTaskManager->NodeExecutePerCore[CoreID];
    switch(syscall){
        case Sys_CreatShareMemory:
            //creat share memory
            returnValue = (void*)Memory::CreatSharing(&task->paging, arg0, (uint64_t*)arg1, (uint64_t*)arg2, (bool)arg3, task->Priviledge);
            task->MemoryAllocated += (uint64_t)returnValue;
            //this function return the allocated size
            break;
        case Sys_GetShareMemory:
            //get share memory
            returnValue = (void*)Memory::GetSharing(&task->paging, (void*)arg0, (uint64_t*)arg1, task->Priviledge);
            break;
        case Sys_FreeShareMemory:
            returnValue = (void*)Memory::FreeSharing((void*)arg0);
        case Sys_CreatSubtask: 
            //creat subTask 
            globalTaskManager->CreatSubTask(task, (void*)arg0, (DeviceTaskAdressStruct*)arg1);
            break;
        case Sys_ExecuteSubtask: 
            //execute subTask
            task->ExecuteSubTask(Registers, CoreID, (DeviceTaskAdressStruct*)arg0, (Parameters*)arg1);

            Atomic::atomicUnlock(&mutexSyscall, 0);
            return;
        case Sys_Exit:
            //exit
            if(!task->IsTaskInTask){
                if(arg0 != NULL){
                    globalLogs->Error("App %s close with error code : %x", task->Name, arg0);
                }else{
                    globalLogs->Successful("App %s close Successfuly", task->Name);
                }
                task->Exit(CoreID);
                globalTaskManager->Scheduler(Registers, CoreID);
            }else{
                returnValue = task->ExitTaskInTask(Registers, CoreID, (void*)arg1);
                Registers->rdi = returnValue;
            }


            Atomic::atomicUnlock(&mutexSyscall, 0);
            return;
        case Sys_Pause:
            task->Pause(CoreID, Registers);
            globalLogs->Warning("App %s is paused", task->Name);
            globalTaskManager->Scheduler(Registers, CoreID);
            
            Atomic::atomicUnlock(&mutexSyscall, 0);
            return;            
        case Sys_Map:
            //mmap
            if(task->Priviledge <= DevicesRing){
                returnValue = (void*)mmap(&task->paging, (void*)arg0, (void*)arg1);    
            }else{
                returnValue = (void*)0;
            }
            break;
        case Sys_Unmap:
            //munmap
            if(task->Priviledge <= DevicesRing){
                returnValue = (void*)munmap(&task->paging, (void*)arg0);  
            }else{
                returnValue = (void*)0;
            }

            break;
        case Sys_IRQRedirect:
            //Redirect IRQ to driver / device
            if(task->Priviledge <= DevicesRing){
                returnValue = (void*)SetIrq(task, (void*)arg0, (uint8_t)arg1);
            }else{
                returnValue = (void*)0;
            }
            break;
        case Sys_IRQExit:
            //Close IRQ
            if(task->InterruptTask){
                task->ExitIRQ();
                APIC::localApicEOI(CoreID);
                globalTaskManager->Scheduler(Registers, CoreID);
                Atomic::atomicUnlock(&mutexSyscall, 0);
                return;
            }else{
                returnValue = (void*)0;
                break;
            }
        case Sys_IRQDefault:
            //Set default redirection IRQ
            if(task->Priviledge <= DevicesRing){
                returnValue = (void*)SetIrqDefault((uint8_t)arg0);
            }else{
                returnValue = (void*)0;
            }
            break;
        case Sys_GetTaskInfo:

            break;
        case Sys_SetTaskInfo:

            break;
        case Sys_CreatThread:
            
            break;
        case Sys_LaunchThread:

            break;
        case Sys_StopThread:

            break;
        case Sys_In:
            returnValue = (void*)IO_IN((uint8_t)arg0, (uint16_t)arg1, (uint32_t)arg2);
            break;
        case Sys_Out:
            returnValue = (void*)IO_OUT((uint8_t)arg0, (uint16_t)arg1);
            break;
        default:
            globalLogs->Error("Unknown syscall %x", syscall);
            break;
    }

    Registers->rdi = returnValue;

    Atomic::atomicUnlock(&mutexSyscall, 0);  
}

uint64_t mmap(PageTableManager* pageTable, void* addressPhysical, void* addressVirtual){
    addressVirtual = (void*)(addressVirtual - (uint64_t)addressVirtual % 0x1000);
    if((uint64_t)addressVirtual < HigherHalfAddress){
        pageTable->MapMemory(addressVirtual, addressPhysical);
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
