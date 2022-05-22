#pragma once
#include <kot/types.h>
#include <event/event.h>
#include <kot/sys/list.h>
#include <memory/memory.h>
#include <scheduler/scheduler.h>
#include <arch/x86-64/interrupts/interrupts.h>

typedef uint64_t (*SyscallHandler)(ContextStack* Registers, struct thread_t* Thread);

extern "C" void SyscallEnable(uint16_t KernelSegment, uint16_t UserSegment);

extern "C" uint64_t SyscallDispatch(struct ContextStack* Registers, struct thread_t* Self);