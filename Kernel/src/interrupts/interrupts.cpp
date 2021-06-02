#include "interrupts.h"

void InitializeInterrupts(){
    IDTR idtr;
    idtr.Limit = 0x0FFF;
    idtr.Offset = (uint64_t)globalAllocator.RequestPage();

    /* Page Fault */
    SetIDTGate((void*)Entry_PageFault_Handler, 0x0E, IDT_TA_InterruptGate, 0x08, idtr);

    /* Double Fault */
    SetIDTGate((void*)Entry_DoubleFault_Handler, 0x8, IDT_TA_InterruptGate, 0x08, idtr);

    /* GP Fault */
    SetIDTGate((void*)Entry_GPFault_Handler, 0xD, IDT_TA_InterruptGate, 0x08, idtr);

    /* Keyboard */
    SetIDTGate((void*)Entry_KeyboardInt_Handler, 0x21, IDT_TA_InterruptGate, 0x08, idtr);

    /* Mouse */
    SetIDTGate((void*)Entry_MouseInt_Handler, 0x2C, IDT_TA_InterruptGate, 0x08, idtr);

    /* PIT */
    SetIDTGate((void*)Entry_PITInt_Handler, 0x20, IDT_TA_InterruptGate, 0x08, idtr);
    PIT::SetDivisor(uint16_Limit);

    asm ("lidt %0" : : "m" (idtr)); 

    RemapPIC();    
}

extern "C" void PageFault_Handler(ErrorInterruptStack* Registers){
    Panic("Page Fault Detected");
    printf("\n%u\n", Registers->ss);
    globalGraphics->Update();
    while(true);
}

extern "C" void DoubleFault_Handler(ErrorInterruptStack* Registers){
    Panic("Double Fault Detected");
    while(true);
}

extern "C" void GPFault_Handler(ErrorInterruptStack* Registers){
    Panic("General Protection Fault Detected");
    while(true);
}

extern "C" void KeyboardInt_Handler(InterruptStack* Registers){
    uint8_t scancode = IoRead8(0x60);
    HandleKeyboard(scancode);
    PIC_EndMaster();
}

extern "C" void MouseInt_Handler(InterruptStack* Registers){
    uint8_t mousedata = IoRead8(0x60);
    HandlePS2Mouse(mousedata);
    PIC_EndSlave();

}

extern "C" void PITInt_Handler(InterruptStack* Registers){
    PIT::Tick();
    globalTaskManager.Scheduler(Registers); 
    PIC_EndMaster();       
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