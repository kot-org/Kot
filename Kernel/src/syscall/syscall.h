#pragma once
#include "../arch/x86-64/interrupts/interrupts.h"
#include "../fileSystem/KFS/kfs.h"
#include "../scheduling/scheduler/scheduler.h"
#include "syscallList.h"

extern "C" void SyscallInt_Handler(InterruptStack* Registers, uint64_t CoreID);

uint64_t KernelRuntime(TaskContext* task, uint64_t arg0, uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4, uint64_t arg5);

uint64_t LogHandler(uint64_t type, char* str);

uint64_t mmap(PageTableManager* pageTable, void* addressPhysical, void* addressVirtual);
uint64_t munmap(PageTableManager* pageTable, void* addressVirtual);