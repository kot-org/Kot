#pragma once
#include <stdint.h>

struct RSDP2 {
    unsigned char Signature[8];
    uint8_t Checksum;
    uint8_t ID[6];
    uint8_t Revision;
    uint32_t RSDTAddress;
    uint32_t Lenght;
    uint64_t XSDTAddress;
    uint8_t ExtendedChecksum;
    uint8_t Reserved[3];
} __attribute__((packed));

struct SDT {
    unsigned char Signature[4];
    uint32_t Lenght;
    uint8_t Revision;
    uint8_t Checksum;
    uint8_t ID[6];
    uint8_t TableID[8];
    uint32_t OEMRevision;
    uint32_t CreatorID;
    uint32_t CreatorRevision;
} __attribute__((packed));

struct MCFG
{
   SDT Header;
   uint64_t Reserved;
} __attribute__((packed));

struct DeviceConfig
{
    uint64_t BaseAddress;
    uint16_t PCISegmentGroup;
    uint8_t StartBus;
    uint8_t EndBus;
    uint32_t Reserved;
}__attribute__((packed));


class ACPI
{
public:
    void* FindTable(SDT* sdt, char* sign); 
};


