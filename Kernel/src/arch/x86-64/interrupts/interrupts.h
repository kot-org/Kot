#pragma once
#include "idt.h"
#include "../io/io.h"
#include "../../../lib/types.h"
#include "../../../logs/logs.h"
#include "../../../drivers/graphics/graphics.h"
#include "../../../misc/panic/panic.h"
#include "../../../drivers/hid/keyboard.h"
#include "../../../drivers/hid/mouse.h"
#include "../../../scheduling/pit/pit.h"
#include "../../../memory/paging/pageFrameAllocator.h"
#include "../../../scheduling/scheduler/scheduler.h"

#define PIC1_COMMAND 0x20
#define PIC1_DATA 0x21
#define PIC2_COMMAND 0xA0
#define PIC2_DATA 0xA1
#define PIC_EOI 0x20

#define ICW1_INIT 0x10
#define ICW1_ICW4 0x01
#define ICW4_8086 0x01


struct InterruptStack {
    void* rax; void* rbx; void* rcx; void* rdx; void* rsi; void* rdi; void* rbp; //push in asm

    void* r8; void* r9; void* r10; void* r11; void* r12; void* r13; void* r14; void* r15; //push in asm

    void* rip; void* cs; void* rflags; void* rsp; void* ss; //push by cpu with an interrupt
}__attribute__((packed));

struct ErrorInterruptStack {
    void* rax; void* rbx; void* rcx; void* rdx; void* rsi; void* rdi; void* rbp; //push in asm

    void* r8; void* r9; void* r10; void* r11; void* r12; void* r13; void* r14; void* r15; //push in asm

    void* errorCode; void* rip; void* cs; void* rflags; void* rsp; void* ss; //push by cpu with an interrupt
}__attribute__((packed));

extern IDTR idtr;

void InitializeInterrupts();


extern "C" void DivideByZero_Handler(InterruptStack* Registers, uint64_t CoreID);
extern "C" void Debug_Handler(InterruptStack* Registers, uint64_t CoreID);
extern "C" void NMI_Handler(InterruptStack* Registers, uint64_t CoreID);
extern "C" void Breakpoint_Handler(InterruptStack* Registers, uint64_t CoreID);
extern "C" void Overflow_Handler(InterruptStack* Registers, uint64_t CoreID);
extern "C" void BoundRangeExceeded_Handler(InterruptStack* Registers, uint64_t CoreID);
extern "C" void InvalidOpcode_Handler(InterruptStack* Registers, uint64_t CoreID);
extern "C" void DeviceNotAvailable_Handler(InterruptStack* Registers, uint64_t CoreID);
extern "C" void DoubleFault_Handler(ErrorInterruptStack* Registers, uint64_t CoreID);
extern "C" void InvalidTSS_Handler(ErrorInterruptStack* Registers, uint64_t CoreID);
extern "C" void SegmentNotPresent_Handler(ErrorInterruptStack* Registers, uint64_t CoreID);
extern "C" void StackSegmentFault_Handler(ErrorInterruptStack* Registers, uint64_t CoreID);
extern "C" void GPFault_Handler(ErrorInterruptStack* Registers, uint64_t CoreID);
extern "C" void PageFault_Handler(ErrorInterruptStack* Registers, uint64_t CoreID, void* Address);
extern "C" void x87FloatingPointException_Handler(InterruptStack* Registers, uint64_t CoreID);
extern "C" void AlignmentCheck_Handler(ErrorInterruptStack* Registers, uint64_t CoreID);
extern "C" void MachineCheck_Handler(InterruptStack* Registers, uint64_t CoreID);
extern "C" void SIMDFloatingPointException_Handler(InterruptStack* Registers, uint64_t CoreID);
extern "C" void VirtualizationException_Handler(InterruptStack* Registers, uint64_t CoreID);
extern "C" void SecurityException_Handler(ErrorInterruptStack* Registers, uint64_t CoreID);

extern "C" void KeyboardInt_Handler(InterruptStack* Registers);
extern "C" void MouseInt_Handler(InterruptStack* Registers);
extern "C" void PITInt_Handler(InterruptStack* Registers);
extern "C" void LAPICTIMERInt_Handler(InterruptStack* Registers, uint64_t CoreID);

void RemapPIC();
void PIC_EndMaster();
void PIC_EndSlave();

extern "C" void Entry_DivideByZero_Handler();
extern "C" void Entry_Debug_Handler();
extern "C" void Entry_NMI_Handler();
extern "C" void Entry_Breakpoint_Handler();
extern "C" void Entry_Overflow_Handler();
extern "C" void Entry_BoundRangeExceeded_Handler();
extern "C" void Entry_InvalidOpcode_Handler();
extern "C" void Entry_DeviceNotAvailable_Handler();
extern "C" void Entry_DoubleFault_Handler();
extern "C" void Entry_InvalidTSS_Handler();
extern "C" void Entry_SegmentNotPresent_Handler();
extern "C" void Entry_StackSegmentFault_Handler();
extern "C" void Entry_GPFault_Handler();
extern "C" void Entry_PageFault_Handler();
extern "C" void Entry_x87FloatingPointException_Handler();
extern "C" void Entry_AlignmentCheck_Handler();
extern "C" void Entry_MachineCheck_Handler();
extern "C" void Entry_SIMDFloatingPointException_Handler();
extern "C" void Entry_VirtualizationException_Handler();
extern "C" void Entry_SecurityException_Handler();

extern "C" void Entry_KeyboardInt_Handler();
extern "C" void Entry_MouseInt_Handler();
extern "C" void Entry_PITInt_Handler();
extern "C" void Entry_LAPICTIMERInt_Handler();
extern "C" void Entry_SyscallInt_Handler();

