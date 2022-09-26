#pragma once

#include <core/main.h>
#include <partition/partition.h>

#define MBR_Signature 0xAA55

struct MBRPartitionEntry_t{
    uint8_t BootIndicator;
    uint8_t StartHead;
    uint8_t StartSector;
    uint8_t StartTrack;
    uint8_t OSIndicator;
    uint8_t EndHead;
    uint8_t EndSector;
    uint8_t EndTrack;
    uint32_t StartingLBA;
    uint32_t SizeInLBA;
}__attribute__((packed));

struct MBRHeader_t{
    uint8_t BootCode[440];
    uint32_t UniqueMBRDiskSignature;
    uint16_t Unknown;
    MBRPartitionEntry_t PartitionRecord[4];
    uint16_t Signature;
}__attribute__((packed));