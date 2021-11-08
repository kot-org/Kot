#include "kernelInit.h"

graphics r = graphics(NULL);
CPU cpu;

uint64_t LastVirtualAddressUsed = 0;
uint64_t memorySize = 0;

PageTable* InitializeMemory(BootInfo* bootInfo){
    uint64_t mMapEntries = bootInfo->memoryInfo.mMapSize / bootInfo->memoryInfo.mMapDescSize;

    globalAllocator = PageFrameAllocator();

    globalAllocator.ReadEFIMemoryMap(bootInfo->memoryInfo.mMap, bootInfo->memoryInfo.mMapSize, bootInfo->memoryInfo.mMapDescSize);
    
    uint64_t fbBase = (uint64_t)bootInfo->framebuffer.BaseAddress;
    uint64_t fbSize = (uint64_t)bootInfo->framebuffer.FrameBufferSize + 0x1000;
    globalAllocator.LockPages((void*)fbBase, Divide(fbSize, 0x1000));

    PageTable* PML4 = (PageTable*)globalAllocator.RequestPage();
    memset(PML4, 0, 0x1000);

    globalPageTableManager.PageTableManagerInit(PML4);

    PageTableManager UEFI_Table;
    UEFI_Table.PageTableManagerInit((PageTable*)bootInfo->memoryInfo.UEFI_CR3);

    //map kernel
    uint64_t KernelPageSize = Divide(bootInfo->memoryInfo.VirtualKernelEnd - bootInfo->memoryInfo.VirtualKernelStart, 0x1000);
    LastVirtualAddressUsed = bootInfo->memoryInfo.VirtualKernelEnd;
    globalPageTableManager.DefinePhysicalMemoryLocation((void*)(LastVirtualAddressUsed + 0x1000));

    for(uint64_t i = 0; i < KernelPageSize; i++){
        void* VirtualAddress = (void*)(bootInfo->memoryInfo.VirtualKernelStart + i * 0x1000);
        void* PhysicalAddress = UEFI_Table.GetPhysicalAddress(VirtualAddress);
        globalPageTableManager.MapMemory(VirtualAddress, PhysicalAddress);
    }
    
    memorySize = GetMemorySize(bootInfo->memoryInfo.mMap, mMapEntries, bootInfo->memoryInfo.mMapDescSize);

    //map all the memory

    for(uint64_t i = 0; i < memorySize; i += 0x1000){
        LastVirtualAddressUsed += 0x1000;
        globalPageTableManager.MapMemory((void*)LastVirtualAddressUsed, (void*)i);
    }

    LastVirtualAddressUsed += 0x1000;

    globalPageTableManager.DefineVirtualTableLocation();

    return PML4;
}

void InitializeACPI(BootInfo* bootInfo){
    bootInfo->rsdp = (ACPI::RSDP2*)globalPageTableManager.GetVirtualAddress(bootInfo->rsdp);
    
    ACPI::SDTHeader* xsdt = (ACPI::SDTHeader*)globalPageTableManager.GetVirtualAddress((void*)bootInfo->rsdp->XSDTAddress);
    ACPI::MCFGHeader* mcfg = (ACPI::MCFGHeader*)ACPI::FindTable(xsdt, (char*)"MCFG");
    PCI::EnumeratePCI(mcfg);

    ACPI::MADTHeader* madt = (ACPI::MADTHeader*)ACPI::FindTable(xsdt, (char*)"APIC");
    APIC::InitializeMADT(madt);

    ACPI::FADTHeader* fadt = (ACPI::FADTHeader*)ACPI::FindTable(xsdt, (char*)"FACP");
    ACPI::InitializeFADT(fadt);
}
  

void InitializeKernel(BootInfo* bootInfo){   
    asm("cli");
    globalCOM1->Initialize();
    globalCOM1->ClearMonitor();
    globalLogs->Message("(c) Kot Corporation. All rights reserved");

    gdtInit();
    globalLogs->Successful("GDT intialize");

    InitializeInterrupts();  
    globalLogs->Successful("IDT intialize");

    PageTable* PML4 = InitializeMemory(bootInfo);
    LoadPaging(PML4, globalPageTableManager.PhysicalMemoryVirtualAddress);
    globalLogs->Successful("Memory intialize");

    //Update bootinfo location
    bootInfo = (BootInfo*)globalPageTableManager.GetVirtualAddress(bootInfo);


    globalLogs->Message("CPU : %s %s", globalCPU.getName(), globalCPU.getVendorID());
    globalCPU.getFeatures();
    globalLogs->Message("CPU features :");
    for(int i = 0; i < globalCPU.cpuFeatures; i++){
        globalCOM1->Print(globalCPU.features[i]);
        globalCOM1->Print(" | ");
    }
    globalCOM1->Print("\n");

    InitializeHeap((void*)LastVirtualAddressUsed, 0x10);
    globalLogs->Successful("Heap intialize");

    r = graphics(bootInfo);
    globalGraphics = &r;
    globalGraphics->framebuffer->BaseAddressBackground = malloc(globalGraphics->framebuffer->FrameBufferSize);
    memset(globalGraphics->framebuffer->BaseAddressBackground, 0, globalGraphics->framebuffer->FrameBufferSize);
    globalGraphics->Update();
    globalLogs->Successful("Graphics intialize");

    if(EnabledSSE() == 0){
        FPUInit();
        globalLogs->Successful("FPU intialize");
    }else{
        globalLogs->Successful("SSE intialize");
    }
    
    InitializeACPI(bootInfo);
    
    InitPS2Mouse();

    // GPT::Partition partitionTest = GPT::Partition(AHCI::ahciDriver->Ports[1], GPT::GetPartitionByGUID(AHCI::ahciDriver->Ports[1], GPT::GetDataGUIDPartitionType()));  
    // FileSystem::KFS* Fs = new FileSystem::KFS(&partitionTest);
    // Fs->mkdir("system", 777);
    // Fs->mkdir("system/background", 777);   
    // Fs->mkdir("system/apps", 777);   


    // FileSystem::File* picture = Fs->fopen("system/background/1.bmp", "r");

    // void* pictureBuffer = malloc(picture->fileInfo->BytesSize);
    // picture->Read(0, picture->fileInfo->BytesSize, pictureBuffer);

    // unsigned char info[54];
    // for(int i = 0; i < 54; i++){
    //     info[i] =  *(uint8_t*)((uint64_t)pictureBuffer + i);
    // }
    

    // int dataOffset = *(int*)&info[10]; 
    // int src_width = *(int*)&info[18];
    // int src_height = *(int*)&info[22];
    // int bitCount = (*(short*)&info[28]) / 8;

    // pictureBuffer += dataOffset;

    // for(int i = 0; i < globalGraphics->framebuffer->Height; i++) {
    //     for(int j = 0; j < globalGraphics->framebuffer->Width; j++){
    //         uint64_t position = ((globalGraphics->framebuffer->Height - i) * src_width + j) * 4;
    //         uint8_t r = *(uint8_t*)((uint64_t)pictureBuffer + position + 2);
    //         uint8_t g = *(uint8_t*)((uint64_t)pictureBuffer + position + 1);
    //         uint8_t b = *(uint8_t*)((uint64_t)pictureBuffer + position);
    //         globalGraphics->Putpixel(j, i, r, g, b);
    //     }
    // }


    globalTaskManager.InitScheduler(APIC::ProcessorCount);

    GPT::Partition partitionTest = GPT::Partition(AHCI::ahciDriver->Ports[1], GPT::GetPartitionByGUID(AHCI::ahciDriver->Ports[1], GPT::GetSystemGUIDPartitionType()));  
    FileSystem::KFS* Fs = new FileSystem::KFS(&partitionTest);

    Fs->mkdir("system", 777);
    Fs->mkdir("system/background", 777);   
    Fs->mkdir("system/apps", 777);  

    FileSystem::File* app = Fs->fopen("system/apps/main.elf", "r");
    void* appBuffer = malloc(app->fileInfo->BytesSize);
    app->Read(0, app->fileInfo->BytesSize, appBuffer);
    ELF::loadElf(appBuffer, 1);


    APIC::EnableAPIC();
    APIC::localApicEOI();
    APIC::StartLapicTimer();

    APIC::LoadCores(); 
    globalTaskManager.EnabledScheduler(0);
    asm("sti");

    return;
}