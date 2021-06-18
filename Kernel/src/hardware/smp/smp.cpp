#include "smp.h" 

extern "C" void TrampolineMain(int cpuID){
    gdtInitCores(cpuID);
    asm ("lidt %0" : : "m" (idtr));
    
    APIC::localApicEOI();
    APIC::StartLapicTimer();
    printf("I am %u\n", cpuID);
    globalGraphics->Update();
    asm("sti");
    while(true){
        asm("hlt");
    }
}

