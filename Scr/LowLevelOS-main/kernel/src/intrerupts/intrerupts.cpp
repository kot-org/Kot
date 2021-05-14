#include "intrerupts.h"

__attribute__((interrupt)) void GeneralProtectionFaultHandler(struct IntreruptFrame* frame) {
    KernelPanic("General Protection Fault",frame);
    while(1);
}
__attribute__((interrupt)) void PageFaultHandler(struct IntreruptFrame* frame) {
    KernelPanic("Page Fault",frame);
    while(1);
}
__attribute__((interrupt)) void DoubleFaultHandler(struct IntreruptFrame* frame) {
    KernelPanic("Double Fault",frame);
    while(1);
}

__attribute__((interrupt)) void InvalideOpcodeHandler(struct IntreruptFrame* frame) {
    KernelPanic("Invalid Opcode",frame);
    while(1);
}

__attribute__((interrupt)) void KBHandler(struct IntreruptFrame* frame) {
    uint8_t keycode = inportb(0x60);
    GlobalKeyboard->Handle(keycode);
    PIC_EndMaster();
}

__attribute__((interrupt)) void MSHandler(struct IntreruptFrame* frame) {
    uint8_t data = inportb(0x60);
    GlobalMouse->Handle(data);
    PIC_EndSlave();
}

__attribute__((interrupt)) void PITHandler(struct IntreruptFrame* frame) {
    PITTick();
    PIC_EndMaster();
}

void PIC_EndMaster(){
    outportb(PIC1_COMMAND, PIC_EOI);
}

void PIC_EndSlave(){
    outportb(PIC2_COMMAND, PIC_EOI);
    outportb(PIC1_COMMAND, PIC_EOI);
}
   

void RemapPIC(){
    uint8_t a1, a2; 

    a1 = inportb(PIC1_DATA);
    io_wait();
    a2 = inportb(PIC2_DATA);
    io_wait();

    outportb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);
    io_wait();
    outportb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
    io_wait();

    outportb(PIC1_DATA, 0x20);
    io_wait();
    outportb(PIC2_DATA, 0x28);
    io_wait();

    outportb(PIC1_DATA, 4);
    io_wait();
    outportb(PIC2_DATA, 2);
    io_wait();

    outportb(PIC1_DATA, ICW4_8086);
    io_wait();
    outportb(PIC2_DATA, ICW4_8086);
    io_wait();

    outportb(PIC1_DATA, a1);
    io_wait();
    outportb(PIC2_DATA, a2);

}