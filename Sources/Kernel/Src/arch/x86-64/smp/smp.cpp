#include <arch/x86-64/smp/smp.h>

extern "C" void TrampolineMain(){
    uint64_t CoreID = CPU::GetAPICID();

    gdtInitCores(CoreID);

    TSSSetIST(CPU::GetAPICID(), IST_Scheduler, DataTrampoline.StackScheduler);
    asm ("lidt %0" : : "m" (idtr));

    uint64_t stackSchedulerAPIC = (uint64_t)malloc(KERNEL_STACK_SIZE) + KERNEL_STACK_SIZE;
    TSSSetIST(CPU::GetAPICID(), IST_SchedulerAPIC, stackSchedulerAPIC);
    uint64_t stackScheduler = (uint64_t)malloc(KERNEL_STACK_SIZE) + KERNEL_STACK_SIZE;
    TSSSetIST(CPU::GetAPICID(), IST_Scheduler, stackScheduler);
    uint64_t stackDestroySelf = (uint64_t)malloc(KERNEL_STACK_SIZE) + KERNEL_STACK_SIZE;
    TSSSetIST(CPU::GetAPICID(), IST_DestroySelf, stackDestroySelf);

    CPU::InitCore();
    simdInit();

    APIC::EnableAPIC(CoreID);
    APIC::StartLapicTimer();


    DataTrampoline.Status = 0xef;

    globalTaskManager->EnabledScheduler(CoreID);
    
    while (DataTrampoline.Status != 0xff){
        __asm__ __volatile__ ("pause" : : : "memory");
    }

    LaunchUserSpace();

    while(true){
        asm("hlt");
    }
}

