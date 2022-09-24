#pragma once

#include <core/main.h>
#include <partition/partition.h>

struct LegacyMBRPartitionRecord_t{
    uint8_t BootIndicator;
    uint8_t StartHead;
    uint8_t StartSector;
    uint8_t StartTrack;
    uint8_t OSIndicator;
    uint8_t EndHead;
    uint8_t EndSector;
    uint8_t EndTrack;
    uin32_t StartingLBA;
    uin32_t SizeInLBA;
}__attribute__((packed));

struct LegacyMBR_t{
    uint8_t BootCode[440];
    uint32_t UniqueMBRDiskSignature;
    uint16_t Unknown;
    LegacyMBRPartitionRecord_t PartitionRecord[4];
    uint16_t Signature;
}__attribute__((packed));