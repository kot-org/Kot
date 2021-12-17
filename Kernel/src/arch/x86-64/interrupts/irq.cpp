#include "interrupts.h"

void* defaultStack = 0;

void DefaultIRQHandler(){
    globalLogs->Warning("Your IRQ was ignored with success because kernel doesn't like you :=)");
}

uint64_t SetIrq(uint8_t ring, PageTableManager* pageTable, uint8_t irq, void* address){
    if(irq < IRQ_MAX){
        IRQRedirectList[irq].stack = Memory::CreatStack(pageTable, 100, false);
        IRQRedirectList[irq].cr3 = pageTable->PML4;
        IRQRedirectList[irq].functionAddress = address;
        return 1;
    }else{
        return 0;
    }
}

uint64_t SetIrqDefault(uint8_t irq){
    if(irq < IRQ_MAX){
        if(defaultStack == NULL) defaultStack = (void*)((uint64_t)malloc(0x100000) + 0x100000);
        IRQRedirectList[irq].stack = defaultStack;
        IRQRedirectList[irq].cr3 = globalPageTableManager.PML4;
        IRQRedirectList[irq].functionAddress = (void*)DefaultIRQHandler;
        return 1;
    }else{
        return 0;
    }
}