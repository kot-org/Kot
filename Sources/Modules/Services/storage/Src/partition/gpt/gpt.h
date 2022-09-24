#pragma once

#include <core/main.h>
#include <partition/partition.h>

struct GPTHeader_t{
    uint64_t Signature;
    uint32_t Revision;
    uint32_t HeaderSize;
    uint32_t HeaderCRC32;
    uint32_t Reserved;
    uint64_t MyLBA;
    uint64_t AlternateLBA;
    uint64_t FirstUsableLBA;
    uint64_t LastUsableLBA;
    uint8_t DiskGUID[16];
    uint64_t PartitionEntryLBA;
    uint32_t NumberOfPartitionEntries;
    uint32_t SizeOfPartitionEntry;
    uint32_t PartitionEntryArrayCRC32;
}__attribute__((packed));

struct GPTPartitionEntry_t{
    uint8_t PartitionTypeGUID[16];
    uint8_t UniquePartitionGUID[16];
    uint64_t StartingLBA;
    uint64_t EndingLBA;
    uint64_t Attributes;
    char PartitionName[72];
}__attribute__((packed));