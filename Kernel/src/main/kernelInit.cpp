#include "kernelInit.h"

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
    globalPageTableManager[CPU::GetCoreID()].PageTableManagerInit(PML4);

    PageTableManager UEFI_Table;
    UEFI_Table.PageTableManagerInit((PageTable*)bootInfo->memoryInfo.UEFI_CR3);

    //map kernel
    uint64_t KernelPageSize = Divide(bootInfo->memoryInfo.VirtualKernelEnd - bootInfo->memoryInfo.VirtualKernelStart, 0x1000);
    LastVirtualAddressUsed = bootInfo->memoryInfo.VirtualKernelEnd;

    LastVirtualAddressUsed += 0x1000 - (LastVirtualAddressUsed % 0x1000);
    globalPageTableManager[CPU::GetCoreID()].DefinePhysicalMemoryLocation((void*)(LastVirtualAddressUsed + 0x1000));

    for(uint64_t i = 0; i < KernelPageSize; i++){
        void* VirtualAddress = (void*)(bootInfo->memoryInfo.VirtualKernelStart + i * 0x1000);
        void* PhysicalAddress = UEFI_Table.GetPhysicalAddress(VirtualAddress);
        globalPageTableManager[CPU::GetCoreID()].MapMemory(VirtualAddress, PhysicalAddress);
    }
    
    memorySize = GetMemorySize(bootInfo->memoryInfo.mMap, mMapEntries, bootInfo->memoryInfo.mMapDescSize);

    //map all the memory

    for(uint64_t i = 0; i < memorySize; i += 0x1000){
        LastVirtualAddressUsed += 0x1000;
        globalPageTableManager[CPU::GetCoreID()].MapMemory((void*)LastVirtualAddressUsed, (void*)i);
    }
    LastVirtualAddressUsed += 0x1000;

    globalPageTableManager[CPU::GetCoreID()].DefineVirtualTableLocation();

    return PML4;
}

void InitializeACPI(BootInfo* bootInfo){
    bootInfo->rsdp = (ACPI::RSDP2*)globalPageTableManager[CPU::GetCoreID()].GetVirtualAddress(bootInfo->rsdp);
    
    ACPI::SDTHeader* xsdt = (ACPI::SDTHeader*)globalPageTableManager[CPU::GetCoreID()].GetVirtualAddress((void*)bootInfo->rsdp->XSDTAddress);

    ACPI::MADTHeader* madt = (ACPI::MADTHeader*)ACPI::FindTable(xsdt, (char*)"APIC");
    APIC::InitializeMADT(madt);

    ACPI::HPETHeader* hpet = (ACPI::HPETHeader*)ACPI::FindTable(xsdt, (char*)"HPET");
    HPET::InitialiseHPET(hpet);
}
  

void InitializeKernel(BootInfo* bootInfo){   
    asm("cli");

    globalCOM1->Initialize();
    globalCOM1->ClearMonitor();
    globalLogs->Message("Welcome to Kot's kernel");
    
    gdtInit();
    globalLogs->Successful("GDT intialize");

    InitializeInterrupts();  
    globalLogs->Successful("IDT intialize");

    PageTable* PML4 = InitializeMemory(bootInfo);
    LoadPaging(PML4, globalPageTableManager[CPU::GetCoreID()].PhysicalMemoryVirtualAddress);
    globalLogs->Successful("Memory intialize");

    //Update bootinfo location
    bootInfo = (BootInfo*)globalPageTableManager[CPU::GetCoreID()].GetVirtualAddress(bootInfo);

    InitializeHeap((void*)LastVirtualAddressUsed, 0x10);
    globalLogs->Successful("Heap intialize");


    CPU::InitCPU();

    if(EnabledSSE() == 0){
        FPUInit();
        globalLogs->Successful("FPU intialize");
    }else{
        globalLogs->Successful("SSE intialize");
    }
    
    InitializeACPI(bootInfo);

    globalTaskManager = (TaskManager*)calloc(sizeof(TaskManager));
    globalTaskManager->InitScheduler(APIC::ProcessorCount);


    APIC::EnableAPIC(CPU::GetCoreID());
    APIC::localApicEOI(CPU::GetCoreID());
    APIC::StartLapicTimer();

    //creat parameters
    KernelInfo* kernelInfo = (KernelInfo*)malloc(sizeof(KernelInfo));

    //frame buffer
    kernelInfo->framebuffer = (Framebuffer*)malloc(sizeof(Framebuffer));
    memcpy(kernelInfo->framebuffer, &bootInfo->framebuffer, sizeof(Framebuffer));

    //ramfs
    kernelInfo->ramfs = (RamFs*)malloc(sizeof(RamFs));
    memcpy(kernelInfo->ramfs, &bootInfo->ramfs, sizeof(RamFs));

    //memory info
    kernelInfo->memoryInfo = &memoryInfo;

    //smbios
    kernelInfo->smbios = bootInfo->smbios;

    //rsdp
    kernelInfo->rsdp = bootInfo->rsdp;

    Parameters* InitParameters = (Parameters*)malloc(sizeof(Parameters));
    InitParameters->Parameter0 = (uint64_t)kernelInfo;


    //load init file
    RamFS::Parse(globalPageTableManager[CPU::GetCoreID()].GetVirtualAddress(bootInfo->ramfs.RamFsBase), bootInfo->ramfs.Size);
    RamFS::File* InitFile = RamFS::FindInitFile();
    if(InitFile != NULL){
        void* BufferInitFile = malloc(InitFile->size);
        Read(InitFile, BufferInitFile);
        ELF::loadElf(BufferInitFile, 1, InitParameters);
    }else{
        globalLogs->Error("Can't load initialization file");
    }
    free(InitParameters);

    APIC::LoadCores(); 

    globalTaskManager->EnabledScheduler(CPU::GetCoreID());

    LaunchUserSpace();

    return;
} 