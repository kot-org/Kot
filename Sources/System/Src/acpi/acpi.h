#pragma once

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <kot/types.h>
#include <kot/memory.h>
#include <kot/uisd/srvs/system.h>

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

struct RSDT{
    struct kot_srv_system_sdtheader_t header;
    uint32_t SDTPointer[];
}__attribute__((packed));

struct XSDT{
    struct kot_srv_system_sdtheader_t header;
    uint64_t SDTPointer[];
}__attribute__((packed));

void* ParseRSDP(void* rsdpPhysical);

uint64_t FindTableIndex(char* signature);
void* GetTablePhysicalAddress(uint64_t index);
size64_t GetTableSize(uint64_t index);