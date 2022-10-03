#include <arch/x86-64.h>

void InitializeACPI(ukl_boot_structure_t* BootData, ArchInfo_t* ArchInfo){
    if(BootData->RSDP.base == NULL){
        KernelPanic("RSDP not found");
    }

    ACPI::RSDP2* RSDP = (ACPI::RSDP2*)vmm_Map((uintptr_t)BootData->RSDP.base, BootData->RSDP.size);

    ACPI::MADTHeader* madt = (ACPI::MADTHeader*)ACPI::FindTable(RSDP, (char*)"APIC");

    if(madt == NULL){
        KernelPanic("APIC not found");
    }

    APIC::InitializeMADT(madt, ArchInfo);
    Successful("APIC intialize");

    ACPI::HPETHeader* hpet = (ACPI::HPETHeader*)ACPI::FindTable(RSDP, (char*)"HPET");
    if(hpet == NULL){
        KernelPanic("HPET not found");
    }
    
    HPET::InitialiseHPET(hpet);
    Successful("HPET intialize");
}

ArchInfo_t* arch_initialize(ukl_boot_structure_t* BootData){
    asm("cli");

    vmm_HHDMAdress = BootData->memory_info.HHDM;

    SerialPort::Initialize();
    SerialPort::ClearMonitor();
    Message("Welcome to Kot kernel");

    gdtInit();
    Successful("GDT initialized");

    Pmm_Init(&BootData->memory_info);
    Successful("PMM initialized");

    uint64_t LastAddressUsed = vmm_Init(BootData);
    Successful("VMM initialized");
    
    InitializeHeap((uintptr_t)LastAddressUsed, 0x10);
    Successful("Heap initialized");

    ArchInfo_t* ArchInfo = (ArchInfo_t*)malloc(sizeof(ArchInfo_t));
    CPU::InitCPU(ArchInfo);
    CPU::InitCore();

    InitializeACPI(BootData, ArchInfo);
    Successful("ACPI initialized");

    InitializeInterrupts(ArchInfo);  
    Successful("IDT initialized");

    simdInit();
    Successful("SIMD initialized");
    

    globalTaskManager = (TaskManager*)calloc(sizeof(TaskManager));
    globalTaskManager->InitScheduler(APIC::ProcessorCount, (uintptr_t)&IdleTask);


    APIC::EnableAPIC(CPU::GetAPICID());
    APIC::localApicEOI(CPU::GetAPICID());
    APIC::StartLapicTimer();
    APIC::LoadCores();

    //framebuffer
    memcpy(&ArchInfo->framebuffer, &BootData->framebuffer, sizeof(ukl_framebuffer_t));

    //initrd
    memcpy(&ArchInfo->initrd, &BootData->initrd, sizeof(ukl_initrd_t));

    //memory info
    ArchInfo->memoryInfo = (memoryInfo_t*)vmm_GetPhysical(vmm_PageTable, &Pmm_MemoryInfo);

    //smbios
    ArchInfo->smbios = (uintptr_t)BootData->SMBIOS.base;
    
    ArchInfo->rsdp = (uintptr_t)BootData->RSDP.base;


    initrd::Parse((uintptr_t)ArchInfo->initrd.base, ArchInfo->initrd.size);
    Successful("Initrd initialized");
    return ArchInfo;
}

KResult GetDataToStartService(ArchInfo_t* ArchInfo, kthread_t* thread, arguments_t* Parameters, uintptr_t* Data, size64_t* Size){
    KResult Status = KFAIL;
    ArchInfo->IRQEvents = (event_t*)calloc(ArchInfo->IRQSize * sizeof(event_t));
    for(uint64_t i = 0; i < ArchInfo->IRQSize; i++){
        if(InterruptEventList[i] != NULL){
            Status = Keyhole_Create((key_t*)&ArchInfo->IRQEvents[i], thread->Parent, thread->Parent, DataTypeEvent, (uint64_t)InterruptEventList[i], KeyholeFlagFullPermissions, PriviledgeApp);
            if(Status != KSUCCESS) return Status;
        }
    }
    *Data = ArchInfo;
    *Size = sizeof(ArchInfo_t);
    return Status;
}

void StopAllCPU(){
    for(uint8_t i = 0; i < APIC::ProcessorCount; i++){
        APIC::GenerateInterruption(i, INT_Stop);
    }

    asm("int $0x42");
}

void SetupRegistersForTask(kthread_t* self){
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