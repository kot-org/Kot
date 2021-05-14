#include "apic.h"

namespace APIC{
    LocalProcessor* Processor[MAX_PROCESSORS];
    size_t ProcessorCount = 0;

    void InitializeMADT(ACPI::MADTHeader* madt){
        if(madt == 0){
            return;
        }

        globalGraphics->Update();
        for(size_t i = 0; i < madt->Header.Length - sizeof(ACPI::MADTHeader);){
            EntryRecord* entryRecord = (EntryRecord*)((uint64_t)madt + sizeof(ACPI::MADTHeader) + i);
            globalPageTableManager.MapMemory(entryRecord,entryRecord);
            i += entryRecord->Length;
            globalGraphics->Update();
            switch(entryRecord->Type){
                case EntryTypeLocalProcessor: {
                    LocalProcessor* processor = (LocalProcessor*)entryRecord;
                    Processor[ProcessorCount] = processor;
                    ProcessorCount++;
                    printf("Processor ID: %d APIC-ID: %d Flags: %d\n", processor->ProcessorID, processor->APICID, processor->Flags);
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
                    break;  
                }                                      
            }
        }
    }    
}
