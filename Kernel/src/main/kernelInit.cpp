#include "kernelInit.h"

PageTableManager pageTableManager = NULL;
graphics r = graphics(NULL);
CPU cpu;

void InitializeMemory(BootInfo* bootInfo){

    uint64_t mMapEntries = bootInfo->mMapSize / bootInfo->mMapDescSize;

    globalAllocator = PageFrameAllocator();

    globalAllocator.ReadEFIMemoryMap(bootInfo->mMap, bootInfo->mMapSize, bootInfo->mMapDescSize);

    uint64_t kernelPages = Divide((uint64_t)bootInfo->KernelStart + bootInfo->KernelSize, 0x1000);

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
    globalCOM1->Initialize();
    globalCOM1->ClearMonitor();
    globalLogs->Message("(c) Kot Corporation. All rights reserved");
    globalLogs->Message("CPU : %s %s", globalCPU.getName(), globalCPU.getVendorID());
    globalCPU.getFeatures();
    globalLogs->Message("CPU features :");
    for(int i = 0; i < globalCPU.cpuFeatures; i++){
        globalCOM1->Print(globalCPU.features[i]);
        globalCOM1->Print(" | ");
    }
    globalCOM1->Print("\n");

    r = graphics(bootInfo);
    globalGraphics = &r;

    gdtInit();
    globalLogs->Successful("GDT intialize");


    InitializeMemory(bootInfo);
    globalLogs->Successful("Memory intialize : map and paging");

    memset(globalGraphics->framebuffer->BaseAddress, 128, globalGraphics->framebuffer->FrameBufferSize);


    InitializeHeap((void*)0x0000010000000000, 0x10);
    globalLogs->Successful("Heap intialize");

    globalGraphics->framebuffer->BaseAddressBackground = malloc(globalGraphics->framebuffer->FrameBufferSize);
    memset(globalGraphics->framebuffer->BaseAddressBackground, 0, globalGraphics->framebuffer->FrameBufferSize);
    globalGraphics->Update();

    InitializeInterrupts();  
    globalLogs->Successful("IDT intialize");
    globalLogs->Message("RAM : %s", ConvertByte(globalAllocator.GetTotalRAM()));
    
    InitPS2Mouse();

    IoWrite8(PIC1_DATA, 0b11111011); /* problem with pit and keyboard */
    IoWrite8(PIC2_DATA, 0b11101111);
    
    if(EnabledSSE() == 0){
        FPUInit();
        globalLogs->Successful("FPU intialize");
    }else{
        globalLogs->Successful("SSE intialize");
    }

    InitializeACPI(bootInfo);

    GPT::Partition partitionTest = GPT::Partition(AHCI::ahciDriver->Ports[1], GPT::GetPartitionByGUID(AHCI::ahciDriver->Ports[1], GPT::GetDataGUIDPartitionType()));  
    FileSystem::KFS* Fs = new FileSystem::KFS(&partitionTest);

    Fs->mkdir("system", 777);
    Fs->mkdir("system/background", 777);
    FileSystem::File* picture = Fs->fopen("system/background/1.bmp", "r");

    void* pictureBuffer = malloc(picture->fileInfo->BytesSize);
    picture->Read(0, picture->fileInfo->BytesSize, pictureBuffer);

    unsigned char info[54];
    for(int i = 0; i < 54; i++){
        info[i] =  *(uint8_t*)((uint64_t)pictureBuffer + i);
    }
    

    int dataOffset = *(int*)&info[10]; 
    int src_width = *(int*)&info[18];
    int src_height = *(int*)&info[22];
    int bitCount = (*(short*)&info[28]) / 8;

    pictureBuffer += dataOffset;

    globalLogs->Successful("%u %u", src_width, src_height);

    for(int i = 0; i < globalGraphics->framebuffer->Height; i++) {
        for(int j = 0; j < globalGraphics->framebuffer->Width; j++){
            uint8_t r = *(uint8_t*)((uint64_t)pictureBuffer + (i * src_width + j) * 4 + 0);
            uint8_t g = *(uint8_t*)((uint64_t)pictureBuffer + (i * src_width + j) * 4 + 1);
            uint8_t b = *(uint8_t*)((uint64_t)pictureBuffer + (i * src_width + j) * 4 + 2);
            globalGraphics->Putpixel(j, i, r, g, b);
        }
    }
    
    globalLogs->Successful("");
    while (true){
        asm("hlt");
    }


    globalTaskManager.InitScheduler(APIC::ProcessorCount);
    globalTaskManager.AddTask((void*)task1, 4096, false, true, 1);
    globalTaskManager.AddTask((void*)task2, 4096, false, true, 2);
    globalTaskManager.AddTask((void*)task3, 4096, false, true, 3);
    globalTaskManager.AddTask((void*)task4, 4096, false, true, 3);

    APIC::EnableAPIC();
    APIC::localApicEOI();
    APIC::StartLapicTimer();

    APIC::LoadCores(); 
    globalTaskManager.EnabledScheduler(0);
    asm("sti");

    return;
}