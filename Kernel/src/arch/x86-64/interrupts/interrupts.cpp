#include <arch/x86-64/interrupts/interrupts.h>

IDTR idtr;

uint8_t IDTData[0x1000];

char* ExceptionList[32] = {
    "DivisionByZero",
    "Debug",
    "Non Maskable Interrupt",
    "Breakpoint",
    "Overflow",
    "Bound Range Exceeded",
    "Invalid Opcode",
    "Device Not Available",
    "Double Fault",
    "Coprocessor Segment Overrun",
    "Invalid TSS",
    "Segment Not Present",
    "Stack Fault",
    "General Protection Fault",
    "Page Fault",
    "Reserved",
    "x87 Floating-Point Exception",
    "Alignment Check",
    "Machine Check",
    "SIMD Floating-Point Exception",
    "Virtualization Exception",
    "Control Protection Exception",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Hypervisor Injection Exception",
    "VMM Communication Exception",
    "Security Exception",
};

void InitializeInterrupts(){
    if(idtr.Limit == 0){
        idtr.Limit = 0x0FFF;
        idtr.Offset = (uint64_t)&IDTData[0];
    }

    /* init interrupt */
    for(int i = 0; i < 256; i++){
        SetIDTGate(InterruptEntryList[i], i, InterruptGateType, KernelRing, GDTInfoSelectorsRing[KernelRing].Code, idtr);
    }

    /* Syscall */
    SetIDTGate((void*)InterruptEntryList[0x80], 0x80, InterruptGateType, UserAppRing, GDTInfoSelectorsRing[KernelRing].Code, idtr);

    /* Sheduler call by app */
    SetIDTGate((void*)InterruptEntryList[0x81], 0x81, InterruptGateType, UserAppRing, GDTInfoSelectorsRing[KernelRing].Code, idtr);

    asm("lidt %0" : : "m" (idtr));     
}

extern "C" void InterruptHandler(InterruptStack* Registers, uint64_t CoreID){
    if(Registers->InterruptNumber < 32){
        // execptions
        ExceptionHandler(Registers, CoreID);
    }else if(Registers->InterruptNumber >= IRQ_START && Registers->InterruptNumber <= IRQ_START + IRQ_MAX){
        // IRQ
        
    }else if(Registers->InterruptNumber == 0x40){
        // APIC timer 
        globalTaskManager->Scheduler(Registers, CoreID); 
    }else if(Registers->InterruptNumber == 0x80){
        SyscallHandler(Registers, CoreID);
    }else if(Registers->InterruptNumber == 0x81){
        // schedule
        globalTaskManager->Scheduler(Registers, CoreID); 
    }
    APIC::localApicEOI(CoreID);

}

void ExceptionHandler(InterruptStack* Registers, uint64_t CoreID){
    // If exception come from kernel we can't recover it

    if(GetCodeRing(Registers) == KernelRing){ 
        KernelUnrecovorable(Registers, CoreID);
    }else{
        //try to recover exception
        if(Registers->InterruptNumber == Exception_PageFault){
            if(PageFaultHandler(Registers, CoreID)){
                return;
            }
        }  
        globalTaskManager->ThreadExecutePerCore[CoreID]->Exit(Registers, CoreID);
        globalTaskManager->Scheduler(Registers, CoreID); 
        globalLogs->Error("App Panic CPU %x", CoreID);
        globalLogs->Error("With execption : '%s' Error code : %x", ExceptionList[Registers->InterruptNumber], Registers->ErrorCode);
    }

}



uint8_t GetCodeRing(InterruptStack* Registers){
    return (Registers->cs & 0b11);
}

bool PageFaultHandler(InterruptStack* Registers, uint64_t CoreID){
    if(globalTaskManager->IsSchedulerEnable[CoreID] && globalTaskManager->ThreadExecutePerCore[CoreID] != NULL){
        uint64_t Address = 0;
        asm("movq %%cr2, %0" : "=r"(Address));
        return globalTaskManager->ThreadExecutePerCore[CoreID]->ExtendStack((uint64_t)Address);
    }
    return false;
}

void KernelUnrecovorable(InterruptStack* Registers, uint64_t CoreID){
    globalLogs->Error("Kernel Panic CPU %x", CoreID);
    globalLogs->Error("With execption : '%s' Error code : %x", ExceptionList[Registers->InterruptNumber], Registers->ErrorCode);

    while(true){
        asm("hlt");
    }
}
