#pragma once
#include <memory/paging/pageFrameAllocator.h>
#include <arch/x86-64/interrupts/idt.h>
#include <scheduler/scheduler.h>
#include <arch/x86-64/cpu/cpu.h>
#include <arch/x86-64/io/io.h>
#include <event/event.h>
#include <lib/types.h>
#include <logs/logs.h>

#define IRQ_START 0x20
#define IRQ_MAX 0x18

#define Exception_PageFault 0xE

void InitializeInterrupts();
void ExceptionHandler(ContextStack* Registers, uint64_t CoreID);
bool PageFaultHandler(ContextStack* Registers, uint64_t CoreID);
uint8_t GetCodeRing(ContextStack* Registers);
void KernelUnrecovorable(ContextStack* Registers, uint64_t CoreID);

extern struct IDTR idtr;
extern void* InterruptEntryList[256];
extern struct event_t* InterruptEventList[256];


uint64_t SetIrq(struct process_t* parent, void* entryPoint, uint8_t irq);
uint64_t SetIrqDefault(uint8_t irq);
void RedirectIRQ(ContextStack* Registers, uint64_t CoreID, uint8_t irq);

