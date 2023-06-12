#include <arch/x86-64/interrupts/interrupts.h>

IDTR idtr;

uint8_t IDTData[PAGE_SIZE];

kevent_t* InterruptEventList[MAX_IRQ];

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

void InitializeInterrupts(ArchInfo_t* ArchInfo){
    idtr.Limit = 0xFFF;
    idtr.Offset = (uint64_t)&IDTData[0];

    for(int i = 0; i < ArchInfo->IRQSize; i++){
        SetIDTGate(InterruptEntryList[i], i, InterruptGateType, KernelRing, GDTInfoSelectorsRing[KernelRing].Code, IST_Interrupts, idtr);
        if(i != INT_Schedule && i != INT_Stop && i >= Exception_End){
            if(i >= ArchInfo->IRQLineStart && i <= ArchInfo->IRQLineStart + ArchInfo->IRQLineSize){
                Event::Create(&InterruptEventList[i], EventTypeIRQLines, i - ArchInfo->IRQLineStart);
            }else{
                Event::Create(&InterruptEventList[i], EventTypeIRQ, i);
            }
        }
    }

    /* Shedule */
    SetIDTGate((void*)InterruptEntryList[INT_ScheduleAPIC], INT_ScheduleAPIC, InterruptGateType, KernelRing, GDTInfoSelectorsRing[KernelRing].Code, IST_SchedulerAPIC, idtr);
    SetIDTGate((void*)InterruptEntryList[INT_Schedule], INT_Schedule, InterruptGateType, UserAppRing, GDTInfoSelectorsRing[KernelRing].Code, IST_Scheduler, idtr);
    SetIDTGate((void*)InterruptEntryList[INT_DestroySelf], INT_DestroySelf, InterruptGateType, KernelRing, GDTInfoSelectorsRing[KernelRing].Code, IST_DestroySelf, idtr); // Interrupt gate type because interrupt should be disable before

    uint64_t stackSchedulerAPIC = (uint64_t)stackalloc(KERNEL_STACK_SIZE) + KERNEL_STACK_SIZE;
    TSSSetIST(CPU::GetAPICID(), IST_SchedulerAPIC, stackSchedulerAPIC);
    uint64_t stackScheduler = (uint64_t)stackalloc(KERNEL_STACK_SIZE) + KERNEL_STACK_SIZE;
    TSSSetIST(CPU::GetAPICID(), IST_Scheduler, stackScheduler);
    uint64_t stackDestroySelf = (uint64_t)stackalloc(KERNEL_STACK_SIZE) + KERNEL_STACK_SIZE;
    TSSSetIST(CPU::GetAPICID(), IST_DestroySelf, stackDestroySelf);

    asm("lidt %0" : : "m" (idtr));   
}

extern "C" void InterruptHandler(ContextStack* Registers, uint64_t CoreID){
    if(Registers->InterruptNumber < Exception_End){
        // Exceptions
        uint64_t Cr2 = 0;
        asm("movq %%cr2, %0" : "=r"(Cr2));
        ExceptionHandler(Cr2, Registers, CoreID);
    }else if(Registers->InterruptNumber == INT_ScheduleAPIC){
        // APIC timer 
        globalTaskManager->Scheduler(Registers, CoreID); 
    }else if(Registers->InterruptNumber == INT_Schedule){
        // Scheduler
        globalTaskManager->Scheduler(Registers, CoreID); 
    }else if(Registers->InterruptNumber == INT_DestroySelf){
        // Destroy
        globalTaskManager->DestroySelf(Registers, CoreID);
    }else if(Registers->InterruptNumber == INT_Stop){
        // Stop all
        while(true){
            asm("hlt");
        }
    }else{
        // Other IRQ & IVT
        Event::TriggerIRQ(InterruptEventList[Registers->InterruptNumber]);
    }
    APIC::localApicEOI(CoreID);
}

void ExceptionHandler(uint64_t Cr2, ContextStack* Registers, uint64_t CoreID){
    // Try to recover exception
    if(Registers->InterruptNumber == Exception_PageFault){
        if(PageFaultHandler(Cr2, Registers, CoreID)){
            return;
        }
        Error("Page fault at 0x%x | Physical address detected in this page table is 0x%x", Cr2, vmm_GetPhysical((void*)Cr2));
    }
    // If exception come from kernel we can't recover it
    if(CPU::GetCodeRing(Registers) == KernelRing){
        KernelUnrecovorable(Cr2, Registers, CoreID);
    }else{
        Error("Thread error, PID : 0x%x | PPID : 0x%x | TID : 0x%x \nWith exception : '%s' | Error code : 0x%x", Registers->threadInfo->thread->Parent->PID, Registers->threadInfo->thread->Parent->PPID, Registers->threadInfo->thread->TID, ExceptionList[Registers->InterruptNumber], Registers->ErrorCode);
        PrintRegisters(Registers);
        StackFrame_t* Frame = (StackFrame_t*)Registers->rbp;
        TraceBegin();
        while(Frame){
            if(!Frame->InstructionPointer) break;
            Trace("0x%x", Frame->InstructionPointer);
            Frame = Frame->Next;
        }
        TraceEnd();
        if(Registers->threadInfo->thread->IsEvent){
            Event::Close(Registers, Registers->threadInfo->thread);
        }else{
            Registers->threadInfo->thread->Close(Registers, NULL); 
        }
    }
}

bool PageFaultHandler(uint64_t Cr2, ContextStack* Registers, uint64_t CoreID){
    if(Registers->threadInfo != NULL){
        return Registers->threadInfo->thread->PageFaultHandler(((Registers->ErrorCode & (1 << 1)) >> 1), (uint64_t)Cr2);
    }
    return false;
}

void KernelUnrecovorable(uint64_t Cr2, ContextStack* Registers, uint64_t CoreID){
    Error("Kernel Panic CPU 0x%x \nWith exception : '%s' | Error code : 0x%x", CoreID, ExceptionList[Registers->InterruptNumber], Registers->ErrorCode);
    PrintRegisters(Registers);
    KernelPanic("Unrecoverable exception ;(");
}
