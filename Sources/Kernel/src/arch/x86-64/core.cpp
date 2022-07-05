#include <arch/x86-64.h>

void InitializeACPI(BootInfo* bootInfo){
    if(bootInfo->RSDP == NULL){
        KernelPanic("RSDP not found");
    }

    ACPI::RSDP2* RSDP = (ACPI::RSDP2*)vmm_Map((uintptr_t)bootInfo->RSDP->rsdp);

    ACPI::MADTHeader* madt = (ACPI::MADTHeader*)ACPI::FindTable(RSDP, (char*)"APIC");

    if(madt == NULL){
        KernelPanic("APIC not found");
    }

    APIC::InitializeMADT(madt);
    Successful("APIC intialize");

    ACPI::HPETHeader* hpet = (ACPI::HPETHeader*)ACPI::FindTable(RSDP, (char*)"HPET");
    if(hpet == NULL){
        KernelPanic("HPET not found");
    }
    
    HPET::InitialiseHPET(hpet);
    Successful("HPET intialize");
}

KernelInfo* arch_initialize(uintptr_t boot){
    asm("cli");
    stivale2_struct* BootStruct = (stivale2_struct*)boot;

    BootInfo* bootInfo = Boot::Init(BootStruct);

    SerialPort::Initialize();
    SerialPort::ClearMonitor();
    Message("Welcome to Kot kernel");

    gdtInit();
    Successful("GDT intialize");

    Pmm_Init(bootInfo->Memory);
    Successful("PMM intialize");

    uint64_t LastAddressUsed = vmm_Init(bootInfo);;
    Successful("VMM intialize");
    
    InitializeHeap((uintptr_t)LastAddressUsed, 0x10);
    Successful("Heap intialize");
    
    InitializeInterrupts();  
    Successful("IDT intialize");

    CPU::InitCPU();

    simdInit();
    Successful("SIMD intialize");
    
    InitializeACPI(bootInfo);

    globalTaskManager = (TaskManager*)calloc(sizeof(TaskManager));
    globalTaskManager->InitScheduler(APIC::ProcessorCount, (uintptr_t)&IdleTask);


    APIC::EnableAPIC(CPU::GetAPICID());
    APIC::localApicEOI(CPU::GetAPICID());
    APIC::StartLapicTimer();
    APIC::LoadCores();

    //create parameters
    KernelInfo* kernelInfo = (KernelInfo*)malloc(sizeof(KernelInfo));

    //frame buffer
    memcpy(&kernelInfo->framebuffer, bootInfo->Framebuffer, sizeof(stivale2_struct_tag_framebuffer));

    //ramfs
    memcpy(&kernelInfo->ramfs, &bootInfo->ramfs, sizeof(ramfs_t));

    //memory info
    kernelInfo->memoryInfo = (memoryInfo_t*)vmm_GetPhysical(vmm_PageTable, &memoryInfo);

    //smbios
    if(bootInfo->smbios->smbios_entry_32 != 0){
        kernelInfo->smbios = (uintptr_t)bootInfo->smbios->smbios_entry_32;
    }else if(bootInfo->smbios->smbios_entry_64 != 0){
        kernelInfo->smbios = (uintptr_t)bootInfo->smbios->smbios_entry_64;
    }
    
    kernelInfo->rsdp = (uintptr_t)bootInfo->RSDP->rsdp;

    return kernelInfo;
}

void StopAllCPU(){
    for(uint8_t i = 0; i < APIC::ProcessorCount; i++){
        APIC::GenerateInterruption(i, IPI_Stop);
    }

    while(true){
        asm("hlt");
    }
}

void SetupRegistersForTask(thread_t* self){
    self->Regs->rip = (uint64_t)self->EntryPoint;
    self->RingPL = GetRingPL(self->Priviledge);
    self->Regs->cs = (GDTInfoSelectorsRing[self->RingPL].Code | self->RingPL);
    self->Regs->ss = (GDTInfoSelectorsRing[self->RingPL].Data | self->RingPL);
    self->Regs->rflags.Reserved0 = true;
    self->Regs->rflags.IF = true;
    if(self->Priviledge == PriviledgeDriver){
        // Allow IO
        self->Regs->rflags.IOPL = 0x3;
        // Allow CPUID
        self->Regs->rflags.ID = true;
    }
    self->IOPL = self->Regs->rflags.IOPL;
    self->Regs->cr3 = (uint64_t)self->Paging; 
}