#pragma once
#include "../arch/x86-64/interrupts/interrupts.h"
#include "../scheduling/scheduler/scheduler.h"
#include "../memory/memory.h"
#include "syscallList.h"

extern "C" void syscallEnable(uint16_t KernelSegment, uint16_t UserSegment);

extern "C" uint64_t SyscallHandler(struct InterruptStack* Registers, uint64_t CoreID);


uint64_t LogHandler(uint64_t type, char* str);

uint64_t mmap(PageTableManager* pageTable, void* addressVirtual, bool usePhysicallAddress, void* addressPhysical);
uint64_t munmap(PageTableManager* pageTable, void* addressVirtual);