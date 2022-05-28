#include <arch/x86-64/interrupts/interrupts.h>

IDTR idtr;

uint8_t IDTData[PAGE_SIZE];

event_t* InterruptEventList[256];

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
        idtr.Limit = 0xFFF;
        idtr.Offset = (uint64_t)&IDTData[0];
    }

    /* init interrupt */
    for(int i = 0; i < 256; i++){
        SetIDTGate(InterruptEntryList[i], i, InterruptGateType, KernelRing, GDTInfoSelectorsRing[KernelRing].Code, IST_Null, idtr);
        if(i >= IRQ_START && i <= IRQ_START + IRQ_MAX){
            Event::Creat(&InterruptEventList[i], EventTypeIRQ, i);
        }else{
            Event::Creat(&InterruptEventList[i], EventTypeIVT, i);
        }
    }

    // SetIDTGate((uintptr_t)0xfffffffff, 8, InterruptGateType, KernelRing, 0x0, IST_Null, idtr);
    // SetIDTGate((uintptr_t)0xfffffffff, 13, InterruptGateType, KernelRing, 0x0, IST_Null, idtr);
    /* Shedule */
    SetIDTGate((uintptr_t)InterruptEntryList[IPI_Schedule], IPI_Schedule, InterruptGateType, UserAppRing, GDTInfoSelectorsRing[KernelRing].Code, IST_Scheduler, idtr);

    asm("lidt %0" : : "m" (idtr));     
}

extern "C" void InterruptHandler(ContextStack* Registers, uint64_t CoreID){
    if(Registers->InterruptNumber < 32){
        // execptions
        ExceptionHandler(Registers, CoreID);
    }else if(Registers->InterruptNumber == IPI_Schedule){
        // APIC timer 
        globalTaskManager->Scheduler(Registers, CoreID); 
    }else if(Registers->InterruptNumber == IPI_Stop){
        // Stop all
        while(true){
            asm("hlt");
        }
    }else{
        // Other IRQ & IVT
        Event::Trigger((thread_t*)0x0, InterruptEventList[Registers->InterruptNumber], 0, 0);        
    }

    APIC::localApicEOI(CoreID);
}

void ExceptionHandler(ContextStack* Registers, uint64_t CoreID){
    // If exception come from kernel we can't recover it

    if(CPU::GetCodeRing(Registers) == KernelRing){ 
        KernelUnrecovorable(Registers, CoreID);
    }else{
        // Try to recover exception
        if(Registers->InterruptNumber == Exception_PageFault){
            if(PageFaultHandler(Registers, CoreID)){
                return;
            }
        }

        globalLogs->Error("Thread error, PID : %x | TID : %x \nWith execption : '%s' | Error code : %x", Registers->ThreadInfo->Thread->Parent->PID, Registers->ThreadInfo->Thread->TID, ExceptionList[Registers->InterruptNumber], Registers->ErrorCode);        
        Registers->ThreadInfo->Thread->Exit(Registers, CoreID); 
        globalTaskManager->Scheduler(Registers, CoreID); 
    }

}

bool PageFaultHandler(ContextStack* Registers, uint64_t CoreID){
    if(globalTaskManager->IsSchedulerEnable[CoreID] && globalTaskManager->ThreadExecutePerCore[CoreID] != NULL){
        uint64_t Address = 0;
        asm("movq %%cr2, %0" : "=r"(Address));
        return globalTaskManager->ThreadExecutePerCore[CoreID]->ExtendStack((uint64_t)Address);
    }
    return false;
}

void KernelUnrecovorable(ContextStack* Registers, uint64_t CoreID){
    globalLogs->Error("Kernel Panic CPU %x \nWith execption : '%s' | Error code : %x", CoreID, ExceptionList[Registers->InterruptNumber], Registers->ErrorCode);
    KernelPanic("Unrecovorable exception ;(");
}
