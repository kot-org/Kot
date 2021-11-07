#pragma once
#include "../arch/x86-64/interrupts/interrupts.h"

extern "C" void SyscallInt_Handler(InterruptStack* Registers, uint64_t CoreID);

int GetKernelInfo(uint64_t InfoID, void* Address);