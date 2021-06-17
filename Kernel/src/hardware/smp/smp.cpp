#include "smp.h" 

extern "C" void TrampolineMain(int cpuID){
    printf("I am %u\n", cpuID);
    globalGraphics->Update();
    gdtInitCores(cpuID);
    uint64_t lapicAddress = (uint64_t)APIC::GetLAPICAddress();
    *((volatile uint32_t*)(lapicAddress + 0xf0)) = *((volatile uint32_t*)(lapicAddress + 0xf0)) | 0x1ff;
	*((volatile uint32_t*)(lapicAddress + 0x320)) = 32 | 0x20000;
	*((volatile uint32_t*)(lapicAddress + 0x3e0)) = 0x3;
	*((volatile uint32_t*)(lapicAddress + 0x380)) = 512;
    asm ("lidt %0" : : "m" (idtr)); 
    asm("sti");
    while(true){
        asm("hlt");
    }
}

