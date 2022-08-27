#pragma once

#include <kot/types.h>
#include <kot/memory.h>
#include <kot/cstring.h>

struct RSDP2{
    uint8_t Signature[8];
    uint8_t Checksum;
    uint8_t OEMID[6];
    uint8_t Revision;
    uint32_t RSDTAddress;
    uint32_t Length;
    uint64_t XSDTAddress;
    uint8_t ExtendedChecksum;
    uint8_t Reserved[3];
}__attribute__((packed));

struct SDTHeader{
    uint8_t Signature[4];
    uint32_t Length;
    uint8_t Revision;
    uint8_t Checksum;
    uint8_t OEMID[6];
    uint8_t OEMTableID[8];
    uint32_t OEMRevision;
    uint32_t CreatorID;
    uint32_t CreatorRevision;
}__attribute__((packed));

struct RSDT{
    struct SDTHeader header;
    uint32_t SDTPointer[];
}__attribute__((packed));

struct XSDT{
    struct SDTHeader header;
    uint64_t SDTPointer[];
}__attribute__((packed));

uintptr_t ParseRSDP(uintptr_t rsdpPhysical);
uintptr_t FindTable(char* signature);