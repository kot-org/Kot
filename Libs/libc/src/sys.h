#ifndef _SYS_H
#define _SYS_H 1

#include <kot/types.h>
#include <kot/sys/list.h>

#define ASMMACRO(X) #X

#define Syscall_48(syscall, arg0, arg1, arg2, arg3, arg4, arg5) ({asm("mov %p6, %%r9\n\t""mov %p5, %%r8\n\t""mov %p4, %%r10\n\t""mov %p3, %%rdx\n\t""mov %p2, %%rsi\n\t""mov %p1, %%rdi\n\t""mov $" ASMMACRO(syscall) ", %%rax\n\t""syscall\n\t":"=a"(ReturnValue):[p1]"m"(arg0),[p2]"m"(arg1),[p3]"m"(arg2),[p4]"m"(arg3),[p5]"m"(arg4),[p6]"m"(arg5));})
#define Syscall_40(syscall, arg0, arg1, arg2, arg3, arg4) ({asm("mov %p5, %%r8\n\t""mov %p4, %%r10\n\t""mov %p3, %%rdx\n\t""mov %p2, %%rsi\n\t""mov %p1, %%rdi\n\t""mov $" ASMMACRO(syscall) ", %%rax\n\t""syscall\n\t":"=a"(ReturnValue):[p1]"m"(arg0),[p2]"m"(arg1),[p3]"m"(arg2),[p4]"m"(arg3),[p5]"m"(arg4));})
#define Syscall_32(syscall, arg0, arg1, arg2, arg3) ({asm("mov %p4, %%r10\n\t""mov %p3, %%rdx\n\t""mov %p2, %%rsi\n\t""mov %p1, %%rdi\n\t""mov $" ASMMACRO(syscall) ", %%rax\n\t""syscall\n\t":"=a"(ReturnValue):[p1]"m"(arg0),[p2]"m"(arg1),[p3]"m"(arg2),[p4]"m"(arg3));})
#define Syscall_24(syscall, arg0, arg1, arg2) ({asm("mov %p3, %%rdx\n\t""mov %p2, %%rsi\n\t""mov %p1, %%rdi\n\t""mov $" ASMMACRO(syscall) ", %%rax\n\t""syscall\n\t":"=a"(ReturnValue):[p1]"m"(arg0),[p2]"m"(arg1),[p3]"m"(arg2));})
#define Syscall_16(syscall, arg0, arg1) ({asm("mov %p2, %%rsi\n\t""mov %p1, %%rdi\n\t""mov $" ASMMACRO(syscall) ", %%rax\n\t""syscall\n\t":"=a"(ReturnValue):[p1]"m"(arg0),[p2]"m"(arg1));})
#define Syscall_8(syscall, arg0) ({asm("mov %p1, %%rdi\n\t""mov $" ASMMACRO(syscall) ", %%rax\n\t""syscall\n\t":"=a"(ReturnValue):[p1]"m"(arg0));})
#define Syscall_0(syscall) ({asm("mov $" ASMMACRO(syscall) ", %%rax\n\t""syscall\n\t":"=a"(ReturnValue):);})

#if defined(__cplusplus)
extern "C" {
#endif

struct SelfData{
    key_t ThreadKey;
    key_t ProcessKey;
}__attribute__((packed));

KResult SYS_CreatShareSpace(kthread_t self, size_t size, uint64_t* virtualAddressPointer, uint64_t* keyPointer, bool ReadOnly);
KResult SYS_GetShareSpace(kthread_t self, uint64_t key, uint64_t* virtualAddressPointer);
KResult SYS_FreeShareSpace(kthread_t self, void* virtualAddress);
KResult SYS_Fork(kthread_t task, struct parameters_t* param);
KResult SYS_Exit(kthread_t self, uint64_t errorCode);
KResult SYS_Pause(kthread_t self);
KResult SYS_Unpause(kthread_t self);
KResult SYS_Map(kthread_t self, uint64_t* addressVirtual, bool isPhysical, void* addressPhysical, size_t size, bool findFree);
KResult SYS_Unmap(kthread_t self, void* addressVirtual, size_t size);

KResult SYS_GetThreadKey(kthread_t* self);
KResult SYS_GetProcessKey(process_t* self);

#if defined(__cplusplus)
} 
#endif

#endif