#include "smp.h" 

extern uint64_t IdleTaskStart;
extern uint64_t IdleTaskEnd;

static uint64_t mutexSMP;

extern "C" void TrampolineMain(int CoreID){
    Atomic::atomicSpinlock(&mutexSMP, 0);
    Atomic::atomicLock(&mutexSMP, 0);
    gdtInitCores(CoreID);
    asm ("lidt %0" : : "m" (idtr));
    
    APIC::EnableAPIC();
    APIC::StartLapicTimer();

    
    if(EnabledSSE() == 0){
        FPUInit();
    }
    
    globalTaskManager.EnabledScheduler(CoreID);
    Atomic::atomicUnlock(&mutexSMP, 0);
    asm("sti");

    while(true){
        asm("hlt");
    }
}

