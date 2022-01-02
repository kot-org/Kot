#pragma once
#include "../arch/x86-64/interrupts/interrupts.h"
#include "../fileSystem/KFS/kfs.h"
#include "../scheduling/scheduler/scheduler.h"
#include "syscallList.h"

extern "C" void SyscallInt_Handler(struct InterruptStack* Registers, uint64_t CoreID);

uint64_t LogHandler(uint64_t type, char* str);

uint64_t mmap(PageTableManager* pageTable, void* addressPhysical, void* addressVirtual);
uint64_t munmap(PageTableManager* pageTable, void* addressVirtual);