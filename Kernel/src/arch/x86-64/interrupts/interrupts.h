#pragma once
#include <arch/x86-64/interrupts/idt.h>
#include <arch/x86-64/io/io.h>
#include <lib/types.h>
#include <logs/logs.h>
#include <memory/paging/pageFrameAllocator.h>
#include <scheduler/scheduler.h>
#include <event/event.h>

#define IRQ_START 0x20
#define IRQ_MAX 0x18

#define Exception_PageFault 0xE

struct InterruptStack {
    uint64_t rax; uint64_t rbx; uint64_t rcx; uint64_t rdx; uint64_t rsi; uint64_t rdi; uint64_t rbp; //push in asm

    uint64_t r8; uint64_t r9; uint64_t r10; uint64_t r11; uint64_t r12; uint64_t r13; uint64_t r14; uint64_t r15; //push in asm

    uint64_t InterruptNumber; uint64_t ErrorCode; uint64_t rip; uint64_t cs; uint64_t rflags; uint64_t rsp; uint64_t ss; //push by cpu with an interrupt
}__attribute__((packed));

struct ProcessorInterruptStack {
    void* rip; void* cs; void* rflags; void* rsp; void* ss; //push by cpu with an interrupt
}__attribute__((packed));

struct IRQRedirect{
    void* stack;
    void* cr3; 
    void* functionAddress;
}__attribute__((packed));


void InitializeInterrupts();
void ExceptionHandler(InterruptStack* Registers, uint64_t CoreID);
bool PageFaultHandler(InterruptStack* Registers, uint64_t CoreID);
uint8_t GetCodeRing(InterruptStack* Registers);
void KernelUnrecovorable(InterruptStack* Registers, uint64_t CoreID);

extern IDTR idtr;
extern void* InterruptEntryList[256];
extern struct event_t* InterruptEventList[256];


uint64_t SetIrq(struct process_t* parent, void* entryPoint, uint8_t irq);
uint64_t SetIrqDefault(uint8_t irq);
void RedirectIRQ(InterruptStack* Registers, uint64_t CoreID, uint8_t irq);

