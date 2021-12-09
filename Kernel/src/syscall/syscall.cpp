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
    TaskContext* task = &globalTaskManager->NodeExecutePerCore[CoreID]->Content;

    switch(syscall){
        case 0x00: 
            //fread
            returnValue = (void*)((FileSystem::File*)arg0)->Read(arg1, arg2, (void*)arg3);
            break;
        case 0x01:
            //fwrite
            returnValue = (void*)((FileSystem::File*)arg0)->Write(arg1, arg2, (void*)arg3);
            break;
        case 0x02: 
            //fopen
            returnValue = (void*)fileSystem->fopen((char*)arg0, (char*)arg1);
            break;
        case 0x09:
            //mmap
            if(task->Priviledge <= DevicesRing){
                returnValue = (void*)mmap(&task->paging, (void*)arg0, (void*)arg1);    
            }else{
                returnValue = (void*)0;
            }
            break;
        case 0x0B:
            //munmap
            returnValue = (void*)munmap(&task->paging, (void*)arg0);
            break;
        case 0x0C:
            //creat share memory
            returnValue = (void*)Memory::CreatSharing(&task->paging, arg0, (void*)arg1, task->Priviledge);
            //this function return the first physciall address of the sharing memory, it's the key to get sharing
            break;
        case 0x0D:
            //get share memory
            returnValue = (void*)Memory::GetSharing(&task->paging, (void*)arg0, (void*)arg1, task->Priviledge);
            break;
        case 0x16: 
            //creat subTask 
            globalTaskManager->CreatSubTask(task->NodeParent, (void*)arg0, (DeviceTaskAdressStruct*)arg1);
            break;
        case 0x17: 
            //execute subTask
            globalTaskManager->ExecuteSubTask(Registers, CoreID, (DeviceTaskAdressStruct*)arg0, (Parameters*)arg1);
            break;
        case 0x27:
            //Get PID
            returnValue = (void*)task->PID;
            break;
        case 0x3C:
            //exit
            if(!task->IsTaskInTask){
                if(arg0 != NULL){
                    globalLogs->Error("App %s close with error code : %x", task->Name, arg0);
                }else{
                    globalLogs->Successful("App %s close Successfuly", task->Name);
                }
                task->Exit();
                globalTaskManager->Scheduler(Registers, CoreID);
            }else{
                if(arg0 != NULL){
                    globalLogs->Error("Subtask %s close with error code : %x", task->Name, arg0);
                }else{
                    globalLogs->Successful("Subtask %s close Successfuly", task->Name);
                }
                returnValue = task->ExitTaskInTask(Registers, CoreID, (void*)arg1);
            }

            Registers->rdi = returnValue;
            Atomic::atomicUnlock(&mutexSyscall, 0);
            return;

        case 0xff: 
            //Kernel runtime will be delete and replace by IPC
            returnValue = (void*)KernelRuntime(task, arg0, arg1, arg2, arg3, arg4, arg5);
            break;
        default:
            globalLogs->Error("Unknown syscall 0x%x", syscall);
            break;
    }

    Registers->rdi = returnValue;

    Atomic::atomicUnlock(&mutexSyscall, 0);  
}

uint64_t KernelRuntime(TaskContext* task, uint64_t arg0, uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4, uint64_t arg5){
    uint64_t returnValue = 0;
    switch(arg0){
        case 0:
            returnValue = LogHandler(arg1, (char*)arg2);
            break;
        default:
            returnValue = 0;
            break;
    }

    return returnValue;
}

uint64_t LogHandler(uint64_t type, char* str){
    switch(type){
        case 0:
            globalLogs->Message(str);
            break;
        case 1:
            globalLogs->Error(str);
            break;
        case 2:
            globalLogs->Warning(str);
            break;
        case 3:
            globalLogs->Successful(str);
            break;
    }

    return 1;
}

uint64_t mmap(PageTableManager* pageTable, void* addressPhysical, void* addressVirtual){
    //check if page is free
    pageTable->MapMemory(addressVirtual, addressPhysical);
    return 1;
}

uint64_t munmap(PageTableManager* pageTable, void* addressVirtual){
    pageTable->UnmapMemory(addressVirtual);
    return 1;
}
