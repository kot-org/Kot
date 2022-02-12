#include "interrupts.h"

uint64_t SetIrq(process_t* parent, void* entryPoint, uint8_t irq){
    // if(irq < IRQ_MAX){
    //     IRQRedirectList[irq] = parent->CreatThread((uint64_t)entryPoint, parent->DefaultPriviledge, 0);
    //     APIC::IoChangeIrqState(irq, 0, true);
    //     return 1;
    // }else{
    //     return 0;
    // }
}
 
uint64_t SetIrqDefault(uint8_t irq){
    // if(irq < IRQ_MAX){
    //     APIC::IoChangeIrqState(irq, 0, false);
    //     void* IRQTaskAddress = (void*)IRQRedirectList[irq];
    //     IRQRedirectList[irq] = NULL;
    //     free(IRQTaskAddress);
    //     return 1;
    // }else{
    //     return 0;
    // }
}

void RedirectIRQ(InterruptStack* Registers, uint64_t CoreID, uint8_t irq){
    // globalTaskManager->SwitchTask(Registers, CoreID, IRQRedirectList[irq]);
}