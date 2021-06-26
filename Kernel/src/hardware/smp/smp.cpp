#include "smp.h" 

extern uint64_t IdleTaskStart;
extern uint64_t IdleTaskEnd;

extern "C" void TrampolineMain(int CoreID){
    gdtInitCores(CoreID);
    asm ("lidt %0" : : "m" (idtr));
    
    //APIC::StartLapicTimer();
    /*printf("I am %u\n", CoreID);
    globalGraphics->Update();*/
    
    if(EnabledSSE() == 0){
        FPUInit();
    }

    asm("sti");

    //void* NewLocationIdleTask = globalAllocator.RequestPage();
    //memcpy(NewLocationIdleTask, (void*)IdleTask, IdleTaskEnd - IdleTaskStart);
    globalTaskManager.EnabledScheduler(CoreID);

    while(true){
        asm("hlt");
    }
}

