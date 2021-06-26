#include "smp.h" 

extern uint64_t IdleTaskStart;
extern uint64_t IdleTaskEnd;

extern "C" void TrampolineMain(int CoreID){
    gdtInitCores(CoreID);
    asm ("lidt %0" : : "m" (idtr));
    
    APIC::EnableAPIC();
    APIC::StartLapicTimer();

    
    if(EnabledSSE() == 0){
        FPUInit();
    }

    asm("sti");

    globalTaskManager.EnabledScheduler(CoreID);

    while(true){
        asm("hlt");
    }
}

