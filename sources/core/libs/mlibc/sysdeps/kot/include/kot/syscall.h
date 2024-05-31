#ifndef SYSCALL_H
#define SYSCALL_H

#include <stdint.h>
#include <bits/ensure.h>

/* types */
typedef long sc_word_t;

/* list */
#define SYS_COUNT               46

#define SYS_LOG                 0
#define SYS_ARCH_PRCTL          1
#define SYS_GET_TID             2
#define SYS_FUTEX_WAIT          3
#define SYS_FUTEX_WAKE          4
#define SYS_MMAP                5
#define SYS_MUNMAP              6
#define SYS_MPROTECT            7
#define SYS_EXIT                8
#define SYS_THREAD_EXIT         9
#define SYS_CLOCK_GET           10
#define SYS_CLOCK_GETRES        11
#define SYS_SLEEP               12
#define SYS_SIGPROCMASK         13
#define SYS_SIGACTION           14
#define SYS_SIGRESTORE          15
#define SYS_FORK                16
#define SYS_WAITPID             17
#define SYS_EXECVE              18
#define SYS_GETPID              19
#define SYS_GETPPID             20
#define SYS_KILL                21
#define SYS_FILE_OPEN           22
#define SYS_FILE_READ           23
#define SYS_FILE_WRITE          24
#define SYS_FILE_SEEK           25
#define SYS_FILE_CLOSE          26
#define SYS_FILE_IOCTL          27
#define SYS_DIR_READ_ENTRIES    28
#define SYS_DIR_REMOVE          29
#define SYS_UNLINK_AT           30
#define SYS_RENAME_AT           31
#define SYS_PATH_STAT           32
#define SYS_FD_STAT             33
#define SYS_FCNTL               34
#define SYS_GETCWD              35
#define SYS_CHDIR               36
#define SYS_SOCKET              37
#define SYS_BIND                38
#define SYS_CONNECT             39
#define SYS_LISTEN              40
#define SYS_ACCEPT              41
#define SYS_SOCKET_SEND         42
#define SYS_SOCKET_RECV         43
#define SYS_SOCKET_PAIR         44
#define SYS_PPOLL               45


/* extern functions */
sc_word_t do_syscall0(long sc);
sc_word_t do_syscall1(long sc, sc_word_t arg0);
sc_word_t do_syscall2(long sc, sc_word_t arg0, sc_word_t arg1);
sc_word_t do_syscall3(long sc, sc_word_t arg0, sc_word_t arg1, sc_word_t arg2);
sc_word_t do_syscall4(long sc, sc_word_t arg0, sc_word_t arg1, sc_word_t arg2, sc_word_t arg3);
sc_word_t do_syscall5(long sc, sc_word_t arg0, sc_word_t arg1, sc_word_t arg2, sc_word_t arg3, sc_word_t arg4);
sc_word_t do_syscall6(long sc, sc_word_t arg0, sc_word_t arg1, sc_word_t arg2, sc_word_t arg3, sc_word_t arg4, sc_word_t arg5);

/* inline functions */
#ifdef __cplusplus

inline sc_word_t sc_cast(long x) { 
    return x; 
}
inline sc_word_t sc_cast(const void *x) { 
    return reinterpret_cast<sc_word_t>(x);
}

__attribute__((always_inline)) static inline long _do_syscall(int call) {
    return do_syscall0(call);
}


__attribute__((always_inline)) static inline long _do_syscall(int call, sc_word_t arg0) {
    return do_syscall1(call, arg0);
}

__attribute__((always_inline)) static inline long _do_syscall(int call, sc_word_t arg0, sc_word_t arg1) {
    return do_syscall2(call, arg0, arg1);
}

__attribute__((always_inline)) static inline long _do_syscall(int call, sc_word_t arg0, sc_word_t arg1, sc_word_t arg2) {
    return do_syscall3(call, arg0, arg1, arg2);
}

__attribute__((always_inline)) static inline long _do_syscall(int call, sc_word_t arg0, sc_word_t arg1, sc_word_t arg2, sc_word_t arg3) {
    return do_syscall4(call, arg0, arg1, arg2, arg3);
}

__attribute__((always_inline)) static inline long _do_syscall(int call, sc_word_t arg0, sc_word_t arg1, sc_word_t arg2, sc_word_t arg3, sc_word_t arg4) {
    return do_syscall5(call, arg0, arg1, arg2, arg3, arg4);
}

__attribute__((always_inline)) static inline long _do_syscall(int call, sc_word_t arg0, sc_word_t arg1, sc_word_t arg2, sc_word_t arg3, sc_word_t arg4, sc_word_t arg5) {
    return do_syscall6(call, arg0, arg1, arg2, arg3, arg4, arg5);
}


template <typename... T>
__attribute__((always_inline)) static inline long do_syscall(sc_word_t call, T... args) {
    return _do_syscall(call, sc_cast(args)...);
}

#endif 

#endif // SYSCALL_H
