#include "smp.h"

void LoadCores(){
    uint64_t lapicAddress = msr::rdmsr(0x1b) & 0xfffff000;
    globalPageTableManager.MapMemory((void*)lapicAddress, (void*)lapicAddress);
    globalPageTableManager.MapMemory((void*)0x8000, (void*)0x8000);

    uint8_t bspid = 0; 
    __asm__ __volatile__ ("mov $1, %%rax; cpuid; shrq $24, %%rbx;": "=r"(bspid)::);

    memcpy((void*)0x8000, (void*)&Trampoline, 0x1000);

    trampolineData* Data = (trampolineData*) (((uint64_t) &DataTrampoline - (uint64_t) &Trampoline) + 0x8000);
    Data->MainEntry = (uint64_t)&TrampolineMain;
    for(int i = 1; i < APIC::ProcessorCount; i++){    
        Data->Stack = (uint64_t)globalAllocator.RequestPage();     
        if(APIC::Processor[i]->APICID == bspid) continue; 
        
        //init IPI
        *((volatile uint32_t*)(lapicAddress + 0x280)) = 0;
		*((volatile uint32_t*)(lapicAddress + 0x310)) = i << 24;
		*((volatile uint32_t*)(lapicAddress + 0x300)) = 0x00C500;

		do { __asm__ __volatile__ ("pause" : : : "memory"); }while(*((volatile uint32_t*)(lapicAddress + 0x300)) & (1 << 12));

		*((volatile uint32_t*)(lapicAddress + 0x310)) = i << 24;
		*((volatile uint32_t*)(lapicAddress + 0x300)) = 0x008500;

		do { __asm__ __volatile__ ("pause" : : : "memory"); }while(*((volatile uint32_t*)(lapicAddress + 0x300)) & (1 << 12));
        PIT::Sleep(10);

        for(int j = 0; j < 2; j++) {
            *((volatile uint32_t*)(lapicAddress + 0x280)) = 0;
            *((volatile uint32_t*)(lapicAddress + 0x310)) = i << 24;
            PIT::Sleep(1);
            *((volatile uint32_t*)(lapicAddress + 0x300)) = 0x000608;
        }

		do { __asm__ __volatile__ ("pause" : : : "memory"); }while(*((volatile uint32_t*)(lapicAddress + 0x300)) & (1 << 12));
        
        printf("Waiting cpu\n");
        globalGraphics->Update(); 
        while (Data->Status == 0); // wait processor
        printf("cpu respond with : %u \n", Data->Status);
        globalGraphics->Update(); 
        while (Data->Status != 3); // wait processor
        printf("Core %u init\n", i);
        globalGraphics->Update();   
    }
}   

extern "C" void TrampolineMain(int cpuID){
    printf("I am %u\n", cpuID);
    globalGraphics->Update();
    gdtInitCores(cpuID);
    InitializeInterrupts();
    IoWrite8(PIC1_DATA, 0b11111000);
    IoWrite8(PIC2_DATA, 0b11101111);
    asm("sti");
    while(true){
        asm("hlt");
    }
}

