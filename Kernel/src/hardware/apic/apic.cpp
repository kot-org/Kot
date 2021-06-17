#include "apic.h"

namespace APIC{
    LocalProcessor* Processor[MAX_PROCESSORS];
    size_t ProcessorCount = 0;

    void InitializeMADT(ACPI::MADTHeader* madt){
        if(madt == 0){
            return;
        }

        uint64_t entries = (madt->Header.Length - sizeof(ACPI::MADTHeader));
        
        for(uint64_t i = 0; i < entries;){

            EntryRecord* entryRecord = (EntryRecord*)((uint64_t)madt + sizeof(ACPI::MADTHeader) + i);
            globalPageTableManager.MapMemory(entryRecord, entryRecord);
            i += entryRecord->Length;

            switch(entryRecord->Type){
                case EntryTypeLocalProcessor: {
                    LocalProcessor* processor = (LocalProcessor*)entryRecord;
                    Processor[ProcessorCount] = processor;
                    ProcessorCount++;
                    printf("Processor ID: %u APIC-ID: %u Flags: %u\n", processor->ProcessorID, processor->APICID, processor->Flags);
                    break;                    
                }
                case EntryTypeIOAPIC:{
                    IOAPIC* ioApic = (IOAPIC*)entryRecord;
                    void* apicPtr = (void*)(uint64_t)ioApic->APICAddress;
                    globalPageTableManager.MapMemory(apicPtr, apicPtr);
                    printf("IO APIC: 0x%x\n", apicPtr);
                    break;
                }                    
                case EntryTypeInterruptSourceOverride:{
                    InterruptSourceOverride* iso = (InterruptSourceOverride*)entryRecord;
					printf("Interrupt Source Override source: 0x%d\n", iso->IRQSource);
                    break;
                }                    
                case EntryTypeNonmaskableinterrupts:{
                    NonMaskableinterrupts* nmi = (NonMaskableinterrupts*)entryRecord;
					printf("Non-Maskable Interrupt ID: %d\n", nmi->ACPIProcessorID);
                    break;
                }                    
                case EntryTypeLocalAPICAddressOverride:{
                    LocalAPICAddressOverride* local_apic_override = (LocalAPICAddressOverride*)entryRecord;
                    break;  
                }                                      
            }
        }
    }   

    void* GetLAPICAddress(){
        void* lapicAddress = (void*)(msr::rdmsr(0x1b) & 0xfffff000);
        globalPageTableManager.MapMemory(lapicAddress, lapicAddress);
        return lapicAddress;
    }

    void EnableAPIC(){
        void* lapicAddress = GetLAPICAddress(); 
        msr::wrmsr(0x1b, (uint64_t)lapicAddress);
        localAPICWriteRegister(0xF0, localAPICReadRegister(0xF0) | 0x100);
    }

    void StartLapicTimer(){
        // Setup Local APIC timer
        localAPICWriteRegister(LocalAPICRegisterOffsetInitialCount, 0x100000);
        uint32_t divide = localAPICReadRegister(LocalAPICRegisterOffsetDivide);
        localAPICWriteRegister(LocalAPICRegisterOffsetDivide, (divide & 0xfffffff4) | 0b1010);
        uint32_t timer = localAPICReadRegister(LocalAPICRegisterOffsetLVTTimer);


        LocalAPICInterruptRegister TimerRegisters;
        TimerRegisters.vector = 0x30;
        TimerRegisters.mask = LocalAPICInterruptRegisterMaskEnable;
        TimerRegisters.timerMode = LocalAPICInterruptTimerModePeriodic;
        
        localAPICWriteRegister(LocalAPICRegisterOffsetLVTTimer, CreatRegisterValueInterrupts(TimerRegisters) | (timer & 0xfffcef00));   
    }

    void localAPICSetTimerCount(uint32_t value){
        localAPICWriteRegister(LocalAPICRegisterOffsetInitialCount, value);
    }

    void localApicEOI(){
        localAPICWriteRegister(LocalAPICRegisterOffsetEOI, 0);
    }

    /* APIC */

    uint32_t localAPICReadRegister(size_t offset){
        void* lapicAddress = GetLAPICAddress();
	    return *((volatile uint32_t*)((void*)((uint64_t)lapicAddress + offset)));
    }

    uint32_t localAPICReadRegister(void* lapicAddress, size_t offset){
	    return *((volatile uint32_t*)((void*)((uint64_t)lapicAddress + offset)));
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
}