#include "apic.h"

namespace APIC{ 
    //processors
    LocalProcessor** Processor;
    uint8_t ProcessorCount;

    //IOAPIC
    IOAPIC** IOapic;
    uint64_t IOAPICCount;

    //ISO
    InterruptSourceOverride** Iso;
    uint64_t IsoCount;

    void* lapicAddressVirtual;
    void* IOapicAddressVirtual;

    void InitializeMADT(ACPI::MADTHeader* madt){
        lapicAddressVirtual = globalPageTableManager.MapMemory(0, 1);
        IOapicAddressVirtual = globalPageTableManager.MapMemory(0, 1);
        ProcessorCount = 0;
        IsoCount = 0;

        if(madt == 0){
            return;
        }

        uint64_t entries = (madt->Header.Length - sizeof(ACPI::MADTHeader));

        for(uint64_t i = 0; i < entries;){
            EntryRecord* entryRecord = (EntryRecord*)((uint64_t)madt + sizeof(ACPI::MADTHeader) + i);
            i += entryRecord->Length;

            switch(entryRecord->Type){
                case EntryTypeLocalProcessor: {
                    ProcessorCount++;        
                    break;                    
                }
                case EntryTypeIOAPIC:{
                    IOAPICCount++;
                    break;
                }                    
                case EntryTypeInterruptSourceOverride:{
                    IsoCount++;
                    break;
                }                    
                case EntryTypeNonmaskableinterrupts:{
                    break;
                }                    
                case EntryTypeLocalAPICAddressOverride:{
                    break;  
                }                                      
            }
        }

        Processor = (LocalProcessor**)malloc(sizeof(LocalProcessor) * ProcessorCount);

        IOapic = (IOAPIC**)malloc(sizeof(IOAPIC) * IOAPICCount);

        Iso = (InterruptSourceOverride**)malloc(sizeof(InterruptSourceOverride) * IsoCount);

        uint8_t ProcessorCountTemp = 0;
        uint64_t IsoCountTemp = 0;
        uint8_t IOAPICTemp = 0;

        for(uint64_t i = 0; i < entries;){            
            EntryRecord* entryRecord = (EntryRecord*)((uint64_t)madt + sizeof(ACPI::MADTHeader) + i);
            i += entryRecord->Length;

            switch(entryRecord->Type){
                case EntryTypeLocalProcessor: {
                    LocalProcessor* processor = (LocalProcessor*)entryRecord;
                    Processor[ProcessorCountTemp] = processor;   
                    ProcessorCountTemp++;
                    break;                    
                }
                case EntryTypeIOAPIC:{
                    IOAPIC* ioApic = (IOAPIC*)entryRecord;
                    IOapic[IOAPICTemp] = ioApic;
                    IOAPICTemp++;
                    break;
                }                    
                case EntryTypeInterruptSourceOverride:{
                    InterruptSourceOverride* iso = (InterruptSourceOverride*)entryRecord;
                    Iso[IsoCountTemp] = iso;
                    IsoCountTemp++;
                    break;
                }                    
                case EntryTypeNonmaskableinterrupts:{
                    NonMaskableinterrupts* nmi = (NonMaskableinterrupts*)entryRecord;
                    break;
                }                    
                case EntryTypeLocalAPICAddressOverride:{
                    LocalAPICAddressOverride* local_apic_override = (LocalAPICAddressOverride*)entryRecord;
                    break;  
                }                                      
            }
        }
        IoAPICInit(0);
    }  

    void IoAPICInit(uint8_t IOApicID){
        // Disable PIC
        IoWrite8(0xa1, 0xff);
        IoWrite8(0x21, 0xff);

        // Configure first IOAPIC
        IOAPIC* ioapic = IOapic[IOApicID];
        globalPageTableManager.MapMemory((void*)IOapicAddressVirtual, (void*)(uint64_t)ioapic->APICAddress);
        uint8_t MaxInterrupts = ((ioapicReadRegister(IOapicAddressVirtual , IOAPICVersion) >> 16) & 0xff) + 1;
        ioapic->MaxInterrupts = MaxInterrupts;

        // Set up the entries
        uint32_t base = ioapic->GlobalSystemInterruptBase;
        for (size_t i = 0; i < 24; i++){
                SetIrqDefault(i);
                uint8_t IRQNumber = i + IRQ_START;
                IoApicSetRedirectionEntry((void*)IOapicAddressVirtual, i - base, (IOAPICRedirectionEntry){
                    .vector = IRQNumber,
                    .mask = IOAPICRedirectionEntryMaskDisable,
                });
        }

        for(size_t i = 0; i < IsoCount; i++) {
            InterruptSourceOverride* iso = Iso[i];
            uint8_t IRQNumber = iso->IRQSource + IRQ_START;
            IoApicSetRedirectionEntry(IOapicAddressVirtual , iso->IRQSource, (IOAPICRedirectionEntry){
                .vector = IRQNumber,
                .delivery_mode = IOAPICRedirectionEntryDeliveryModeFixed,
                .destination_mode = IOAPICRedirectionEntryDestinationModePhysicall,
                .pin_polarity = (iso->Flags & 0x03) == 0x03 ? IOAPICRedirectionEntryPinPolarityActiveLow : IOAPICRedirectionEntryPinPolarityActiveHigh,
                .trigger_mode = (iso->Flags & 0x0c) == 0x0c ? IOAPICRedirectionEntryTriggerModeLevel : IOAPICRedirectionEntryTriggerModeEdge,
                .mask = IOAPICRedirectionEntryMaskDisable,
            });
        } 

    } 

    void IoChangeIrqState(uint8_t irq, uint8_t IOApicID, bool IsEnable){
        IOAPIC* ioapic = IOapic[IOApicID];
        globalPageTableManager.MapMemory((void*)IOapicAddressVirtual, (void*)(uint64_t)ioapic->APICAddress);
        uint32_t base = ioapic->GlobalSystemInterruptBase;
        size_t index = irq - base;
        
        volatile uint32_t low = 0;

        low = ioapicReadRegister(IOapicAddressVirtual, IOAPICRedirectionTable + 2 * index);
        
        if(!IsEnable){
            low |= 1 << IOAPICRedirectionBitsLowMask;
        }else{
            low &= ~(1 << IOAPICRedirectionBitsLowMask);
        }

        ioapicWriteRegister(IOapicAddressVirtual, IOAPICRedirectionTable + 2 * index, low);
    }

    void LoadCores(){
        uint64_t lapicAddress = (uint64_t)GetLAPICAddress();
        void* TrampolineVirtualAddress = globalPageTableManager.MapMemory((void*)0x8000, 1);

        uint8_t bspid = 0; 
        __asm__ __volatile__ ("mov $1, %%rax; cpuid; shrq $24, %%rbx;": "=r"(bspid)::);

        memcpy((void*)TrampolineVirtualAddress, (void*)&Trampoline, 0x1000);

        trampolineData* Data = (trampolineData*) (((uint64_t)&DataTrampoline - (uint64_t) &Trampoline) + TrampolineVirtualAddress);

        Data->MainEntry = (uint64_t)&TrampolineMain; 
        
        //temp trampoline map
        globalPageTableManager.MapMemory((void*)0x8000, (void*)0x8000);

        for(int i = 1; i < ProcessorCount; i++){ 
            Data->Paging = (uint64_t)globalPageTableManager.PML4;
            uint64_t StackSize = 0x1000000; // 10 mb
            Data->Stack = (uint64_t)malloc(StackSize) + StackSize;
                
            if(Processor[i]->APICID == bspid) continue; 

            //init IPI
            localAPICWriteRegister(LocalAPICRegisterOffsetErrorStatus, 0);
            localAPICWriteRegister(LocalAPICRegisterOffsetInterruptCommand + 0x10, i << 24);
            localAPICWriteRegister(LocalAPICRegisterOffsetInterruptCommand, 0x00C500);
            do { __asm__ __volatile__ ("pause" : : : "memory"); }while(localAPICReadRegister(LocalAPICRegisterOffsetInterruptCommand) & (1 << 12));

            localAPICWriteRegister(LocalAPICRegisterOffsetInterruptCommand + 0x10, i << 24);
            localAPICWriteRegister(LocalAPICRegisterOffsetInterruptCommand, 0x008500);
            do { __asm__ __volatile__ ("pause" : : : "memory"); }while(localAPICReadRegister(LocalAPICRegisterOffsetInterruptCommand) & (1 << 12));

            for(int j = 0; j < 2; j++) {

                localAPICWriteRegister(LocalAPICRegisterOffsetErrorStatus, 0);
                localAPICWriteRegister(LocalAPICRegisterOffsetInterruptCommand + 0x10, i << 24);
                localAPICWriteRegister(LocalAPICRegisterOffsetInterruptCommand, 0x000608);
                do { __asm__ __volatile__ ("pause" : : : "memory"); }while(localAPICReadRegister(LocalAPICRegisterOffsetInterruptCommand) & (1 << 12));
            }

            globalLogs->Warning("Wait processor %u", i);

            while (Data->Status != 3); // wait processor
            globalLogs->Successful("Processor %u respond with success", i);

            while (StatusProcessor != 4); // wait processor
            globalLogs->Successful("Processor %u is in the main function", i);
            StatusProcessor = 0;
        }
        globalPageTableManager.UnmapMemory((void*)0x8000);
    }  

    void* GetLAPICAddress(){
        void* lapicAddress = (void*)(msr::rdmsr(0x1b) & 0xfffff000);
        globalPageTableManager.MapMemory(lapicAddressVirtual, lapicAddress);
        return lapicAddressVirtual;
    }

    void EnableAPIC(){
        void* lapicAddress = (void*)(msr::rdmsr(0x1b) & 0xfffff000);
        msr::wrmsr(0x1b, (uint64_t)lapicAddress);
        localAPICWriteRegister(0xF0, localAPICReadRegister(0xF0) | 0x1ff);
    }

    static uint64_t mutexSLT;
    void StartLapicTimer(){
        Atomic::atomicSpinlock(&mutexSLT, 0);
        Atomic::atomicLock(&mutexSLT, 0);
        localApicEnableSpuriousInterrupts();
        // Setup Local APIC timer
        localAPICWriteRegister(LocalAPICRegisterOffsetInitialCount, 0x10000);
        uint32_t divide = localAPICReadRegister(LocalAPICRegisterOffsetDivide);
        localAPICWriteRegister(LocalAPICRegisterOffsetDivide, (divide & 0xfffffff4) | 0b1010);
        uint32_t timer = localAPICReadRegister(LocalAPICRegisterOffsetLVTTimer);

        LocalAPICInterruptRegister TimerRegisters;
        TimerRegisters.vector = 0x40;
        TimerRegisters.mask = LocalAPICInterruptRegisterMaskEnable;
        TimerRegisters.timerMode = LocalAPICInterruptTimerModePeriodic;
        
        localAPICWriteRegister(LocalAPICRegisterOffsetLVTTimer, CreatRegisterValueInterrupts(TimerRegisters) | (timer & 0xfffcef00));      
        Atomic::atomicUnlock(&mutexSLT, 0);
    }

    void localAPICSetTimerCount(uint32_t value){
        localAPICWriteRegister(LocalAPICRegisterOffsetInitialCount, value);
    }

    uint32_t localAPICGetTimerCount(){
        return localAPICReadRegister(LocalAPICRegisterOffsetCurentCount);
    }

    void localApicEOI(){        
        localAPICWriteRegister(LocalAPICRegisterOffsetEOI, 0);
    }

    void localApicEnableSpuriousInterrupts(){
        localAPICWriteRegister(LocalAPICRegisterOffsetSpuriouseIntVector, localAPICReadRegister(LocalAPICRegisterOffsetSpuriouseIntVector) | 0x100);
    }

    /* APIC */

    uint32_t localAPICReadRegister(size_t offset){
        void* lapicAddress = GetLAPICAddress();
	    return *((volatile uint32_t*)((void*)((uint64_t)lapicAddress + offset)));
    }

    uint32_t localAPICReadRegister(void* lapicAddress, size_t offset){
	    return *((volatile uint32_t*)((void*)((uint64_t)lapicAddress + offset)));
    }

    uint32_t ioapicReadRegister(void* apicPtr , uint8_t offset){
        *(volatile uint32_t*)(apicPtr) = offset;
        return *(volatile uint32_t*)(apicPtr + 0x10);
    }

    void ioapicWriteRegister(void* apicPtr , uint8_t offset, uint32_t value){
        *(volatile uint32_t*)(apicPtr) = offset;
        *(volatile uint32_t*)(apicPtr + 0x10) = value;
    }
    
    void localAPICWriteRegister(size_t offset, uint32_t value){
        void* lapicAddress = GetLAPICAddress();
        *((volatile uint32_t*)((void*)((uint64_t)lapicAddress + offset))) = value;
    }

    void localAPICWriteRegister(void* lapicAddress, size_t offset, uint32_t value){
        *((volatile uint32_t*)((void*)((uint64_t)lapicAddress + offset))) = value;
    }

    uint32_t CreatRegisterValueInterrupts(LocalAPICInterruptRegister reg){
        return (
            (reg.vector << LocalAPICInterruptVector) |
            (reg.messageType << LocalAPICInterruptMessageType) |
            (reg.deliveryStatus << LocalAPICInterruptDeliveryStatus) |
            (reg.triggerMode << LocalAPICInterruptTrigerMode) |
            (reg.mask << LocalAPICInterruptMask) |
            (reg.timerMode << LocalAPICInterruptTimerMode)
        );
    }

    void IoApicSetRedirectionEntry(void* apicPtr, size_t index, IOAPICRedirectionEntry entry){
        volatile uint32_t low = (
            (entry.vector << IOAPICRedirectionBitsLowVector) |
            (entry.delivery_mode << IOAPICRedirectionBitsLowDeliveryMode) |
            (entry.destination_mode << IOAPICRedirectionBitsLowDestinationMode) |
            (entry.delivery_status << IOIOAPICRedirectionBitsLowDeliveryStatus) |
            (entry.pin_polarity << IOAPICRedirectionBitsLowPonPolarity) |
            (entry.remote_irr << IOAPICRedirectionBitsLowRemoteIrr) |
            (entry.trigger_mode << IOAPICRedirectionBitsLowTriggerMode) |
            (entry.mask << IOAPICRedirectionBitsLowMask)
        );
        volatile uint32_t high = (
            (entry.destination << IOAPICRedirectionBitsHighDestination)
        );
        
        ioapicWriteRegister(apicPtr, IOAPICRedirectionTable + 2 * index, low);
        ioapicWriteRegister(apicPtr, IOAPICRedirectionTable + 2 * index + 1, high);
    }

    void SetCommandIPI(uint32_t commandLow, uint32_t commandHigh){
        void* lapicAddress = GetLAPICAddress();
        localAPICWriteRegister(LocalAPICRegisterOffsetErrorStatus, 0);
        localAPICWriteRegister(lapicAddress, LocalAPICRegisterOffsetInterruptCommand + 0x10, commandHigh);
        localAPICWriteRegister(lapicAddress, LocalAPICRegisterOffsetInterruptCommand, commandLow);
        do { __asm__ __volatile__ ("pause" : : : "memory"); }while(localAPICReadRegister(LocalAPICRegisterOffsetInterruptCommand) & (1 << 12));
    }

    void GenerateInterruption(uint8_t CoreID, uint8_t Vector){
        LocalAPICIipi registerInterrupt;
        registerInterrupt.vector = Vector;
        registerInterrupt.deliveryMode = LocalAPICDeliveryModeFixed;
        registerInterrupt.destinationMode = LocalAPICDestinationModePhysicalDestination;
        registerInterrupt.destinationType = LocalAPICDestinationTypeBase;
        uint32_t commandLow = CreatLocalAPICIipiRegister(registerInterrupt);
        uint32_t commandHigh = CoreID << 24;
        
        SetCommandIPI(commandLow, commandHigh);
    }

    uint32_t CreatLocalAPICIipiRegister(LocalAPICIipi reg){
        return (
            (reg.vector << LocalAPICInterruptipiVector) |
            (reg.deliveryMode << LocalAPICInterruptipiMessageType) |
            (reg.destinationMode << LocalAPICInterruptipiDestinationMode) |
            (reg.destinationType << LocalAPICInterruptipiDestinationType)
        );
    }
}