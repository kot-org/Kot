#pragma once
#include "../io/ports.h"
#include "../drivers/keyboard/keyboarddriver.h"
#include "../io/serial.h"
#include "../panic.h"
#include "../drivers/display/displaydriver.h"
#include "../drivers/mouse/mouse.h"
#include "../scheduling/pit.h"

#define PIC1_COMMAND 0x20
#define PIC1_DATA 0x21
#define PIC2_COMMAND 0xA0
#define PIC2_DATA 0xA1
#define PIC_EOI 0x20

#define ICW1_INIT 0x10
#define ICW1_ICW4 0x01
#define ICW4_8086 0x01

__attribute__((interrupt)) void InvalideOpcodeHandler(struct IntreruptFrame* frame);//6
__attribute__((interrupt)) void GeneralProtectionFaultHandler(struct IntreruptFrame* frame);//13
__attribute__((interrupt)) void PageFaultHandler(struct IntreruptFrame* frame); //14
__attribute__((interrupt)) void DoubleFaultHandler(struct IntreruptFrame* frame);
__attribute__((interrupt)) void KBHandler(struct IntreruptFrame* frame);
__attribute__((interrupt)) void MSHandler(struct IntreruptFrame* frame);
__attribute__((interrupt)) void PITHandler(struct IntreruptFrame* frame);


void RemapPIC();
void PIC_EndMaster();
void PIC_EndSlave();