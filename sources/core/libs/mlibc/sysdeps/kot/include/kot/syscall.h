#ifndef SYSCALL_H
#define SYSCALL_H

#include <stdint.h>
#include <bits/ensure.h>

/* types */
typedef long sc_word_t;

/* list */
#define SYSCALL_COUNT 1

#define SYS_LOG 0


/* extern functions */
sc_word_t do_syscall0(long sc);
sc_word_t do_syscall1(long sc, sc_word_t arg1);
sc_word_t do_syscall2(long sc, sc_word_t arg1, sc_word_t arg2);
sc_word_t do_syscall3(long sc, sc_word_t arg1, sc_word_t arg2, sc_word_t arg3);
sc_word_t do_syscall4(long sc, sc_word_t arg1, sc_word_t arg2, sc_word_t arg3, sc_word_t arg4);
sc_word_t do_syscall5(long sc, sc_word_t arg1, sc_word_t arg2, sc_word_t arg3, sc_word_t arg4, sc_word_t arg5);
sc_word_t do_syscall6(long sc, sc_word_t arg1, sc_word_t arg2, sc_word_t arg3, sc_word_t arg4, sc_word_t arg5, sc_word_t arg6);


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
__attribute__((always_inline)) static inline long syscall(sc_word_t call, T... args) {
    return _do_syscall(call, sc_cast(args)...);
}

#endif 

#endif // SYSCALL_H
