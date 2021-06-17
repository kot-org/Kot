#include "smp.h" 

extern "C" void TrampolineMain(int cpuID){
    printf("I am %u\n", cpuID);
    globalGraphics->Update();
    gdtInitCores(cpuID);
    asm ("lidt %0" : : "m" (idtr));
    APIC::localApicEOI();
    APIC::StartLapicTimer();
    asm("sti");
    while(true){
        asm("hlt");
    }
}

