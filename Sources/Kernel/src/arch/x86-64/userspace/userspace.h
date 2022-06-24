#pragma once
#include <kot/types.h>
#include <arch/arch.h>

extern "C" void IdleTask();

extern "C" void LaunchUserSpace();
extern "C" void ExitAndSetBit(uint8_t* address);