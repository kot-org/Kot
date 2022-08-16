#include <arch/x86-64/interrupts/interrupts.h>

IDTR idtr;

uint8_t IDTData[PAGE_SIZE];

event_t* InterruptEventList[MAX_IRQ];

parameters_t* InterruptParameters;

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
    if(idtr.Limit == 0){
        idtr.Limit = 0xFFF;
        idtr.Offset = (uint64_t)&IDTData[0];
    }

    /* init interrupts */
    InterruptParameters = (parameters_t*)malloc(sizeof(parameters_t));

    for(int i = 0; i < ArchInfo->IRQSize; i++){
        SetIDTGate(InterruptEntryList[i], i, InterruptGateType, KernelRing, GDTInfoSelectorsRing[KernelRing].Code, IST_Interrupts, idtr);
        if(i != IPI_Schedule && i != IPI_Stop && i >= Exception_End){
            if(i >= ArchInfo->IRQLineStart && i <= ArchInfo->IRQLineStart + ArchInfo->IRQLineSize){
                Event::Create(&InterruptEventList[i], EventTypeIRQLines, i - ArchInfo->IRQLineStart);
            }else{
                Event::Create(&InterruptEventList[i], EventTypeIRQ, i);
            }
        }
    }

    /* Shedule */
    SetIDTGate((uintptr_t)InterruptEntryList[IPI_Schedule], IPI_Schedule, InterruptGateType, UserAppRing, GDTInfoSelectorsRing[KernelRing].Code, IST_Scheduler, idtr);

    uint64_t stack = (uint64_t)malloc(KERNEL_STACK_SIZE) + KERNEL_STACK_SIZE;
    TSSSetIST(CPU::GetAPICID(), IST_Interrupts, stack);
    uint64_t stackScheduler = (uint64_t)malloc(KERNEL_STACK_SIZE) + KERNEL_STACK_SIZE;
    TSSSetIST(CPU::GetAPICID(), IST_Scheduler, stackScheduler);

    asm("lidt %0" : : "m" (idtr));   
}

extern "C" void InterruptHandler(ContextStack* Registers, uint64_t CoreID){
    // bug : styack corruptiom because it's schedule with exeception stack create ist
    if(Registers->InterruptNumber < Exception_End){
        // exceptions
        globalTaskManager->IsSchedulerEnable[CoreID] = false;
        ExceptionHandler(Registers, CoreID);
        globalTaskManager->IsSchedulerEnable[CoreID] = true;
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
        globalTaskManager->IsSchedulerEnable[CoreID] = false;
        InterruptParameters->Arg0 = Registers->InterruptNumber;
        Event::Trigger((kthread_t*)0x0, InterruptEventList[Registers->InterruptNumber], InterruptParameters);
        globalTaskManager->IsSchedulerEnable[CoreID] = true;
    }
    APIC::localApicEOI(CoreID);
}

void ExceptionHandler(ContextStack* Registers, uint64_t CoreID){
    // If exception come from kernel we can't recover it
    globalTaskManager->IsSchedulerEnable[CoreID] = false;
    if(CPU::GetCodeRing(Registers) == KernelRing){ 
        KernelUnrecovorable(Registers, CoreID);
    }else{
        // Try to recover exception
        if(Registers->InterruptNumber == Exception_PageFault){
            if(PageFaultHandler(Registers, CoreID)){
                return;
            }
        }

        Error("Thread error, PID : %x | TID : %x \nWith exception : '%s' | Error code : %x", Registers->threadInfo->thread->Parent->PID, Registers->threadInfo->thread->TID, ExceptionList[Registers->InterruptNumber], Registers->ErrorCode);
        PrintRegisters(Registers);
        globalTaskManager->Exit(Registers, CoreID, Registers->threadInfo->thread); 
        globalTaskManager->IsSchedulerEnable[CoreID] = true;
        globalTaskManager->Scheduler(Registers, CoreID); 
    }
}

bool PageFaultHandler(ContextStack* Registers, uint64_t CoreID){
    if(globalTaskManager->threadExecutePerCore[CoreID] != NULL){
        uint64_t Address = 0;
        asm("movq %%cr2, %0" : "=r"(Address));
        return globalTaskManager->threadExecutePerCore[CoreID]->ExtendStack((uint64_t)Address);
    }
    return false;
}

void KernelUnrecovorable(ContextStack* Registers, uint64_t CoreID){
    Error("Kernel Panic CPU %x \nWith exception : '%s' | Error code : %x", CoreID, ExceptionList[Registers->InterruptNumber], Registers->ErrorCode);
    PrintRegisters(Registers);
    KernelPanic("Unrecoverable exception ;(");
}
