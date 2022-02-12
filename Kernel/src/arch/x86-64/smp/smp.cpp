#include "smp.h" 

extern uint64_t IdleTaskStart;
extern uint64_t IdleTaskEnd;

static uint64_t mutexSMP;
uint64_t StatusProcessor;

extern "C" void TrampolineMain(){
    Atomic::atomicSpinlock(&mutexSMP, 0);
    Atomic::atomicLock(&mutexSMP, 0);

    CPU::InitCPU();
    uint64_t CoreID = CPU::GetCoreID();

    //Creat new paging for each cpu
    void* PML4 = globalAllocator.RequestPage();
    memset(globalPageTableManager[0].GetVirtualAddress(PML4), 0, 0x1000);
    globalPageTableManager[CoreID].PageTableManagerInit((PageTable*)PML4);
    globalPageTableManager[CoreID].PhysicalMemoryVirtualAddressSaver = globalPageTableManager[0].PhysicalMemoryVirtualAddressSaver;
    globalPageTableManager[CoreID].PhysicalMemoryVirtualAddress = globalPageTableManager[0].PhysicalMemoryVirtualAddress;
    globalPageTableManager[CoreID].VirtualAddress = globalPageTableManager[0].VirtualAddress;
    globalPageTableManager[CoreID].CopyHigherHalf(&globalPageTableManager[0]);
    globalPageTableManager[CoreID].PhysicalMemoryVirtualAddress = globalPageTableManager[0].PhysicalMemoryVirtualAddress;
    globalPageTableManager[CoreID].ChangePaging(&globalPageTableManager[CoreID]);

    gdtInitCores(CoreID);
    asm ("lidt %0" : : "m" (idtr));
    
    APIC::EnableAPIC(CoreID);
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

