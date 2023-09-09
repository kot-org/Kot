#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <asm/prctl.h>
#include <kot/syscall.h>
#include <bits/ensure.h>
#include <frg/vector.hpp>
#include <mlibc/debug.hpp>
#include <mlibc/elf/startup.h>
#include <mlibc/allocator.hpp>
#include <mlibc/all-sysdeps.hpp>

namespace mlibc{
    void sys_libc_log(const char *message){
        do_syscall(SYS_LOG, message, strlen(message));
    }

    [[noreturn]] void sys_libc_panic(){
        sys_libc_log("libc panic!");
        __builtin_trap();
        for(;;);
    }

    int sys_tcb_set(void *pointer){
        auto result = do_syscall(SYS_ARCH_PRCTL, ARCH_SET_FS, (uint64_t)pointer);

        if(result < 0){
            return -result;
        }

        return 0;
    }

    int sys_futex_tid(){
        auto result = do_syscall(SYS_GET_TID);
        return static_cast<int>(result);
    }

    int sys_futex_wait(int *pointer, int expected, const struct timespec *time){
        auto result = do_syscall(SYS_FUTEX_WAIT, pointer, expected, time);
        
        if(result < 0){
            return -result;
        }
        
        return 0;
    }

    int sys_futex_wake(int *pointer){
        auto result = do_syscall(SYS_FUTEX_WAKE, pointer);
        
        if(result < 0){
            return -result;
        }
        
        return 0;
    }

    int sys_anon_allocate(size_t size, void **pointer){
        return sys_vm_map(nullptr, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0, pointer);
    }

    int sys_anon_free(void *pointer, size_t size){
        return sys_vm_unmap(pointer, size);
    }

    int sys_vm_map(void *hint, size_t size, int prot, int flags, int fd, off_t offset, void **window){
        intptr_t result = do_syscall(SYS_MMAP, hint, size, prot, flags, fd, offset);

        if(result < 0){
            return -result;
        }

        *window = (void*)result;
        
        return 0;
    }

    int sys_vm_unmap(void *pointer, size_t size){
        auto result = do_syscall(SYS_MUNMAP, pointer, size);
        
        if(result < 0){
            return -result;
        }

        return 0;
    }

    int sys_vm_protect(void *pointer, size_t size, int prot){
        auto result = do_syscall(SYS_MPROTECT, pointer, size, prot);
        
        if(result < 0){
            return -result;
        }

        return 0;
    }

    [[noreturn]] void sys_exit(int status){
        do_syscall(SYS_EXIT, status);
        
        mlibc::panicLogger() << "mlibc: " << __func__ << " failed" << frg::endlog;
    }

    [[noreturn, gnu::weak]] void sys_thread_exit(){
        do_syscall(SYS_THREAD_EXIT);
        
        mlibc::panicLogger() << "mlibc: " << __func__ << " failed" << frg::endlog;
    }

    int sys_clock_get(int clock, time_t *secs, long *nanos){
        struct timespec ts;
        auto result = do_syscall(SYS_CLOCK_GET, clock, &ts);

        if(result < 0){
            return -result;
        }

        *secs = ts.tv_sec;
        *nanos = ts.tv_nsec;

        return 0;
    }

    int sys_clock_getres(int clock, time_t *secs, long *nanos){
        struct timespec ts;
        auto result = do_syscall(SYS_CLOCK_GETRES, clock, &ts);

        if(result < 0){
            return -result;
        }

        *secs = ts.tv_sec;
        *nanos = ts.tv_nsec;

        return 0;
    }

    int sys_sleep(time_t *secs, long *nanos){
        struct timespec ts = {.tv_sec = *secs, .tv_nsec = *nanos};

        auto result = do_syscall(SYS_SLEEP, &ts);

        if(result < 0){
            return -result;
        }

        return 0;
    }

    int sys_sigprocmask(int how, const sigset_t *__restrict set, sigset_t *__restrict retrieve){
        auto result = do_syscall(SYS_SIGPROCMASK, how, set, retrieve);

        if(result < 0){
            return -result;
        }

        return 0;
    }

    int sys_sigaction(int how, const struct sigaction *__restrict action, struct sigaction *__restrict old_action){
        mlibc::infoLogger() << "mlibc: " << __func__ << " is a stub!" << frg::endlog;
        return 0;
    }

    int sys_fork(pid_t *child){
        auto result = do_syscall(SYS_FORK);

        if(result < 0){
            return -result;
        }

        *child = result;
        return 0;
    }

    int sys_waitpid(pid_t pid, int *status, int flags, struct rusage *ru, pid_t *ret_pid){
        auto result = do_syscall(SYS_WAITPID, pid, status, flags, ru);

        if(result < 0){
            return -result;
        }

        *ret_pid = result;
        return 0;
    }

    int sys_execve(const char *path, char *const argv[], char *const envp[]){
        auto result = do_syscall(SYS_EXECVE, path, argv, envp);

        if(result < 0){
            return -result;
        }

        return 0;
    }

    pid_t sys_getpid(){
        auto result = do_syscall(SYS_GETPID);
        __ensure(result >= 0);

        return result;
    }

    pid_t sys_getppid(){
        auto result = do_syscall(SYS_GETPPID);
        __ensure(result != 0);

        return result;
    }

    int sys_kill(int pid, int sig){
        auto result = do_syscall(SYS_KILL, pid, sig);

        if(result < 0){
            return -result;
        }

        return 0;
    }
}