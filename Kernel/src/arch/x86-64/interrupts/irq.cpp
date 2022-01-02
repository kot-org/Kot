#include "interrupts.h"

uint64_t SetIrq(Task* parent, void* address, uint8_t irq){
    if(irq < IRQ_MAX){
        IRQRedirectList[irq] = (Task*)malloc(sizeof(Task));
        memcpy(IRQRedirectList[irq], parent, sizeof(Task));
        IRQRedirectList[irq]->EntryPoint = address;
        IRQRedirectList[irq]->Regs = (ContextStack*)calloc(sizeof(ContextStack));
        IRQRedirectList[irq]->Stack = IRQRedirectList[irq]->heap->malloc(0x100000);
        IRQRedirectList[irq]->Regs->cs = parent->Regs->cs;
        IRQRedirectList[irq]->Regs->ss = parent->Regs->ss;
        IRQRedirectList[irq]->Regs->rflags = parent->Regs->rflags;
        IRQRedirectList[irq]->Regs->rsp = (void*)((uint64_t)IRQRedirectList[irq]->Stack + 0x100000);
        IRQRedirectList[irq]->Regs->rip = address;
        IRQRedirectList[irq]->InterruptTask = true;
        IRQRedirectList[irq]->Parent = parent; 
        APIC::IoChangeIrqState(irq, 0, true);
        return 1;
    }else{
        return 0;
    }
}

uint64_t SetIrqDefault(uint8_t irq){
    if(irq < IRQ_MAX){
        APIC::IoChangeIrqState(irq, 0, false);
        void* IRQTaskAddress = (void*)IRQRedirectList[irq];
        IRQRedirectList[irq] = NULL;
        free(IRQTaskAddress);
        return 1;
    }else{
        return 0;
    }
}

void RedirectIRQ(InterruptStack* Registers, uint8_t CoreID, Task* parent){
    if(parent->InterruptTask){
        Task* task = globalTaskManager->DuplicateTask(parent);
        globalTaskManager->SwitchTask(Registers, CoreID, task);
    }
}