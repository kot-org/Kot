#include "smp.h" 

extern uint64_t IdleTaskStart;
extern uint64_t IdleTaskEnd;

static void* mutex;

extern "C" void TrampolineMain(int CoreID){
    mutex = Atomic::atomicLoker((void*)mutex);
    gdtInitCores(CoreID);
    asm ("lidt %0" : : "m" (idtr));
    
    APIC::EnableAPIC();
    APIC::localApicEOI();
    APIC::StartLapicTimer();

    
    if(EnabledSSE() == 0){
        FPUInit();
    }

    Atomic::atomicUnlock((void*)mutex);
    asm("sti");

    globalTaskManager.EnabledScheduler(CoreID);

    while(true){
        asm("hlt");
    }
}

