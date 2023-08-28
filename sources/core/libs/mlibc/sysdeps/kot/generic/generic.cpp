#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <kot/syscall.h>
#include <bits/ensure.h>
#include <frg/vector.hpp>
#include <mlibc/debug.hpp>
#include <mlibc/elf/startup.h>
#include <mlibc/allocator.hpp>
#include <mlibc/all-sysdeps.hpp>

namespace mlibc{
    void sys_libc_log(const char *message){
        syscall(SYS_LOG, message, strlen(message));
    }

    [[noreturn]] void sys_libc_panic(){
        sys_libc_log("libc panic!");
        __builtin_trap();
        for(;;);
    }

    int sys_tcb_set(void *pointer){
        // TODO
        __ensure(!"Not implemented");
        return 0;
    }

    int sys_futex_tid(){
        // TODO
        __ensure(!"Not implemented");
        return 0;
    }

    int sys_futex_wait(int *pointer, int expected, const struct timespec *time){
        // TODO
        __ensure(!"Not implemented");
        return 0;
    }

    int sys_futex_wake(int *pointer){
        // TODO
        __ensure(!"Not implemented");
        return 0;
    }

    int sys_anon_allocate(size_t size, void **pointer){
        // TODO
        __ensure(!"Not implemented");
        return 0;
    }

    int sys_anon_free(void *pointer, size_t size){
        // TODO
        __ensure(!"Not implemented");
        return 0;
    }

    int sys_vm_map(void *hint, size_t size, int prot, int flags, int fd, off_t offset, void **window){
        // TODO
        __ensure(!"Not implemented");
        return 0;
    }

    int sys_vm_unmap(void *pointer, size_t size){
        // TODO
        __ensure(!"Not implemented");
        return 0;
    }

    int sys_vm_protect(void *pointer, size_t size, int prot){
        // TODO
        __ensure(!"Not implemented");
        return 0;
    }

    [[noreturn]] void sys_exit(int status){
        // TODO
        __ensure(!"Not implemented");
        __builtin_unreachable();
    }

    [[noreturn, gnu::weak]] void sys_thread_exit(){
        // TODO
        __ensure(!"Not implemented");
        __builtin_unreachable();
    }

    int sys_clock_get(int clock, time_t *secs, long *nanos){
        // TODO
        __ensure(!"Not implemented");
        *secs = 0;
        *nanos = 0;
        return 0;
    }

    int sys_clock_getres(int clock, time_t *secs, long *nanos){
        // TODO
        __ensure(!"Not implemented");
        *secs = 0;
        *nanos = 0;
        return 0;
    }

    int sys_sleep(time_t *secs, long *nanos){
        // TODO
        __ensure(!"Not implemented");
        *secs = 0;
	    *nanos = 0;
        return 0;
    }

    int sys_sigprocmask(int how, const sigset_t *__restrict set, sigset_t *__restrict retrieve){
        // TODO
        __ensure(!"Not implemented");
        return 0;
    }

    int sys_sigaction(int how, const struct sigaction *__restrict action, struct sigaction *__restrict old_action){
        // TODO
        __ensure(!"Not implemented");
        return 0;
    }

    int sys_fork(pid_t *child){
        // TODO
        __ensure(!"Not implemented");
        return 0;
    }

    int sys_waitpid(pid_t pid, int *status, int flags, struct rusage *ru, pid_t *ret_pid){
        // TODO
        __ensure(!"Not implemented");
        return 0;
    }

    int sys_execve(const char *path, char *const argv[], char *const envp[]){
        // TODO
        __ensure(!"Not implemented");
        __builtin_unreachable();
    }

    pid_t sys_getpid(){
        // TODO
        __ensure(!"Not implemented");
        return 0;
    }

    pid_t sys_getppid(){
        // TODO
        __ensure(!"Not implemented");
        return 0;
    }

    int sys_kill(int pid, int sig){
        // TODO
        __ensure(!"Not implemented");
        return 0;
    }
}