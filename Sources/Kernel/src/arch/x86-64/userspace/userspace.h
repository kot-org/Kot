#pragma once
#include <kot/types.h>
#include <arch/arch.h>

extern "C" void IdleTask();

extern "C" void LaunchUserSpace();
extern "C" void ForceSchedule();
extern "C" void ForceScheduleAndSetBit(uint8_t* address);