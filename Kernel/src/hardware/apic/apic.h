#pragma once
#include <stdint.h>
#include <stddef.h>
#include "../../lib/stdio.h"
#include "../acpi/acpi.h"
#include "../../IO/msr/msr.h"
#include "../../IO/IO.h"
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
        EntryRecord Record;
        uint8_t ProcessorID;
        uint8_t APICID;
        uint32_t Flags; //bit 0 = Processor Enabled / bit 1 = Online Capable
    } __attribute__((packed));

    //Entry Type 1 : I/O APIC
    struct IOAPIC{
        EntryRecord Record;
        uint8_t APICID;
        uint8_t Reserved;
        uint32_t APICAddress;
        uint32_t GlobalSystemInterruptBase;
    } __attribute__((packed));

    //Entry Type 2 : Interrupt Source Override
    struct InterruptSourceOverride{
        EntryRecord Record;
        uint8_t BusSource;
        uint8_t IRQSource;
        uint32_t GlobalSystemInterrupt;
        uint16_t Flags;
    } __attribute__((packed));

    //Entry Type 4 : Non-maskable interrupts
    struct NonMaskableinterrupts{
        EntryRecord Record;
        uint8_t ACPIProcessorID; //0xFF means all processors
        uint16_t Flags;
        uint8_t LINT; //0 or 1
    } __attribute__((packed));

    //Entry Type 5 : Local APIC Address Override
    struct LocalAPICAddressOverride{
        EntryRecord Record;
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
    extern "C" void ap_trampoline();
    void WaitAPIC(uint64_t APICAddress);
    void WriteAPIC(uint64_t apicPtr, uint64_t offset, uint64_t value);
    uint32_t ReadAPIC(uint64_t apicPtr, uint64_t offset);

    extern LocalProcessor* Processor[MAX_PROCESSORS];
    extern size_t ProcessorCount;
}
