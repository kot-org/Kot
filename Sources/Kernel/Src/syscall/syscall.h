#pragma once
#include <mm/mm.h>
#include <lib/pid.h>
#include <lib/wait.h>
#include <lib/types.h>
#include <event/event.h>
#include <memory/memory.h>
#include <syscall/kot/kot.h>
#include <syscall/std/std.h>
#include <kot/syscall/list.h>
#include <scheduler/scheduler.h>
#include <arch/x86-64/interrupts/interrupts.h>

#define SYSCALL_ARG0(Registers) ((Registers)->arg0)
#define SYSCALL_ARG1(Registers) ((Registers)->arg1)
#define SYSCALL_ARG2(Registers) ((Registers)->arg2)
#define SYSCALL_ARG3(Registers) ((Registers)->arg3)
#define SYSCALL_ARG4(Registers) ((Registers)->arg4)
#define SYSCALL_ARG5(Registers) ((Registers)->arg5)

typedef uint64_t (*SyscallHandler)(struct SyscallStack* Registers, struct kthread_t* Thread);

extern "C" void SyscallEnable(uint16_t KernelSegment, uint16_t UserSegment);

extern "C" void SyscallDispatch(struct SyscallStack* Registers, struct kthread_t* Self);