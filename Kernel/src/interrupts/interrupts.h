#pragma once
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


extern "C" void PageFault_Handler();
extern "C" void DoubleFault_Handler();
extern "C" void GPFault_Handler();
extern "C" void KeyboardInt_Handler();
extern "C" void MouseInt_Handler();
extern "C" void* PITInt_Handler(void* stack);

void RemapPIC();
void PIC_EndMaster();
void PIC_EndSlave();

extern "C" void Entry_PageFault_Handler();
extern "C" void Entry_DoubleFault_Handler();
extern "C" void Entry_GPFault_Handler();
extern "C" void Entry_KeyboardInt_Handler();
extern "C" void Entry_MouseInt_Handler();
extern "C" void Entry_PITInt_Handler();
