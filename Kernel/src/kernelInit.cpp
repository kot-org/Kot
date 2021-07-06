#include "kernelInit.h"

PageTableManager pageTableManager = NULL;
graphics r = graphics(NULL);
CPU cpu;

void InitializeMemory(BootInfo* bootInfo){

    uint64_t mMapEntries = bootInfo->mMapSize / bootInfo->mMapDescSize;

    globalAllocator = PageFrameAllocator();

    globalAllocator.ReadEFIMemoryMap(bootInfo->mMap, bootInfo->mMapSize, bootInfo->mMapDescSize);

    uint64_t kernelPages = (uint64_t)bootInfo->KernelSize / 0x1000 + 1;

    globalAllocator.LockPages(0, kernelPages);

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

    asm ("mov %0, %%cr3" :: "r" (PML4));
}

void InitializeACPI(BootInfo* bootInfo){
    ACPI::SDTHeader* xsdt = (ACPI::SDTHeader*)(bootInfo->rsdp->XSDTAddress);

    ACPI::MCFGHeader* mcfg = (ACPI::MCFGHeader*)ACPI::FindTable(xsdt, (char*)"MCFG");
    PCI::EnumeratePCI(mcfg);

    ACPI::MADTHeader* madt = (ACPI::MADTHeader*)ACPI::FindTable(xsdt, (char*)"APIC");
    APIC::InitializeMADT(madt);

    ACPI::FADTHeader* fadt = (ACPI::FADTHeader*)ACPI::FindTable(xsdt, (char*)"FACP");
    ACPI::InitializeFADT(fadt);
}
  

void InitializeKernel(BootInfo* bootInfo){   
    r = graphics(bootInfo);
    globalGraphics = &r;

    gdtInit();

    InitializeMemory(bootInfo);

    memset(globalGraphics->framebuffer->BaseAddress, 0, globalGraphics->framebuffer->FrameBufferSize);

    InitializeHeap((void*)0x0000100000000000, 0x10);

    globalGraphics->framebuffer->BaseAddressBackground = mallocK(globalGraphics->framebuffer->FrameBufferSize);
    memset(globalGraphics->framebuffer->BaseAddressBackground, 0, globalGraphics->framebuffer->FrameBufferSize);
    globalGraphics->Update();

    InitializeInterrupts();  
    
    InitPS2Mouse();

    IoWrite8(PIC1_DATA, 0b11111000);
    IoWrite8(PIC2_DATA, 0b11101111);
    
    if(EnabledSSE() == 0){
        FPUInit();
    }

    InitializeACPI(bootInfo);

    globalTaskManager.InitScheduler(APIC::ProcessorCount);
    globalTaskManager.AddTask((void*)task1, 4096, false, true);
    globalTaskManager.AddTask((void*)task2, 4096, false, true);
    globalTaskManager.AddTask((void*)task3, 4096, false, true);
    globalTaskManager.AddTask((void*)task4, 4096, false, true);

    APIC::StartLapicTimer();

    //APIC::LoadCores(); 

    //globalTaskManager.EnabledScheduler(0);
    asm("sti");

    return;
}

