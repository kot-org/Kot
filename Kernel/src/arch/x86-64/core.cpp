#include <arch/x86-64.h>

void InitializeACPI(BootInfo* bootInfo){
    ACPI::RSDP2* RSDP = (ACPI::RSDP2*)bootInfo->RSDP->rsdp;

    ACPI::MADTHeader* madt = (ACPI::MADTHeader*)ACPI::FindTable(RSDP, (char*)"APIC");
    APIC::InitializeMADT(madt);
    globalLogs->Successful("APIC intialize");

    ACPI::HPETHeader* hpet = (ACPI::HPETHeader*)ACPI::FindTable(RSDP, (char*)"HPET");
    HPET::InitialiseHPET(hpet);
    globalLogs->Successful("HPET intialize");
}

KernelInfo* arch_initialize(void* boot){
    asm("cli");
    stivale2_struct* BootStruct = (stivale2_struct*)boot;

    BootInfo* bootInfo = Boot::Init(BootStruct);
    
    /* clear frame buffer */
    memset((void*)bootInfo->Framebuffer->framebuffer_addr, 0x0, bootInfo->Framebuffer->framebuffer_pitch * bootInfo->Framebuffer->framebuffer_height);

    SerialPort::Initialize();
    SerialPort::ClearMonitor();
    globalLogs->Message("Welcome to Kot kernel");

    gdtInit();
    globalLogs->Successful("GDT intialize");

    Pmm_Init(bootInfo->Memory);
    globalLogs->Successful("PMM intialize");

    uint64_t LastAddressUsed = vmm_Init(bootInfo);;
    globalLogs->Successful("VMM intialize");
    
    InitializeHeap((void*)LastAddressUsed, 0x10);
    globalLogs->Successful("Heap intialize");
    
    InitializeInterrupts();  
    globalLogs->Successful("IDT intialize");

    CPU::InitCPU();

    simdInit();
    globalLogs->Successful("SIMD intialize");
    
    InitializeACPI(bootInfo);

    globalTaskManager = (TaskManager*)calloc(sizeof(TaskManager));
    globalTaskManager->InitScheduler(APIC::ProcessorCount, (void*)&IdleTask);


    APIC::EnableAPIC(CPU::GetAPICID());
    APIC::localApicEOI(CPU::GetAPICID());
    APIC::StartLapicTimer();
    APIC::LoadCores();

    //creat parameters
    KernelInfo* kernelInfo = (KernelInfo*)malloc(sizeof(KernelInfo));

    //frame buffer
    memcpy(&kernelInfo->framebuffer, bootInfo->Framebuffer, sizeof(stivale2_struct_tag_framebuffer));

    //ramfs
    memcpy(&kernelInfo->ramfs, &bootInfo->ramfs, sizeof(ramfs_t));

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

    //page size
    kernelInfo->MMapPageSize = PAGE_SIZE;

    return kernelInfo;
}