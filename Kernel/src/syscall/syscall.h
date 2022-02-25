#pragma once
#include <event/event.h>
#include <kot/sys/list.h>
#include <memory/memory.h>
#include <scheduler/scheduler.h>
#include <arch/x86-64/interrupts/interrupts.h>

extern "C" void syscallEnable(uint16_t KernelSegment, uint16_t UserSegment);

extern "C" uint64_t SyscallHandler(struct InterruptStack* Registers, uint64_t CoreID);

uint64_t mmap(struct thread_t* task, void* addressVirtual, bool usePhysicallAddress, void* addressPhysical);
uint64_t munmap(struct thread_t* task, void* addressVirtual);