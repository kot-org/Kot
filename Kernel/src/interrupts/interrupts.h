#pragma once
#include "IDT.h"
#include "../graphics.h"
#include "../panic/panic.h"
#include "../IO/IO.h"
#include "../userInput/keyboard.h"
#include "../userInput/mouse.h"
#include "../scheduling/pit/pit.h"
#include "../scheduling/scheduler/scheduler.h"

#define PIC1_COMMAND 0x20
#define PIC1_DATA 0x21
#define PIC2_COMMAND 0xA0
#define PIC2_DATA 0xA1
#define PIC_EOI 0x20

#define ICW1_INIT 0x10
#define ICW1_ICW4 0x01
#define ICW4_8086 0x01


typedef struct InterruptStack {
    void* rax; void* rbx; void* rcx; void* rdx; void* KernelRsp; void* rsi; void* rdi; void* rbp; //push in asm

    void* r8; void* r9; void* r10; void* r11; void* r12; void* r13; void* r14; void* r15; //push in asm

    void* rip; void* cs; void* rflags; void* rsp; void* ss; //push by cpu with an interrupt
};

typedef struct ErrorInterruptStack {
    void* rax; void* rbx; void* rcx; void* rdx; void* KernelRsp; void* rsi; void* rdi; void* rbp; //push in asm

    void* r8; void* r9; void* r10; void* r11; void* r12; void* r13; void* r14; void* r15; //push in asm

    void* errorCode; void* rip; void* cs; void* rflags; void* rsp; void* ss; //push by cpu with an interrupt
}__attribute__((packed));

void InitializeInterrupts();

extern "C" void PageFault_Handler(ErrorInterruptStack* Registers);
extern "C" void DoubleFault_Handler(ErrorInterruptStack* Registers);
extern "C" void GPFault_Handler(ErrorInterruptStack* Registers);
extern "C" void KeyboardInt_Handler(InterruptStack* Registers);
extern "C" void MouseInt_Handler(InterruptStack* Registers);
extern "C" void PITInt_Handler(InterruptStack* Registers);

void RemapPIC();
void PIC_EndMaster();
void PIC_EndSlave();

extern "C" void Entry_PageFault_Handler();
extern "C" void Entry_DoubleFault_Handler();
extern "C" void Entry_GPFault_Handler();
extern "C" void Entry_KeyboardInt_Handler();
extern "C" void Entry_MouseInt_Handler();
extern "C" void Entry_PITInt_Handler();
