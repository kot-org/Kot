#include "kernelInit.h"

PageTableManager pageTableManager = NULL;
graphics r = graphics(NULL);
CPU cpu;

void InitializeMemory(BootInfo* bootInfo){

    uint64_t mMapEntries = bootInfo->mMapSize / bootInfo->mMapDescSize;

    globalAllocator = PageFrameAllocator();

    globalAllocator.ReadEFIMemoryMap(bootInfo->mMap, bootInfo->mMapSize, bootInfo->mMapDescSize);

    uint64_t kernelPages = (uint64_t)bootInfo->KernelSize / 0x1000 + 1;

    globalAllocator.LockPages(bootInfo->KernelStart, kernelPages);

    PageTable* PML4 = (PageTable*)globalAllocator.RequestPage();
    memset(PML4, 0, 0x1000);

    globalPageTableManager = PageTableManager(PML4);

    for (uint64_t t = 0; t < GetMemorySize(bootInfo->mMap, mMapEntries, bootInfo->mMapDescSize); t+= 0x1000){
        globalPageTableManager.MapMemory((void*)t, (void*)t);
    }

    uint64_t fbBase = (uint64_t)bootInfo->framebuffer->BaseAddress;
    uint64_t fbSize = (uint64_t)bootInfo->framebuffer->FrameBufferSize + 0x1000;
    globalAllocator.LockPages((void*)fbBase, fbSize / 0x1000 + 1);
    for (uint64_t t = fbBase; t < fbBase + fbSize; t += 4096){
        globalPageTableManager.MapMemory((void*)t, (void*)t);
    }

    DataTrampoline.Paging = (uint64_t)PML4;
    asm ("mov %0, %%cr3" :: "r" (PML4));
}

void InitializeACPI(BootInfo* bootInfo){
    ACPI::SDTHeader* xsdt = (ACPI::SDTHeader*)(bootInfo->rsdp->XSDTAddress);

    ACPI::MCFGHeader* mcfg = (ACPI::MCFGHeader*)ACPI::FindTable(xsdt, (char*)"MCFG");
    PCI::EnumeratePCI(mcfg);

    ACPI::MADTHeader* madt = (ACPI::MADTHeader*)ACPI::FindTable(xsdt, (char*)"APIC");
    APIC::InitializeMADT(madt);
}

void LoadCores(){
    uint64_t lapicAddress = msr::rdmsr(0x1b) & 0xfffff000;
    globalGraphics->Update(); 
    globalPageTableManager.MapMemory((void*)lapicAddress, (void*)lapicAddress);
    globalPageTableManager.MapMemory((void*)0x8000, (void*)0x8000);

    uint8_t bspid = 0; 
    __asm__ __volatile__ ("mov $1, %%rax; cpuid; shrq $24, %%rbx;": "=r"(bspid)::);

    memcpy((void*)0x8000, (void*)&Trampoline, 0x1000);

    for(int i = 1; i < APIC::ProcessorCount; i++){    
        trampolineData* data = (trampolineData*) (((uint64_t) &DataTrampoline - (uint64_t) &Trampoline) + 0x8000);
        DataTrampoline.Stack = (uint64_t)globalAllocator.RequestPage();     
        if(APIC::Processor[i]->APICID == bspid) continue; 
        
        //init IPI
        *((volatile uint32_t*)(lapicAddress + 0x280)) = 0;
		*((volatile uint32_t*)(lapicAddress + 0x310)) = i << 24;
		*((volatile uint32_t*)(lapicAddress + 0x300)) = 0x00C500;

		do { __asm__ __volatile__ ("pause" : : : "memory"); }while(*((volatile uint32_t*)(lapicAddress + 0x300)) & (1 << 12));

		*((volatile uint32_t*)(lapicAddress + 0x310)) = i << 24;
		*((volatile uint32_t*)(lapicAddress + 0x300)) = 0x008500;

		do { __asm__ __volatile__ ("pause" : : : "memory"); }while(*((volatile uint32_t*)(lapicAddress + 0x300)) & (1 << 12));
        asm("sti");
        PIT::Sleep(10);
        asm("cli");

        for(int j = 0; j < 2; j++) {
            *((volatile uint32_t*)(lapicAddress + 0x280)) = 0;
            *((volatile uint32_t*)(lapicAddress + 0x310)) = i << 24;
            asm("sti");
            PIT::Sleep(1);
            asm("cli");
            *((volatile uint32_t*)(lapicAddress + 0x300)) = 0x000608;
        }

		do { __asm__ __volatile__ ("pause" : : : "memory"); }while(*((volatile uint32_t*)(lapicAddress + 0x300)) & (1 << 12));
        
        printf("Waiting cpu\n");
        globalGraphics->Update(); 
        while (DataTrampoline.Status == 0); // wait processor
        printf("cpu respond with : %u \n", DataTrampoline.Status);
        globalGraphics->Update(); 
        while (DataTrampoline.Status != 3);
        printf("Core %u init", i);
        globalGraphics->Update();   
    }
}   
  

void InitializeKernel(BootInfo* bootInfo){   
    r = graphics(bootInfo);
    globalGraphics = &r;

    gdtInit();

    InitializeMemory(bootInfo);

    memset(globalGraphics->framebuffer->BaseAddress, 0, globalGraphics->framebuffer->FrameBufferSize);

    InitializeHeap((void*)0x0000100000000000, 0x10);

    globalGraphics->framebuffer->BaseAddressBackground = malloc(globalGraphics->framebuffer->FrameBufferSize);
    memset(globalGraphics->framebuffer->BaseAddressBackground, 0, globalGraphics->framebuffer->FrameBufferSize);
    globalGraphics->Update();

    InitializeInterrupts();  
    
    InitPS2Mouse();

    IoWrite8(PIC1_DATA, 0b11111000);
    IoWrite8(PIC2_DATA, 0b11101111);

    FPUInit();
    
    InitializeACPI(bootInfo);

    LoadCores();

    asm("sti");
    
    //globalTaskManager.AddTask((void*)task1, 4096);
    //globalTaskManager.AddTask((void*)task2, 4096);
    //globalTaskManager.AddTask((void*)task3, 4096);
    //globalTaskManager.AddTask((void*)task4, 4096);
    //globalTaskManager.EnabledScheduler();

    return;
}

