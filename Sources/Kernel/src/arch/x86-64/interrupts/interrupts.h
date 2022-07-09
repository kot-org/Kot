#pragma once
#include <arch/arch.h>
#include <kot/types.h>
#include <kot/x86_64.h>
#include <logs/logs.h>
#include <event/event.h>
#include <scheduler/scheduler.h>

#define Exception_PageFault 0xE

#define IST_Null 0x0
#define IST_Interrupts 0x1

#define IRQ_START 0x20

void InitializeInterrupts(ArchInfo_t* ArchInfo);
void ExceptionHandler(ContextStack* Registers, uint64_t CoreID);
bool PageFaultHandler(ContextStack* Registers, uint64_t CoreID);
void KernelUnrecovorable(ContextStack* Registers, uint64_t CoreID);

extern struct IDTR idtr;
extern uintptr_t InterruptEntryList[256];
extern struct event_t* InterruptEventList[256];