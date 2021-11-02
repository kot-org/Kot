#include "interrupts.h"

IDTR idtr;

uint8_t IDTData[0x1000];

void InitializeInterrupts(){
    if(idtr.Limit == 0){
        idtr.Limit = 0x0FFF;
        idtr.Offset = (uint64_t)&IDTData[0];
    }

    /* Exceptions */

    /* Divide-by-zero */
    SetIDTGate((void*)Entry_DivideByZero_Handler, 0x0, InterruptGateType, KernelRing, GDTInfoSelectorsRing[KernelRing].Code, idtr);
    
    /* Debug */
    SetIDTGate((void*)Entry_Debug_Handler, 0x1, InterruptGateType, KernelRing, GDTInfoSelectorsRing[KernelRing].Code, idtr);
    
    /* NMI */
    SetIDTGate((void*)Entry_NMI_Handler, 0x2, InterruptGateType, KernelRing, GDTInfoSelectorsRing[KernelRing].Code, idtr);
    
    /* Breakpoint */
    SetIDTGate((void*)Entry_Breakpoint_Handler, 0x3, InterruptGateType, KernelRing, GDTInfoSelectorsRing[KernelRing].Code, idtr);
    
    /* Overflow */
    SetIDTGate((void*)Entry_Overflow_Handler, 0x4, InterruptGateType, KernelRing, GDTInfoSelectorsRing[KernelRing].Code, idtr);
    
    /* Bound Range Exceeded */
    SetIDTGate((void*)Entry_BoundRangeExceeded_Handler, 0x5, InterruptGateType, KernelRing, GDTInfoSelectorsRing[KernelRing].Code, idtr);
    
    /* Invalid Opcode */
    SetIDTGate((void*)Entry_InvalidOpcode_Handler, 0x6, InterruptGateType, KernelRing, GDTInfoSelectorsRing[KernelRing].Code, idtr);
    
    /* Device Not Available */
    SetIDTGate((void*)Entry_DeviceNotAvailable_Handler, 0x7, InterruptGateType, KernelRing, GDTInfoSelectorsRing[KernelRing].Code, idtr);

    /* Double Fault */
    SetIDTGate((void*)Entry_DoubleFault_Handler, 0x8, InterruptGateType, KernelRing, GDTInfoSelectorsRing[KernelRing].Code, idtr);

    /* Invalid TSS */
    SetIDTGate((void*)Entry_InvalidTSS_Handler, 0xA, InterruptGateType, KernelRing, GDTInfoSelectorsRing[KernelRing].Code, idtr);

    /* Segment Not Present */
    SetIDTGate((void*)Entry_SegmentNotPresent_Handler, 0xB, InterruptGateType, KernelRing, GDTInfoSelectorsRing[KernelRing].Code, idtr);

    /* Stack-Segment Fault */
    SetIDTGate((void*)Entry_StackSegmentFault_Handler, 0xC, InterruptGateType, KernelRing, GDTInfoSelectorsRing[KernelRing].Code, idtr);

    /* GP Fault */
    SetIDTGate((void*)Entry_GPFault_Handler, 0xD, InterruptGateType, KernelRing, GDTInfoSelectorsRing[KernelRing].Code, idtr);

    /* Page Fault */
    SetIDTGate((void*)Entry_PageFault_Handler, 0xE, InterruptGateType, KernelRing, GDTInfoSelectorsRing[KernelRing].Code, idtr);

    /* x87 Floating-Point Exception */
    SetIDTGate((void*)Entry_x87FloatingPointException_Handler, 0x10, InterruptGateType, KernelRing, GDTInfoSelectorsRing[KernelRing].Code, idtr);

    /* Alignment Check */
    SetIDTGate((void*)Entry_AlignmentCheck_Handler, 0x11, InterruptGateType, KernelRing, GDTInfoSelectorsRing[KernelRing].Code, idtr);

    /* Machine Check */
    SetIDTGate((void*)Entry_MachineCheck_Handler, 0x12, InterruptGateType, KernelRing, GDTInfoSelectorsRing[KernelRing].Code, idtr);

    /* SIMD Floating-Point Exception */
    SetIDTGate((void*)Entry_SIMDFloatingPointException_Handler, 0x13, InterruptGateType, KernelRing, GDTInfoSelectorsRing[KernelRing].Code, idtr);

    /* Virtualization Exception */
    SetIDTGate((void*)Entry_VirtualizationException_Handler, 0x14, InterruptGateType, KernelRing, GDTInfoSelectorsRing[KernelRing].Code, idtr);
    
    /* Security Exception */
    SetIDTGate((void*)Entry_SecurityException_Handler, 0x1E, InterruptGateType, KernelRing, GDTInfoSelectorsRing[KernelRing].Code, idtr);

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

    asm("lidt %0" : : "m" (idtr));     
}

extern "C" void DivideByZero_Handler(InterruptStack* Registers, uint64_t CoreID){
    Panic("Divide-by-zero Error Detected");
    globalLogs->Error("Divide-by-zero Error Detected -> At processor %u", CoreID);
    while(true);
}

extern "C" void Debug_Handler(InterruptStack* Registers, uint64_t CoreID){
    Panic("Debug Detected");
    globalLogs->Error("Debug Detected -> At processor %u", CoreID);
    while(true);
}

extern "C" void NMI_Handler(InterruptStack* Registers, uint64_t CoreID){
    Panic("Non-maskable Interrupt Detected");
    globalLogs->Error("Non-maskable Interrupt Detected -> At processor %u", CoreID);
    while(true);
}

extern "C" void Breakpoint_Handler(InterruptStack* Registers, uint64_t CoreID){
    Panic("Breakpoint Detected");
    globalLogs->Error("Breakpoint Detected -> At processor %u", CoreID);
    while(true);
}

extern "C" void Overflow_Handler(InterruptStack* Registers, uint64_t CoreID){
    Panic("Overflow Detected");
    globalLogs->Error("Overflow Detected -> At processor %u", CoreID);
    while(true);
}

extern "C" void BoundRangeExceeded_Handler(InterruptStack* Registers, uint64_t CoreID){
    Panic("Bound Range Exceeded Detected");
    globalLogs->Error("Bound Range Exceeded Detected -> At processor %u", CoreID);
    while(true);
}

extern "C" void InvalidOpcode_Handler(InterruptStack* Registers, uint64_t CoreID){
    Panic("Invalid Opcode Detected");
    globalLogs->Error("Invalid Opcode Detected -> At processor %u", CoreID);
    while(true);
}

extern "C" void DeviceNotAvailable_Handler(InterruptStack* Registers, uint64_t CoreID){
    Panic("Device Not Available Detected");
    globalLogs->Error("Device Not Available Detected -> At processor %u", CoreID);
    while(true);
}

extern "C" void DoubleFault_Handler(ErrorInterruptStack* Registers, uint64_t CoreID){
    Panic("Double Fault Detected");
    globalLogs->Error("Double Fault Detected -> At processor %u", CoreID);
    while(true);
}

extern "C" void InvalidTSS_Handler(ErrorInterruptStack* Registers, uint64_t CoreID){
    Panic("Invalid TSS Detected");
    globalLogs->Error("Invalid TSS Detected -> At processor %u", CoreID);
    while(true);
}

extern "C" void SegmentNotPresent_Handler(ErrorInterruptStack* Registers, uint64_t CoreID){
    Panic("Segment Not Present Detected");
    globalLogs->Error("Segment Not Present Detected -> At processor %u", CoreID);
    while(true);
}

extern "C" void StackSegmentFault_Handler(ErrorInterruptStack* Registers, uint64_t CoreID){
    Panic("Stack-Segment Fault Detected");
    globalLogs->Error("Stack-Segment Fault Detected -> At processor %u", CoreID);
    while(true);
}

extern "C" void GPFault_Handler(ErrorInterruptStack* Registers, uint64_t CoreID){
    Panic("General Protection Fault Detected");
    globalLogs->Error("General Protection Fault Detected -> At processor %u", CoreID);
    if(ReadBit((uint8_t)(uint64_t)Registers->errorCode, 0)){
        globalLogs->Message("The exception originated externally to the processor");
    }

    uint8_t Tbl = 0;
    Tbl |= ReadBit((uint8_t)(uint64_t)Registers->errorCode, 1);
    Tbl |= ReadBit((uint8_t)(uint64_t)Registers->errorCode, 2) << 1;
    switch (Tbl)
    {
    case 0:
        globalLogs->Message("Caused by gdt");
        break;
    case 1:
        globalLogs->Message("Caused by idt");
        break;
    case 2:
        globalLogs->Message("Caused by ldt");
        break;
    
    default:
        break;
    }
    while(true);
}

extern "C" void PageFault_Handler(ErrorInterruptStack* Registers, uint64_t CoreID, void* Address){
    globalLogs->Error("Page Fault Detected : Memory address : 0x%x | Processor ID : %u", Address, CoreID);
    Panic("Page Fault Detected");
    if(ReadBit((uint8_t)(uint64_t)Registers->errorCode, 0)){
        globalLogs->Message("Page-protection violation");
    }else{
        globalLogs->Message("Non-present page");
    }

    if(ReadBit((uint8_t)(uint64_t)Registers->errorCode, 1)){
        globalLogs->Message("Error caused by writting");
    }else{
        globalLogs->Message("Error caused by reading");
    }   

    if(ReadBit((uint8_t)(uint64_t)Registers->errorCode, 2)){
        globalLogs->Message("Non-user page");
    }

    if(ReadBit((uint8_t)(uint64_t)Registers->errorCode, 3)){
        globalLogs->Message("Page contain reserved bit");
    }

    if(ReadBit((uint8_t)(uint64_t)Registers->errorCode, 4)){
        globalLogs->Message("Caused by an instruction fetch");
    }

    if(ReadBit((uint8_t)(uint64_t)Registers->errorCode, 5)){
        globalLogs->Message("Protection-key violation");
    }

    if(ReadBit((uint8_t)(uint64_t)Registers->errorCode, 6)){
        globalLogs->Message("Caused by shadow stack access");
    }

    if(ReadBit((uint8_t)(uint64_t)Registers->errorCode, 7)){
        globalLogs->Message("Caused to an SGX violaton");
    }
    while(true);
}

extern "C" void x87FloatingPointException_Handler(InterruptStack* Registers, uint64_t CoreID){
    Panic("x87 Floating-Point Exception Detected");
    globalLogs->Error("x87 Floating-Point Exception Detected -> At processor %u", CoreID);
    while(true);
}

extern "C" void AlignmentCheck_Handler(ErrorInterruptStack* Registers, uint64_t CoreID){
    Panic("Alignment Check Detected");
    globalLogs->Error("Alignment Check Detected -> At processor %u", CoreID);
    while(true);
}

extern "C" void MachineCheck_Handler(InterruptStack* Registers, uint64_t CoreID){
    Panic("Machine Check Detected");
    globalLogs->Error("Machine Check Detected -> At processor %u", CoreID);
    while(true);
}

extern "C" void SIMDFloatingPointException_Handler(InterruptStack* Registers, uint64_t CoreID){
    Panic("SIMD Floating-Point Exception Detected");
    globalLogs->Error("SIMD Floating-Point Exception Detected -> At processor %u", CoreID);
    while(true);
}

extern "C" void VirtualizationException_Handler(InterruptStack* Registers, uint64_t CoreID){
    Panic("Virtualization Exception Detected");
    globalLogs->Error("Virtualization Exception Detected -> At processor %u", CoreID);
    while(true);
}

extern "C" void SecurityException_Handler(ErrorInterruptStack* Registers, uint64_t CoreID){
    Panic("Security Exception Detected");
    globalLogs->Error("Security Exception Detected -> At processor %u", CoreID);
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
    PIC_EndMaster();       
}

static uint64_t mutexScheduler;
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

    if(arg5 != NULL){
        globalLogs->Successful("%u %s", CoreID, arg5);
    }else{
        globalLogs->Error("%u", CoreID);
    }
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