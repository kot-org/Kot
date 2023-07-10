#include <syscall/std/std.h>

#include <bits/off_t.h>
#include <abi-bits/errno.h>
#include <abi-bits/pid_t.h>
#include <abi-bits/signal.h>
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

    if(!CheckUserAddress((void*)Window, sizeof(void*))){
        return -EINVAL;
    } 

    /* main */
    if(Size == 0){
        /* return */
        return -EINVAL;
    }

    int UnknownFlags = Flags & ~(MAP_ANONYMOUS | MAP_FIXED);
    if(UnknownFlags){
        /* return */
        Warning("Sys_Std_Vm_Map: Unsupported flags %x", Flags);
        return -EINVAL;
    }

    if(Size > 0x40000000){
        /* return */
        Warning("Sys_Std_Vm_Map: Unsupported size: %u", Size);
        return -EINVAL;
    }

    if(Size % PAGE_SIZE){
        Size -= Size % PAGE_SIZE;
        Size += PAGE_SIZE;
    }

    if((uintptr_t)Hint % PAGE_SIZE){
        Hint = (void*)((uintptr_t)Hint - (uintptr_t)Hint % PAGE_SIZE);
    }

    int Errno = 0;
    MemoryRegion_t* Region = MMAllocateRegionVM(Thread->Parent->MemoryManager, Hint, Size, Flags, Prot, &Errno);
    if(!Region){
        /* return */
        Warning("Sys_Std_Vm_Map: Region is isn't valid");
        return -Errno;
    }
    
    /* return */
    if(MMAllocateMemoryBlock(Thread->Parent->MemoryManager, Region) == KSUCCESS){
        /* return */
        *Window = Region->Base;
        return 0;
    }else{
        return -EINVAL;
    }

}

int Sys_Std_Vm_Unmap(SyscallStack* Registers, kthread_t* Thread){
    /* args */
    void* Pointer = reinterpret_cast<void*>(SYSCALL_ARG0(Registers));
    size_t Size = static_cast<size_t>(SYSCALL_ARG1(Registers));
    

    /* main */
    if(MMFree(Thread->Parent->MemoryManager, Pointer, Size) == KSUCCESS){
        /* return */
        return 0;
    }else{
        /* return */
        return -EINVAL;   
    }
}

int Sys_Std_Vm_Protect(SyscallStack* Registers, kthread_t* Thread){
    /* args */
    void* Pointer = reinterpret_cast<void*>(SYSCALL_ARG0(Registers));
    size_t Size = static_cast<size_t>(SYSCALL_ARG1(Registers));
    int Prot = static_cast<int>(SYSCALL_ARG2(Registers));
    

    /* main */
    if(MMProtect(Thread->Parent->MemoryManager, Pointer, Size, Prot) == KSUCCESS){
        /* return */
        return 0;
    }else{
        /* return */
        return -EINVAL;   
    }

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
    int How = static_cast<int>(SYSCALL_ARG0(Registers));
    const sigset_t* Set = reinterpret_cast<const sigset_t*>(SYSCALL_ARG1(Registers));
    sigset_t* Retrieve = reinterpret_cast<sigset_t*>(SYSCALL_ARG2(Registers));

    /* main */
    if(Retrieve){
        if(CheckUserAddress(Retrieve, sizeof(sigset_t)) != KSUCCESS){
            return -EINVAL;
        }
        *Retrieve = Thread->SignalMask;
    }
    switch (How){
        case SIG_BLOCK:
            Thread->SignalMask |= *Set;
            break;
        case SIG_UNBLOCK:{
            Thread->SignalMask &= ~(*Set);
            break;
        }
        case SIG_SETMASK:{
            Thread->SignalMask = *Set;
            break;
        }
        default:
            return -EINVAL;
    }

    /* return */
    return 0;
}

int Sys_Std_Sigaction(SyscallStack* Registers, kthread_t* Thread){
    /* args */
    int Signum = static_cast<int>(SYSCALL_ARG0(Registers));
    const struct sigaction* Act = reinterpret_cast<sigaction*>(SYSCALL_ARG1(Registers));
    struct sigaction* Oldact = reinterpret_cast<sigaction*>(SYSCALL_ARG2(Registers));

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