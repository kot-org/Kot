#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <kot/sys.h>
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

namespace mlibc{
    void sys_libc_log(const char *message){
        Kot::Sys_Logs((char*)message, strlen(message));
    }

    [[noreturn]] void sys_libc_panic(){
        sys_libc_log("libc panic!");
        __builtin_trap();
        for(;;);
    }

    int sys_tcb_set(void *pointer){
        return (Kot::Sys_SetTCB(Kot::Sys_GetThread(), (uintptr_t)pointer) != KSUCCESS);
    }

    int sys_futex_tid(){
        return static_cast<int>(Kot::Sys_GetTID());
    }

    int sys_futex_wait(int *pointer, int expected, const struct timespec *time){
        // TODO
        __ensure(!"Not implemented");
    }

    int sys_futex_wake(int *pointer){
        // TODO
        __ensure(!"Not implemented");
    }


    int sys_anon_allocate(size_t size, void **pointer){
        // TODO
        *pointer = (void*)KotSpecificData.HeapLocation;
        KotSpecificData.HeapLocation += size;
        return (Syscall_48(KSys_Map, Kot::Sys_GetProcess(), (uint64_t)pointer, 0, 0, (uint64_t)&size, true) != KSUCCESS);
    }

    int sys_anon_free(void *pointer, size_t size){
        sys_vm_unmap(pointer, size);
        return 0;
    }

    int sys_vm_map(void *hint, size_t size, int prot, int flags, int fd, off_t offset, void **window){
        // TODO
        return (Syscall_48(KSys_Map, Kot::Sys_GetProcess(), (uint64_t)&hint, 0, 0, (uint64_t)&size, false) != KSUCCESS);
    }

    int sys_vm_unmap(void *pointer, size_t size){
        return (Kot::Sys_Unmap(Kot::Sys_GetThread(), (uintptr_t)pointer, static_cast<size64_t>(size)) != KSUCCESS);
    }

    int sys_vm_protect(void *pointer, size_t size, int prot){
        // TODO
        __ensure(!"Not implemented");
    }

    [[noreturn]] void sys_exit(int status){
        Syscall_8(KSys_Exit, status);
        __builtin_unreachable();
    }

    [[noreturn, gnu::weak]] void sys_thread_exit(){
        sys_exit(KSUCCESS);
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
        KResult Status = Kot::Sleep((*secs) * 1000000000 + (*nanos));
        *secs = 0;
	    *nanos = 0;
        return 0;
    }

    int sys_sigprocmask(int how, const sigset_t *__restrict set, sigset_t *__restrict retrieve){
        // TODO
        __ensure(!"Not implemented");
    }

    int sys_sigaction(int, const struct sigaction *__restrict, struct sigaction *__restrict){
        // TODO
        //__ensure(!"Not implemented");
        return 0;
    }

    int sys_fork(pid_t *child){
        kot_process_t ProcessChild;
        KResult Status = Kot::Sys_Fork((kot_process_t*)&ProcessChild);

        if(Status != KSUCCESS) return -1;


        if(Kot::Sys_GetPPID()){
            *child = NULL;
        }else{
            *child = ProcessChild;
        }
        return (Status != KSUCCESS);
    }

    int sys_waitpid(pid_t pid, int *status, int flags, struct rusage *ru, pid_t *ret_pid){
        // TODO
        __ensure(!"Not implemented");
    }

    int sys_execve(const char *path, char *const argv[], char *const envp[]){
        Kot::srv_system_callback_t* Callback = Kot::Srv_System_LoadExecutable(Kot::Sys_GetPriviledgeThread(), (char*)path, true);
        KResult Status = Callback->Status;

        uintptr_t MainStackData;
        size64_t SizeMainStackData;
        uint64_t argc = 0;
        for(; argv[argc] != NULL; argc++);
        Kot::SetupStack(&MainStackData, &SizeMainStackData, argc, (char**)argv, (char**)envp);

        Kot::ShareDataWithArguments_t Data{
            .Data = MainStackData,
            .Size = SizeMainStackData,
            .ParameterPosition = 0x0,
        };

        kot_arguments_t InitParameters;

        __ensure(Sys_ExecThread((kot_thread_t)Callback->Data, &InitParameters, Kot::ExecutionTypeQueu, &Data) == KSUCCESS);
        free((void*)MainStackData);
        free((void*)Callback);

        if(Status != KSUCCESS){
            return -1;
        }else{
            Kot::Sys_Close(KSUCCESS);
        }
    }

    pid_t sys_getpid(){
        return static_cast<pid_t>(Kot::Sys_GetPID());
    }

    int sys_kill(int, int){
        // TODO
        __ensure(!"Not implemented");
    }
}