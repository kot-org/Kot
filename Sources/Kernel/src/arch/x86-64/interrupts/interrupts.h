#pragma once
#include <arch/arch.h>
#include <kot/types.h>
#include <kot/x86_64.h>
#include <logs/logs.h>
#include <event/event.h>
#include <scheduler/scheduler.h>

#define Exception_PageFault 0xE

#define IST_Null 0x0
#define IST_Scheduler 0x1

void InitializeInterrupts();
void ExceptionHandler(ContextStack* Registers, uint64_t CoreID);
bool PageFaultHandler(ContextStack* Registers, uint64_t CoreID);
void KernelUnrecovorable(ContextStack* Registers, uint64_t CoreID);

extern struct IDTR idtr;
extern uintptr_t InterruptEntryList[256];
extern struct event_t* InterruptEventList[256];