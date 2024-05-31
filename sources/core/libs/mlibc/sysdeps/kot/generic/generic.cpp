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

    uid_t sys_getuid(){
        mlibc::infoLogger() << "mlibc: " << __func__ << " is a stub" << frg::endlog;
        return 0;
    }

    uid_t sys_geteuid(){
        mlibc::infoLogger() << "mlibc: " << __func__ << " is a stub" << frg::endlog;
        return 0;
    }

    gid_t sys_getgid(){
        mlibc::infoLogger() << "mlibc: " << __func__ << " is a stub" << frg::endlog;
        return 0;
    }

    gid_t sys_getegid(){
        mlibc::infoLogger() << "mlibc: " << __func__ << " is a stub" << frg::endlog;
        return 0;
    }

    pid_t sys_getpgid(pid_t pid, pid_t* pgid){
        mlibc::infoLogger() << "mlibc: " << __func__ << " is a stub" << frg::endlog;
        *pgid = 0;
        return 0;
    }

    int sys_socket(int family, int type, int protocol, int *fd){
        auto result = do_syscall(SYS_SOCKET, family, type, protocol);

        if(result < 0){
            return -result;
        }

        *fd = result;
        return 0;
    }

    int sys_bind(int fd, const struct sockaddr *addr_ptr, socklen_t addr_length){
        auto result = do_syscall(SYS_BIND, fd, addr_ptr, (sc_word_t)addr_length);

        if(result < 0){
            return -result;
        }

        return 0;
    }

    int sys_connect(int fd, const struct sockaddr *addr_ptr, socklen_t addr_length){
        auto result = do_syscall(SYS_CONNECT, fd, addr_ptr, (sc_word_t)addr_length);

        if(result < 0){
            return -result;
        }

        return 0;
    }

    int sys_listen(int fd, int backlog){
        auto result = do_syscall(SYS_LISTEN, fd, backlog);

        if(result < 0){
            return -result;
        }

        return 0;
    }

    int sys_accept(int sockfd, int *newfd, struct sockaddr *addr_ptr, socklen_t *addr_length){
        auto result = do_syscall(SYS_ACCEPT, sockfd, addr_ptr, addr_length);

        if(result < 0){
            return -result;
        }

        *newfd = result;
        return 0;
    }

    int sys_msg_send(int fd, const struct msghdr *hdr, int flags, ssize_t *length){
        auto result = do_syscall(SYS_SOCKET_SEND, fd, hdr, flags);
        if(result < 0)
            return -result;

        *length = result;
        return 0;
    }

    int sys_msg_recv(int sockfd, struct msghdr *msg_hdr, int flags, ssize_t *length){
        auto result = do_syscall(SYS_SOCKET_RECV, sockfd, msg_hdr, flags);

        if(result < 0){
            return -result;
        }

        *length = result;
        return 0;
    }

    int sys_socketpair(int domain, int type_and_flags, int proto, int *fds){
        auto result = do_syscall(SYS_SOCKET_PAIR, domain, type_and_flags, proto, fds);

        if(result < 0){
            return -result;
        }

        return 0;
    }

    int sys_getsockopt(int fd, int layer, int number, void *__restrict buffer, socklen_t *__restrict size){
        (void)fd;
        (void)size;
        if (layer == SOL_SOCKET && number == SO_PEERCRED) {
            mlibc::infoLogger() << "\e[31mmlibc: getsockopt() call with SOL_SOCKET "
                                "and SO_PEERCRED is unimplemented\e[39m"
                                << frg::endlog;
            *(int *)buffer = 0;
            return 0;
        } else if (layer == SOL_SOCKET && number == SO_SNDBUF) {
            mlibc::infoLogger() << "\e[31mmlibc: getsockopt() call with SOL_SOCKET "
                                "and SO_SNDBUF is unimplemented\e[39m"
                                << frg::endlog;
            *(int *)buffer = 4096;
            return 0;
        } else if (layer == SOL_SOCKET && number == SO_TYPE) {
            mlibc::infoLogger()
                << "\e[31mmlibc: getsockopt() call with SOL_SOCKET and SO_TYPE is "
                "unimplemented, hardcoding SOCK_STREAM\e[39m"
                << frg::endlog;
            *(int *)buffer = SOCK_STREAM;
            return 0;
        } else if (layer == SOL_SOCKET && number == SO_ERROR) {
            mlibc::infoLogger()
                << "\e[31mmlibc: getsockopt() call with SOL_SOCKET and SO_ERROR is "
                "unimplemented, hardcoding 0\e[39m"
                << frg::endlog;
            *(int *)buffer = 0;
            return 0;
        } else if (layer == SOL_SOCKET && number == SO_KEEPALIVE) {
            mlibc::infoLogger()
                << "\e[31mmlibc: getsockopt() call with SOL_SOCKET and "
                "SO_KEEPALIVE is unimplemented, hardcoding 0\e[39m"
                << frg::endlog;
            *(int *)buffer = 0;
            return 0;
        } else {
            mlibc::panicLogger()
                << "\e[31mmlibc: Unexpected getsockopt() call, layer: " << layer
                << " number: " << number << "\e[39m" << frg::endlog;
            __builtin_unreachable();
        }

        return 0;
    }
}