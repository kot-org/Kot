#pragma once
#include <arch/arch.h>
#include <lib/types.h>
#include <lib/x86_64.h>
#include <logs/logs.h>
#include <event/event.h>
#include <scheduler/scheduler.h>

#define Exception_PageFault 0xE
#define Exception_End       0x20

#define IST_Null                0x0
#define IST_Interrupts          0x1
#define IST_SchedulerAPIC       0x2
#define IST_Scheduler           0x3
#define IST_DestroyPauseSelf    0x4

#define IRQ_START 0x20

void InitializeInterrupts(ArchInfo_t* ArchInfo);
void ExceptionHandler(uint64_t Cr2, ContextStack* Registers, uint64_t CoreID);
bool PageFaultHandler(uint64_t Cr2, ContextStack* Registers, uint64_t CoreID);
void KernelUnrecovorable(uint64_t Cr2, ContextStack* Registers, uint64_t CoreID);

extern struct IDTR idtr;
extern void* InterruptEntryList[256];
extern struct kevent_t* InterruptEventList[256];