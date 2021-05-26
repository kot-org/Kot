#pragma once
#include <stdint.h>
#include <stddef.h>
#include "../acpi/acpi.h"
#include "../../lib/stdio.h"
#include "../../graphics.h"
#include "../../paging/pageTableManager.h"

namespace APIC{
    #define MAX_PROCESSORS   256
    #define IO_APIC     256


    struct EntryRecord{
        uint8_t Type;
        uint8_t Length;
    } __attribute__((packed));

    //Entry Type 0 : Processor Local APIC
    struct LocalProcessor{
        uint8_t ProcessorID;
        uint8_t APICID;
        uint32_t Flags; //bit 0 = Processor Enabled / bit 1 = Online Capable
    } __attribute__((packed));

    //Entry Type 1 : I/O APIC
    struct IOAPIC{
        uint8_t APICID;
        uint8_t Reserved;
        uint32_t APICAddress;
        uint32_t GlobalSystemInterruptBase;
    } __attribute__((packed));

    //Entry Type 2 : Interrupt Source Override
    struct InterruptSourceOverride{
        uint8_t BusSource;
        uint8_t IRQSource;
        uint32_t GlobalSystemInterrupt;
        uint16_t Flags;
    } __attribute__((packed));

    //Entry Type 4 : Non-maskable interrupts
    struct NonMaskableinterrupts{
        uint8_t ACPIProcessorID; //0xFF means all processors
        uint16_t Flags;
        uint8_t LINT; //0 or 1
    } __attribute__((packed));

    //Entry Type 5 : Local APIC Address Override
    struct LocalAPICAddressOverride{
        uint8_t Reserved;
        uint64_t LocalAPICAddress;
    } __attribute__((packed));

    enum EntryType{
        EntryTypeLocalProcessor = 0,
        EntryTypeIOAPIC = 1,
        EntryTypeInterruptSourceOverride = 2,
        EntryTypeNonmaskableinterrupts = 4,
        EntryTypeLocalAPICAddressOverride = 5,
    };

    void InitializeMADT(ACPI::MADTHeader* madt);
}
