#pragma once

#include <kot/types.h>
#include <kot/sys/list.h>

#define Syscall_48(syscall, arg0, arg1, arg2, arg3, arg4, arg5) (DoSyscall(syscall, (uint64_t)arg0, (uint64_t)arg1, (uint64_t)arg2, (uint64_t)arg3, (uint64_t)arg4, (uint64_t)arg5))
#define Syscall_40(syscall, arg0, arg1, arg2, arg3, arg4) (DoSyscall(syscall, (uint64_t)arg0, (uint64_t)arg1, (uint64_t)arg2, (uint64_t)arg3, (uint64_t)arg4, 0))
#define Syscall_32(syscall, arg0, arg1, arg2, arg3) (DoSyscall(syscall, (uint64_t)arg0, (uint64_t)arg1, (uint64_t)arg2, (uint64_t)arg3, 0, 0))
#define Syscall_24(syscall, arg0, arg1, arg2) (DoSyscall(syscall, (uint64_t)arg0, (uint64_t)arg1, (uint64_t)arg2, 0, 0, 0))
#define Syscall_16(syscall, arg0, arg1) (DoSyscall(syscall, (uint64_t)arg0, (uint64_t)arg1, 0, 0, 0, 0))
#define Syscall_8(syscall, arg0) (DoSyscall(syscall, (uint64_t)arg0, 0, 0, 0, 0, 0))
#define Syscall_0(syscall) (DoSyscall(syscall, 0, 0, 0, 0, 0, 0))

#if defined(__cplusplus)
extern "C" {
#endif

uint64_t DoSyscall(uint64_t syscall, uint64_t arg0, uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4, uint64_t arg5);

KResult SYS_CreatShareSpace(kthread_t self, size_t size, uint64_t* virtualAddressPointer, uint64_t* keyPointer, bool ReadOnly);
KResult SYS_GetShareSpace(kthread_t self, uint64_t key, uint64_t* virtualAddressPointer);
KResult SYS_FreeShareSpace(kthread_t self, void* virtualAddress);
KResult SYS_Fork(kthread_t task, struct parameters_t* param);
KResult SYS_Exit(kthread_t self, uint64_t errorCode);
KResult SYS_Pause(kthread_t self);
KResult SYS_Unpause(kthread_t self);
KResult SYS_Map(kthread_t self, void* addressVirtual, bool isPhysical, void* addressPhysical);
KResult SYS_Unmap(kthread_t self, void* addressVirtual);


extern uint64_t _main_thread;
extern uint64_t _process;

#if defined(__cplusplus)
} 
#endif