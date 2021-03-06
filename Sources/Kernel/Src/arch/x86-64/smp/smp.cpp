#include <arch/x86-64/smp/smp.h>

extern "C" void TrampolineMain(){
    uint64_t CoreID = CPU::GetAPICID();

    gdtInitCores(CoreID);

    asm ("lidt %0" : : "m" (idtr));
 
    CPU::InitCPU();
    simdInit();

    APIC::EnableAPIC(CoreID);
    APIC::StartLapicTimer();


    DataTrampoline.Status = 0xef;

    while (DataTrampoline.Status != 0xff){
        __asm__ __volatile__ ("pause" : : : "memory");
    }

    globalTaskManager->EnabledScheduler(CoreID);

    LaunchUserSpace();

    while(true){
        asm("hlt");
    }
}

