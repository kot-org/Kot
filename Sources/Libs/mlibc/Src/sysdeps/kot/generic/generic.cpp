#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <kot/sys.h>
#include <kot/uisd.h>
#include <bits/ensure.h>
#include <frg/vector.hpp>
#include <mlibc/debug.hpp>
#include <mlibc/elf/startup.h>
#include <mlibc/allocator.hpp>
#include <kot/uisd/srvs/time.h>
#include <mlibc/all-sysdeps.hpp>
#include <kot/uisd/srvs/system.h>


extern char **environ;
extern mlibc::exec_stack_data __mlibc_stack_data;

uint64_t KotAnonAllocateLock = 0;

namespace mlibc{
    void sys_libc_log(const char *message){
        kot_Sys_Logs((char*)message, strlen(message));
    }

    [[noreturn]] void sys_libc_panic(){
        sys_libc_log("libc panic!");
        __builtin_trap();
        for(;;);
    }

    int sys_tcb_set(void *pointer){
        return (int)Syscall_8(KSys_Std_TCB_Set, (uint64_t)pointer);
    }

    int sys_futex_tid(){
        return static_cast<int>(kot_Sys_GetTID());
    }

    int sys_futex_wait(int *pointer, int expected, const struct timespec *time){
        return (int)Syscall_24(KSys_Std_Futex_Wait, (uint64_t)pointer, (uint64_t)expected, (uint64_t)time);
    }

    int sys_futex_wake(int *pointer){
        return (int)Syscall_8(KSys_Std_Futex_Wake, (uint64_t)pointer);
    }

    int sys_anon_allocate(size_t size, void **pointer){
        return sys_vm_map(nullptr, size, PROT_READ | PROT_WRITE, MAP_ANONYMOUS, -1, 0, pointer);
    }

    int sys_anon_free(void *pointer, size_t size){
        sys_vm_unmap(pointer, size);
        return 0;
    }

    int sys_vm_map(void *hint, size_t size, int prot, int flags, int fd, off_t offset, void **window){
        auto Result = Syscall_40(KSys_Std_Vm_Map, (uint64_t)hint, (uint64_t)size, (uint64_t)prot, (uint64_t)flags, (uint64_t)window);

        if(Result < 0){
            return -Result;
        }

        return Result;
    }

    int sys_vm_unmap(void *pointer, size_t size){
        return Syscall_16(KSys_Std_Vm_Unmap, (uint64_t)pointer, (uint64_t)size);
    }

    int sys_vm_protect(void *pointer, size_t size, int prot){
        auto Result = Syscall_24(KSys_Std_Vm_Protect, (uint64_t)pointer, (uint64_t)size, (uint64_t)prot);

        if(Result < 0){
            return -Result;
        }

        return 0;
    }

    [[noreturn]] void sys_exit(int status){
        Syscall_8(KSys_Std_Exit, (uint64_t)status);
        __builtin_unreachable();
    }

    [[noreturn, gnu::weak]] void sys_thread_exit(){
        Syscall_0(KSys_Std_Thread_Exit);
        __builtin_unreachable();
    }

    int sys_clock_get(int clock, time_t *secs, long *nanos){
        // TODO
        *secs = 0;
        *nanos = 0;
        return 0;
    }

    int sys_clock_getres(int clock, time_t *secs, long *nanos){
        // TODO
        *secs = 0;
        *nanos = 0;
        return 0;
    }

    int sys_sleep(time_t *secs, long *nanos){
        KResult Status = kot_Sleep((*secs) * 1000000000 + (*nanos));
        *secs = 0;
	    *nanos = 0;
        return 0;
    }

    int sys_sigprocmask(int how, const sigset_t *__restrict set, sigset_t *__restrict retrieve){
        auto Result = Syscall_24(KSys_Std_Sigprocmask, (uint64_t)how, (uint64_t)set, (uint64_t)retrieve);

        if(Result < 0){
            return -Result;
        }

        return 0;
    }

    int sys_sigaction(int how, const struct sigaction *__restrict action, struct sigaction *__restrict old_action){
        uint64_t Sigreturn = (uint64_t)0; // TODO

        auto Result = Syscall_32(KSys_Std_Sigaction, (uint64_t)how, (uint64_t)action, (uint64_t)Sigreturn, (uint64_t)old_action);

        if (Result < 0) {
            return -Result;
        }

        return 0;
    }

    int sys_fork(pid_t *child){
        pid_t Result = (pid_t)Syscall_0(KSys_Std_Fork);

        if(Result < 0){
            return -Result;
        }

        if(Result == 0){
            __ensure(kot_ResetUISDThreads() == KSUCCESS);
        }

        *child = Result;

        return 0;
    }

    int sys_waitpid(pid_t pid, int *status, int flags, struct rusage *ru, pid_t *ret_pid){
        if(ru){
            mlibc::infoLogger() << "mlibc: struct rusage in sys_waitpid is unsupported" << frg::endlog;
            return -ENOSYS;
        }

        auto Result = Syscall_24(KSys_Std_Wait_PID, pid, status, flags);

        if(Result < 0){
            return -Result;
        }

        *ret_pid = static_cast<pid_t>(Result);

        return 0;
    }

    int sys_execve(const char *path, char *const argv[], char *const envp[]){
        if(!fopen(path, "r")){
           return -1;
        }

        void* MainStackData;
        size64_t SizeMainStackData;
        uint64_t argc = 0;
        for(; argv[argc] != NULL; argc++);
        kot_SetupStack(&MainStackData, &SizeMainStackData, argc, (char**)argv, (char**)envp);

        kot_Srv_System_LoadExecutableToProcess((char*)path, MainStackData, SizeMainStackData, 0, NULL, 0);

        __builtin_unreachable();
    }

    pid_t sys_getpid(){
        return static_cast<pid_t>(Syscall_0(KSys_Std_Get_PID));
    }

    int sys_kill(int pid, int sig){
        auto Result = Syscall_16(KSys_Std_Kill, pid, sig);

        if(Result < 0){
            return -Result;
        }

        return 0;
    }

    uint64_t sys_debug_malloc_lock = 0;

    void *sys_debug_malloc(size_t size){
        if(size == 0) return NULL;
        atomicAcquire(&sys_debug_malloc_lock, 0);
        size64_t SizeToAlloc = size + sizeof(size64_t);
        if(SizeToAlloc % KotSpecificData.MMapPageSize){
            SizeToAlloc -= SizeToAlloc % KotSpecificData.MMapPageSize;
            SizeToAlloc += KotSpecificData.MMapPageSize;
        }
        size64_t* CurrentSeg = (size64_t*)((uintptr_t)KotSpecificData.HeapLocation + KotSpecificData.MMapPageSize - ((size + sizeof(size64_t)) % KotSpecificData.MMapPageSize));
        void* VirtualAddress = (void*)CurrentSeg;
        __ensure(kot_Sys_Map(kot_Sys_GetProcess(), (void**)&VirtualAddress, AllocationTypeBasic, NULL, (size64_t*)&SizeToAlloc, false) == KSUCCESS);
        void* Buffer = (void*)((uintptr_t)CurrentSeg + sizeof(size64_t));
        KotSpecificData.HeapLocation = (uint64_t)((uintptr_t)KotSpecificData.HeapLocation + SizeToAlloc + KotSpecificData.MMapPageSize); // Add page size for the guard
        *CurrentSeg = size;
        atomicUnlock(&sys_debug_malloc_lock, 0);
        return Buffer;
    }

    void *sys_debug_realloc(void *ptr, size_t size){
        void* newBuffer = sys_debug_malloc(size);
        if(newBuffer == NULL){
            return NULL;
        }
        if(ptr != NULL){
            size64_t oldSize = *(size64_t*)((uintptr_t)ptr - sizeof(size64_t));
            if(oldSize > size){
                mlibc::infoLogger() << size << " " << oldSize << frg::endlog;
                oldSize = size;
            }
            memcpy(newBuffer, ptr, oldSize);
            sys_debug_free(ptr);
        }
        return newBuffer;
    }

    void sys_debug_free(void *ptr){
        if(ptr == NULL) return;
        size64_t SizeToFree = *(size64_t*)(void*)((uintptr_t)ptr - sizeof(size64_t));
        uintptr_t StartAdress = (uintptr_t)((uintptr_t)ptr - sizeof(size64_t));
        if(StartAdress % KotSpecificData.MMapPageSize){
            SizeToFree += StartAdress % KotSpecificData.MMapPageSize;
            StartAdress -= StartAdress % KotSpecificData.MMapPageSize;
        }
        kot_Sys_Unmap(kot_Sys_GetProcess(), (void*)StartAdress, SizeToFree);
    }
}