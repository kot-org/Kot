#pragma once
#include <kot/types.h>
#include <arch/arch.h>

extern "C" void IdleTask();

extern "C" void LaunchUserSpace();
extern "C" void ForceSchedule()__attribute__((noreturn));
extern "C" void ForceScheduleAndSetBit(uint8_t* address)__attribute__((noreturn));
extern "C" void ForceScheduleAndClearBit(uint8_t* address)__attribute__((noreturn));