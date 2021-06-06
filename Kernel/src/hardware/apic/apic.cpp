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
                    break;  
                }                                      
            }
        }

        uint64_t lapicAddress = msr::rdmsr(0x1b) & 0xfffff000;
        globalPageTableManager.MapMemory((void*)lapicAddress, (void*)lapicAddress);
        memcpy((void*)0x8000, (void*)ap_trampoline, 4096);
        
        for(int i = 0; i < ProcessorCount; i++){
            //init IPI
            WriteAPIC(lapicAddress, 0x280, 0);
            WriteAPIC(lapicAddress, 0x310, ReadAPIC(lapicAddress, 0x310) & 0x00ffffff | (Processor[i]->APICID << 24));
            WriteAPIC(lapicAddress, 0x300, ReadAPIC(lapicAddress, 0x300) & 0xfff00000 | 0x00C500);         
            WaitAPIC(lapicAddress); 

            WriteAPIC(lapicAddress, 0x310, ReadAPIC(lapicAddress, 0x310) & 0x00ffffff | (Processor[i]->APICID << 24));
            WriteAPIC(lapicAddress, 0x300, ReadAPIC(lapicAddress, 0x300) & 0xfff00000 | 0x008500);         
            WaitAPIC(lapicAddress);

            for(int j = 0; j < 2; j++) {
                WriteAPIC(lapicAddress, 0x280, 0);
                WriteAPIC(lapicAddress, 0x310, ReadAPIC(lapicAddress, 0x310) & 0x00ffffff | (Processor[i]->APICID << 24));
                WriteAPIC(lapicAddress, 0x300, ReadAPIC(lapicAddress, 0x300) & 0xfff0f800 | 0x000608);
                WaitAPIC(lapicAddress); 
            }
        }
    } 

    void WaitAPIC(uint64_t APICAddress){
        do { __asm__ __volatile__ ("pause" : : : "memory"); }while(*((volatile uint32_t*)(APICAddress + 0x300)) & (1 << 12)); 
    }

    void WriteAPIC(uint64_t apicPtr, uint32_t offset, uint32_t value){
        *((volatile uint32_t*)((uint64_t)apicPtr + offset)) = value;
    } 

    uint32_t ReadAPIC(uint64_t apicPtr, uint32_t offset){
        return *((volatile uint32_t*)((uint64_t)apicPtr + offset));
    } 
}
