#pragma once
#include <lib/types.h>

namespace ACPI{
    struct RSDP2 {
        unsigned char Signature[8];
        uint8_t Checksum;
        uint8_t OEMID[6];
        uint8_t Revision;
        uint32_t RSDTAddress;
        uint32_t Length;
        uint64_t XSDTAddress;
        uint8_t ExtendedChecksum;
        uint8_t Reserved[3];
    } __attribute__((packed));

    struct SDTHeader{
        unsigned char Signature[4];
        uint32_t Length;
        uint8_t Revision;
        uint8_t Checksum;
        uint8_t OEMID[6];
        uint8_t OEMTableID[8];
        uint32_t OEMRevision;
        uint32_t CreatorID;
        uint32_t CreatorRevision;
    }__attribute__((packed));

    struct GenericAddressStructure{
        uint8_t AddressSpace;       // 0 - system memory, 1 - system I/O
        uint8_t BitWidth;
        uint8_t BitOffset;
        uint8_t AccessSize;
        uint64_t Address;
    }__attribute__((packed));

    struct MCFGHeader{
        SDTHeader Header;
        uint64_t Reserved;
    }__attribute__((packed));

    struct MADTHeader{
        SDTHeader   Header;
        uint32_t    APICAddress;
        uint32_t    Flags;
    }__attribute__((packed));


    struct HPETHeader{
        SDTHeader Header;

        uint8_t RevisionID;
        uint8_t ComparatorCount:5;
        uint8_t CounterSize:1;
        uint8_t Reserved:1;
        uint8_t LegacyReplacement:1;
        uint16_t VendorID;     
           
        GenericAddressStructure Address;

        uint8_t HPETNumber;
        uint16_t MinimumTick;
        uint8_t PageProtection;
    }__attribute__((packed));

    struct DeviceConfig{
        uint64_t BaseAddress;
        uint16_t PCISegGroup;
        uint8_t StartBus;
        uint8_t EndBus;
        uint32_t Reserved;
    }__attribute__((packed));

    void* FindTable(SDTHeader* sdtHeader, char* signature);
}