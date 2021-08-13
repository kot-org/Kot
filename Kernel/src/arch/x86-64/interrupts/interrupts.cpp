#include "interrupts.h"

IDTR idtr;

void InitializeInterrupts(){
    if(idtr.Limit == 0){
        idtr.Limit = 0x0FFF;
        idtr.Offset = (uint64_t)globalAllocator.RequestPage();
    }


    /* Page Fault */
    SetIDTGate((void*)Entry_PageFault_Handler, 0x0E, InterruptGateType, KernelRing, GDTInfoSelectorsRing[KernelRing].Code, idtr);

    /* Double Fault */
    SetIDTGate((void*)Entry_DoubleFault_Handler, 0x0C, InterruptGateType, KernelRing, GDTInfoSelectorsRing[KernelRing].Code, idtr);

    /* GP Fault */
    SetIDTGate((void*)Entry_GPFault_Handler, 0x0D, InterruptGateType, KernelRing, GDTInfoSelectorsRing[KernelRing].Code, idtr);

    /* Keyboard */
    SetIDTGate((void*)Entry_KeyboardInt_Handler, 0x21, InterruptGateType, KernelRing, GDTInfoSelectorsRing[KernelRing].Code, idtr);

    /* Mouse */
    SetIDTGate((void*)Entry_MouseInt_Handler, 0x2C, InterruptGateType, KernelRing, GDTInfoSelectorsRing[KernelRing].Code, idtr);

    /* PIT */
    SetIDTGate((void*)Entry_PITInt_Handler, 0x20, InterruptGateType, KernelRing, GDTInfoSelectorsRing[KernelRing].Code, idtr);

    /* APIC Timer */
    SetIDTGate((void*)Entry_LAPICTIMERInt_Handler, 0x30, InterruptGateType, KernelRing, GDTInfoSelectorsRing[KernelRing].Code, idtr);
    
    /* Syscall */
    SetIDTGate((void*)Entry_SyscallInt_Handler, 0x80, InterruptGateType, UserAppRing, GDTInfoSelectorsRing[KernelRing].Code, idtr);
    
    RemapPIC();
    PIT::SetDivisor(uint16_Limit);

    asm ("lidt %0" : : "m" (idtr));        
}

extern "C" void PageFault_Handler(ErrorInterruptStack* Registers){
    Panic("Page Fault Detected");
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

static uint64_t mutexScheduler;

extern "C" void PITInt_Handler(InterruptStack* Registers){
    PIT::Tick();
    PIC_EndMaster();       
}

extern "C" void LAPICTIMERInt_Handler(InterruptStack* Registers, uint64_t CoreID){
    Atomic::atomicSpinlock(&mutexScheduler, 0);
    Atomic::atomicLock(&mutexScheduler, 0);
    globalTaskManager.Scheduler(Registers, CoreID); 
    
    APIC::localApicEOI();
    Atomic::atomicUnlock(&mutexScheduler, 0);
}

static uint64_t mutexSyscall;

extern "C" void SyscallInt_Handler(InterruptStack* Registers, uint64_t CoreID){
    Atomic::atomicSpinlock(&mutexSyscall, 0);
    Atomic::atomicLock(&mutexSyscall, 0);

    uint64_t syscall = (uint64_t)Registers->rax;
    uint64_t arg0 = (uint64_t)Registers->rdi;
    uint64_t arg1 = (uint64_t)Registers->rsi;
    uint64_t arg2 = (uint64_t)Registers->rdx;
    uint64_t arg3 = (uint64_t)Registers->r10;
    uint64_t arg4 = (uint64_t)Registers->r8;
    uint64_t arg5 = (uint64_t)Registers->r9;

    switch(syscall){
        case 0x01:
            break;
    }

    //globalGraphics->Print((char*)arg5);
    Atomic::atomicUnlock(&mutexSyscall, 0);    
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