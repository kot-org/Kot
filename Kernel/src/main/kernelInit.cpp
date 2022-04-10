#include <main/kernelInit.h>

uint64_t LastVirtualAddressUsed = 0;
uint64_t memorySize = 0;

void InitializeMemory(BootInfo* bootInfo){
    globalAllocator = PageFrameAllocator();

    globalAllocator.ReadMemoryMap(bootInfo->Memory);

    vmm_Init(bootInfo);
    return;
}

void InitializeACPI(BootInfo* bootInfo){
    ACPI::RSDP2* RSDP = (ACPI::RSDP2*)bootInfo->RSDP->rsdp;

    ACPI::MADTHeader* madt = (ACPI::MADTHeader*)ACPI::FindTable(RSDP, (char*)"APIC");
    APIC::InitializeMADT(madt);

    ACPI::HPETHeader* hpet = (ACPI::HPETHeader*)ACPI::FindTable(RSDP, (char*)"HPET");
    HPET::InitialiseHPET(hpet);
}
  

void InitializeKernel(stivale2_struct* stivale2_struct){  
    asm("cli");

    BootInfo* bootInfo = Boot::Init(stivale2_struct);

    SerialPort::Initialize();
    SerialPort::ClearMonitor();
    globalLogs->Message("Welcome to Kot's kernel");

    gdtInit();
    globalLogs->Successful("GDT intialize");

    InitializeMemory(bootInfo);
    globalLogs->Successful("Memory intialize");
    
    InitializeHeap((void*)LastVirtualAddressUsed, 0x10);
    globalLogs->Successful("Heap intialize");

    InitializeInterrupts();  
    globalLogs->Successful("IDT intialize");

    CPU::InitCPU();

    simdInit();
    globalLogs->Successful("SIMD intialize");
    
    InitializeACPI(bootInfo);

    globalTaskManager = (TaskManager*)calloc(sizeof(TaskManager));
    globalTaskManager->InitScheduler(APIC::ProcessorCount);


    APIC::EnableAPIC(CPU::GetCoreID());
    APIC::localApicEOI(CPU::GetCoreID());
    APIC::StartLapicTimer();

    //creat parameters
    KernelInfo* kernelInfo = (KernelInfo*)malloc(sizeof(KernelInfo));

    //frame buffer
    kernelInfo->framebuffer = (stivale2_struct_tag_framebuffer*)malloc(sizeof(stivale2_struct_tag_framebuffer));
    memcpy(kernelInfo->framebuffer, &bootInfo->Framebuffer, sizeof(stivale2_struct_tag_framebuffer));

    //ramfs
    kernelInfo->ramfs = (RamFs*)malloc(sizeof(RamFs));
    memcpy(kernelInfo->ramfs, &bootInfo->ramfs, sizeof(RamFs));

    //memory info
    kernelInfo->memoryInfo = &memoryInfo;

    //smbios
    if(bootInfo->smbios->smbios_entry_32 != 0){
        kernelInfo->smbios = (void*)bootInfo->smbios->smbios_entry_32;
    }else if(bootInfo->smbios->smbios_entry_64 != 0){
        kernelInfo->smbios = (void*)bootInfo->smbios->smbios_entry_64;
    }
    

    //rsdp
    kernelInfo->rsdp = (void*)bootInfo->RSDP->rsdp;

    Parameters* InitParameters = (Parameters*)malloc(sizeof(Parameters));
    InitParameters->Parameter0 = (uint64_t)kernelInfo;


    //load init file
    RamFS::Parse(bootInfo->ramfs.RamFsBase, bootInfo->ramfs.Size);
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