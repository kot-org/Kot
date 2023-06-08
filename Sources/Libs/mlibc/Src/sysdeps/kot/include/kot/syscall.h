#ifndef KOT_SYSCALL_H
#define KOT_SYSCALL_H

#include <kot/syscall/list.h>

#include <stdint.h>
#include <kot/sys.h>
#include <kot/types.h>

#if defined(__cplusplus)
extern "C" {
#endif

#define Syscall_48(syscall, arg0, arg1, arg2, arg3, arg4, arg5) (DoSyscall48(syscall, (uint64_t)arg0, (uint64_t)arg1, (uint64_t)arg2, (uint64_t)arg3, (uint64_t)arg4, (uint64_t)arg5))
#define Syscall_40(syscall, arg0, arg1, arg2, arg3, arg4) (DoSyscall40(syscall, (uint64_t)arg0, (uint64_t)arg1, (uint64_t)arg2, (uint64_t)arg3, (uint64_t)arg4))
#define Syscall_32(syscall, arg0, arg1, arg2, arg3) (DoSyscall32(syscall, (uint64_t)arg0, (uint64_t)arg1, (uint64_t)arg2, (uint64_t)arg3))
#define Syscall_24(syscall, arg0, arg1, arg2) (DoSyscall24(syscall, (uint64_t)arg0, (uint64_t)arg1, (uint64_t)arg2))
#define Syscall_16(syscall, arg0, arg1) (DoSyscall16(syscall, (uint64_t)arg0, (uint64_t)arg1))
#define Syscall_8(syscall, arg0) (DoSyscall8(syscall, (uint64_t)arg0))
#define Syscall_0(syscall) (DoSyscall0(syscall))

__attribute__((always_inline))
static inline uint64_t DoSyscall0(uint64_t call) {
    volatile uint64_t ret;
    asm volatile("syscall" : "=a"(ret) : "a"(call)); 
    return ret;
}

__attribute__((always_inline))
static uint64_t DoSyscall8(uint64_t call, uint64_t arg0) {
    volatile uint64_t ret;
    asm volatile("syscall" : "=a"(ret) : "a"(call), "D"(arg0) : "memory"); 
    return ret;
}

__attribute__((always_inline))
static uint64_t DoSyscall16(uint64_t call, uint64_t arg0, uint64_t arg1) {
    volatile uint64_t ret;
    asm volatile("syscall" : "=a"(ret) : "a"(call), "D"(arg0), "S"(arg1) : "memory"); 
    return ret;
}

__attribute__((always_inline))
static uint64_t DoSyscall24(uint64_t call, uint64_t arg0, uint64_t arg1, uint64_t arg2) {
    volatile uint64_t ret;
    asm volatile("syscall" : "=a"(ret) : "a"(call), "D"(arg0), "S"(arg1), "d"(arg2) : "memory"); 
    return ret;
}

__attribute__((always_inline))
static uint64_t DoSyscall32(uint64_t call, uint64_t arg0, uint64_t arg1, uint64_t arg2, uint64_t arg3) {
    volatile uint64_t ret;
	register uint64_t arg3r asm("r10") = arg3; // put arg3 in r10
    asm volatile("syscall" : "=a"(ret) : "a"(call), "D"(arg0), "S"(arg1), "d"(arg2), "r"(arg3r) : "memory"); 
    return ret;
}

__attribute__((always_inline))
static uint64_t DoSyscall40(uint64_t call, uint64_t arg0, uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4) {
    volatile uint64_t ret;
	register uint64_t arg3r asm("r10") = arg3; // put arg3 in r10
	register uint64_t arg4r asm("r9") = arg4; // put arg4 in r9
    asm volatile("syscall" : "=a"(ret) : "a"(call), "D"(arg0), "S"(arg1), "d"(arg2), "r"(arg3r), "r"(arg4r) : "memory"); 
    return ret;
}

__attribute__((always_inline))
static uint64_t DoSyscall48(uint64_t call, uint64_t arg0, uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4, uint64_t arg5) {
    volatile uint64_t ret;
	register uint64_t arg3r asm("r10") = arg3; // put arg3 in r10
	register uint64_t arg4r asm("r9") = arg4; // put arg4 in r9
	register uint64_t arg5r asm("r8") = arg5; // put arg5 in r8
    asm volatile("syscall" : "=a"(ret) : "a"(call), "D"(arg0), "S"(arg1), "d"(arg2), "r"(arg3r), "r"(arg4r), "r"(arg5r) : "memory"); 
    return ret;
}

#if defined(__cplusplus)
} 
#endif

#endif
