#include "interrupts.h"

extern "C" void PageFault_Handler(){
    Panic("Page Fault Detected");
    while(true);
}

extern "C" void DoubleFault_Handler(){
    Panic("Double Fault Detected");
    while(true);
}

extern "C" void GPFault_Handler(){
    Panic("General Protection Fault Detected");
    while(true);
}
extern "C" void KeyboardInt_Handler(){
    uint8_t scancode = IoRead8(0x60);
    HandleKeyboard(scancode);
    PIC_EndMaster();
}

extern "C" void MouseInt_Handler(){
    uint8_t mousedata = IoRead8(0x60);
    HandlePS2Mouse(mousedata);
    PIC_EndSlave();
}

extern "C" void* PITInt_Handler(InterruptStack* Registers){
    PIT::Tick();
    void* NewRSP = scheduler.Scheduler(Registers);        
    PIC_EndMaster();
    return NewRSP;
}

void RemapPIC(){
    uint8_t a1, a2;

    a1 = IoRead8(PIC1_DATA);
    io_wait();
    a2 = IoRead8(PIC2_DATA);
    io_wait();

    IoWrite8(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);
    io_wait();
    IoWrite8(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
    io_wait();

    IoWrite8(PIC1_DATA, 0x20);
    io_wait();
    IoWrite8(PIC2_DATA, 0x28);
    io_wait();

    IoWrite8(PIC1_DATA, 4);
    io_wait();
    IoWrite8(PIC2_DATA, 2);
    io_wait();

    IoWrite8(PIC1_DATA, ICW4_8086);
    io_wait();
    IoWrite8(PIC2_DATA, ICW4_8086);
    io_wait();

    IoWrite8(PIC1_DATA, a1);
    io_wait();
    IoWrite8(PIC2_DATA, a2);
}

void PIC_EndMaster(){
    IoWrite8(PIC1_COMMAND, PIC_EOI);
}

void PIC_EndSlave(){
    IoWrite8(PIC2_COMMAND, PIC_EOI);
    IoWrite8(PIC1_COMMAND, PIC_EOI);
}