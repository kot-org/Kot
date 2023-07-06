#include <syscall/std/std.h>

#include <bits/off_t.h>
#include <abi-bits/errno.h>
#include <abi-bits/pid_t.h>
#include <abi-bits/vm-flags.h>

/* -------------------------------Specs--------------------------------------- */

// int Sys_Std_Syscall_Example_Name(SyscallStack* Registers, kthread_t* Thread){
//     /* args */
//     type_t 0 = static_cast<type_t>(SYSCALL_ARG0(Registers));
//     type_t 1 = static_cast<type_t>(SYSCALL_ARG1(Registers));
//     type_t 2 = static_cast<type_t>(SYSCALL_ARG2(Registers));
//     type_t* 3 = reinterpret_cast<type_t*>(SYSCALL_ARG3(Registers));
//     type_t* 4 = reinterpret_cast<type_t*>(SYSCALL_ARG4(Registers));
//     type_t* 5 = reinterpret_cast<type_t*>(SYSCALL_ARG5(Registers));

//     /* main */
//     /* ... */

//     /* return */
//     return 0;
// }



int Sys_Std_TCB_Set(SyscallStack* Registers, kthread_t* Thread){
    /* args */
    void* FSBase = reinterpret_cast<void*>(SYSCALL_ARG0(Registers));

    /* main */
    Thread->FSBase = FSBase;
    CPU::SetCPUFSBase((uint64_t)FSBase);

    /* return */
    return 0;
}

int Sys_Std_Futex_TID(SyscallStack* Registers, kthread_t* Thread){
    /* args */

    /* main */

    /* return */
    return static_cast<int>(Thread->TID);
}

int Sys_Std_Futex_Wait(SyscallStack* Registers, kthread_t* Thread){
    /* args */

    /* main */
    // TODO

    /* return */
    return 0;
}

int Sys_Std_Futex_Wake(SyscallStack* Registers, kthread_t* Thread){
    /* args */

    /* main */
    // TODO

    /* return */
    return 0;
}

int Sys_Std_Vm_Map(SyscallStack* Registers, kthread_t* Thread){
    /* args */
    void* Hint = reinterpret_cast<void*>(SYSCALL_ARG0(Registers));
    size_t Size = static_cast<size_t>(SYSCALL_ARG1(Registers));
    int Prot = static_cast<int>(SYSCALL_ARG2(Registers));
    int Flags = static_cast<int>(SYSCALL_ARG3(Registers));
    void** Window = reinterpret_cast<void**>(SYSCALL_ARG4(Registers));

    /* main */
    if(Size == 0){
        /* return */
        return -EINVAL;
    }

    bool Fixed = Flags & MAP_FIXED;
    bool Anon = Flags & MAP_ANONYMOUS;

    int UnknownFlags = Flags & ~(MAP_ANONYMOUS | MAP_FIXED);
    if(UnknownFlags){
        Warning("Sys_Std_Vm_Map: Unsupported flags %x", Flags);
        return -EINVAL;
    }

    if(Size > 0x40000000){
        Warning("Sys_Std_Vm_Map: Unsupported size: %u", Size);
        return -EINVAL;
    }

    MemoryRegion_t* Region = MMAllocateRegionVM(Thread->Parent->MemoryManager, Hint, Size, Flags, Prot);

    /* return */
    if(!Region){
        return -EINVAL;
    }else{
        *Window = Region->Base;
        return 0;
    }
}

int Sys_Std_Vm_Unmap(SyscallStack* Registers, kthread_t* Thread){
    /* args */

    /* main */
    // TODO

    /* return */
    return 0;
}

int Sys_Std_Vm_Protect(SyscallStack* Registers, kthread_t* Thread){
    /* args */

    /* main */
    // TODO

    /* return */
    return 0;
}

int Sys_Std_Exit(SyscallStack* Registers, kthread_t* Thread){
    /* args */
    int Status = SYSCALL_ARG0(Registers);

    /* main */
    globalTaskManager->Exit((ContextStack*)Registers, Thread, Status);

    /* return */
    return -1;
}

int Sys_Std_Thread_Exit(SyscallStack* Registers, kthread_t* Thread){
    /* args */

    /* main */
    globalTaskManager->Exit((ContextStack*)Registers, Thread, KSUCCESS);

    /* return */
    return -1;
}

int Sys_Std_Sigprocmask(SyscallStack* Registers, kthread_t* Thread){
    /* args */

    /* main */
    // TODO

    /* return */
    return 0;
}

int Sys_Std_Sigaction(SyscallStack* Registers, kthread_t* Thread){
    /* args */

    /* main */
    // TODO

    /* return */
    return 0;
}

int Sys_Std_Fork(SyscallStack* Registers, kthread_t* Thread){
    /* args */

    /* main */
    kprocess_t* Child = NULL;
    kthread_t* ChildThread = NULL;
    kprocess_t* Parent = Thread->Parent;

    Thread->Parent->Fork((ContextStack*)Registers, Thread, &Child, &ChildThread);

    /* return */
    if(Child){
        return Child->PID;
    }else{
        return -ENOMEM;
    }
}

int Sys_Std_Wait_PID(SyscallStack* Registers, kthread_t* Thread){
    /* args */
    pid_t Pid = static_cast<pid_t>(SYSCALL_ARG0(Registers));
    int* Status = reinterpret_cast<int*>(SYSCALL_ARG1(Registers));
    int Flags = static_cast<int>(SYSCALL_ARG2(Registers));

    /* main */
    wait_pid_t* WaitPIDInfo = (wait_pid_t*)kmalloc(sizeof(wait_pid_t));
    WaitPIDInfo->Pid = Pid;
    WaitPIDInfo->Thread = Thread;

    if(Pid <= 0){
        kprocess_t* ProcParent = Thread->Parent;

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
        kprocess_t* ProcessFromPid = Thread->Parent->TaskManagerParent->GetProcessList(Pid);
        if(!ProcessFromPid) return -ECHILD;
        
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

    /* return */
    return 0;
}

int Sys_Std_Get_PID(SyscallStack* Registers, kthread_t* Thread){
    /* args */

    /* main */

    /* return */
    return static_cast<int>(Thread->Parent->PID);
}

int Sys_Std_Kill(SyscallStack* Registers, kthread_t* Thread){
    /* args */

    /* main */
    // TODO

    /* return */
    return 0;
}