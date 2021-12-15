#include "smp.h" 

extern uint64_t IdleTaskStart;
extern uint64_t IdleTaskEnd;

static uint64_t mutexSMP;
uint64_t StatusProcessor;

extern "C" void TrampolineMain(){
    Atomic::atomicSpinlock(&mutexSMP, 0);
    Atomic::atomicLock(&mutexSMP, 0);
    SaveCoreID();
    uint8_t CoreID = GetCoreID();
    gdtInitCores(CoreID);
    asm ("lidt %0" : : "m" (idtr));
    
    APIC::EnableAPIC();
    APIC::StartLapicTimer();

    
    if(EnabledSSE() == 0){
        FPUInit();
    }

    //End processor init
    StatusProcessor = 4;

    globalTaskManager->EnabledScheduler(CoreID);
    Atomic::atomicUnlock(&mutexSMP, 0);
    asm("sti");

    while(true){
        asm("hlt");
    }
}

