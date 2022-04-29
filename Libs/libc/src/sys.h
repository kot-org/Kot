#ifndef _SYS_H
#define _SYS_H 1

#include <kot/types.h>
#include <kot/sys/list.h>

#define ASMMACRO(X) #X

#define Syscall_48(syscall, arg0, arg1, arg2, arg3, arg4, arg5) ({asm volatile("mov %p6, %%r8\n\t""mov %p5, %%r9\n\t""mov %p4, %%r10\n\t""mov %p3, %%rdx\n\t""mov %p2, %%rsi\n\t""mov %p1, %%rdi\n\t""mov $" ASMMACRO(syscall) ", %%rax\n\t""syscall\n\t":"=a"(ReturnValue):[p1]"m"(arg0),[p2]"m"(arg1),[p3]"m"(arg2),[p4]"m"(arg3),[p5]"m"(arg4),[p6]"m"(arg5));})
#define Syscall_40(syscall, arg0, arg1, arg2, arg3, arg4) ({asm volatile("mov %p5, %%r9\n\t""mov %p4, %%r10\n\t""mov %p3, %%rdx\n\t""mov %p2, %%rsi\n\t""mov %p1, %%rdi\n\t""mov $" ASMMACRO(syscall) ", %%rax\n\t""syscall\n\t":"=a"(ReturnValue):[p1]"m"(arg0),[p2]"m"(arg1),[p3]"m"(arg2),[p4]"m"(arg3),[p5]"m"(arg4));})
#define Syscall_32(syscall, arg0, arg1, arg2, arg3) ({asm volatile("mov %p4, %%r10\n\t""mov %p3, %%rdx\n\t""mov %p2, %%rsi\n\t""mov %p1, %%rdi\n\t""mov $" ASMMACRO(syscall) ", %%rax\n\t""syscall\n\t":"=a"(ReturnValue):[p1]"m"(arg0),[p2]"m"(arg1),[p3]"m"(arg2),[p4]"m"(arg3));})
#define Syscall_24(syscall, arg0, arg1, arg2) ({asm volatile("mov %p3, %%rdx\n\t""mov %p2, %%rsi\n\t""mov %p1, %%rdi\n\t""mov $" ASMMACRO(syscall) ", %%rax\n\t""syscall\n\t":"=a"(ReturnValue):[p1]"m"(arg0),[p2]"m"(arg1),[p3]"m"(arg2));})
#define Syscall_16(syscall, arg0, arg1) ({asm volatile("mov %p2, %%rsi\n\t""mov %p1, %%rdi\n\t""mov $" ASMMACRO(syscall) ", %%rax\n\t""syscall\n\t":"=a"(ReturnValue):[p1]"m"(arg0),[p2]"m"(arg1));})
#define Syscall_8(syscall, arg0) ({asm volatile("mov %p1, %%rdi\n\t""mov $" ASMMACRO(syscall) ", %%rax\n\t""syscall\n\t":"=a"(ReturnValue):[p1]"m"(arg0));})
#define Syscall_0(syscall) ({asm volatile("mov $" ASMMACRO(syscall) ", %%rax\n\t""syscall\n\t":"=a"(ReturnValue):);})

#if defined(__cplusplus)
extern "C" {
#endif

struct SelfData{
    kprocess_t ThreadKey;
    kprocess_t ProcessKey;
}__attribute__((packed));

enum EventType{
    DataTypeUnknow = 0,
    DataTypeThread = 1,
    DataTypeProcess = 2,
    DataTypeEvent = 3,
    DataTypeMemory = 4,
};

KResult SYS_CreatShareSpace(kthread_t self, size_t size, uint64_t* virtualAddressPointer, uint64_t* keyPointer, bool ReadOnly);
KResult SYS_GetShareSpace(kthread_t self, uint64_t key, uint64_t* virtualAddressPointer);
KResult SYS_FreeShareSpace(kthread_t self, void* virtualAddress);
KResult SYS_Fork(kthread_t task, struct parameters_t* param);
KResult Sys_CreatProc(kprocess_t* key, uint8_t privilege, uint64_t data);
KResult Sys_CloseProc();
KResult SYS_Exit(kthread_t self, uint64_t errorCode);
KResult SYS_Pause(kthread_t self);
KResult SYS_Unpause(kthread_t self);
KResult SYS_Map(kthread_t self, uint64_t* addressVirtual, bool isPhysical, void* addressPhysical, size_t size, bool findFree);
KResult SYS_Unmap(kthread_t self, void* addressVirtual, size_t size);
KResult Sys_CreatThread(kprocess_t self, void* entryPoint, uint8_t privilege, uint64_t data, kthread_t* result);
KResult Sys_DuplicateThread(kprocess_t parent, kthread_t source, uint64_t data, kthread_t* self);
KResult Sys_ExecThread(kthread_t self, struct parameters_t* parameters);


KResult SYS_GetThreadKey(kthread_t* self);
KResult SYS_GetProcessKey(kprocess_t* self);

#if defined(__cplusplus)
} 
#endif

#endif