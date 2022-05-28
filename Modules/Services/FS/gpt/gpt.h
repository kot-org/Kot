#pragma once

#include "../../lib/types.h"
#include "../../lib/lib.h"
#include "../../lib/stdio.h"
#include "../../lib/hash/hash.h"
#include "../../memory/heap/heap.h"
#include "../../drivers/ahci/ahci.h"


namespace GPT{
    #define MaxParitionsNumber 128

    struct GPTHeader{
        uint8_t Signature[8];
        uint8_t Revision[4];
        uint32_t HeaderSize;
        uint32_t CRC32;
        uint32_t Reserved;
        uint64_t CurrentLBA;
        uint64_t BackupLBA;
        uint64_t FirstUsableLBAPartitions;
        uint64_t LastUsableLBAPartitions;
        GUID DiskGUID;
        uint64_t PartitionEntriesStartingLBA;
        uint32_t NumberPartitionEntries;
        uint32_t SizePartitionEntry;
        uint32_t CRC32PartitionArray;
    }__attribute__((packed));

    struct GUIDPartitionEntryFormat{
        GUID PartitionTypeGUID;
        GUID UniquePartitionGUID;
        uint64_t FirstLBA;
        uint64_t LastLBA;
        uint64_t Flags;
        uint16_t PartitionName[36];
    }__attribute__((packed));


    struct Partitions{
        GUIDPartitionEntryFormat* AllParitions[MaxParitionsNumber];
        uint64_t NumberPartitionsCreated;
        BitmapHeap IsPartitionsEntryBitmapFree;
    }__attribute__((packed));

    struct PartitionsInfo{
        AHCI::Port* Port;
        GUIDPartitionEntryFormat* Partition;    
        uint64_t PatitionNumber;    
    }__attribute__((packed));

    extern PartitionsInfo** AllPartitionsInfo;
    extern uint64_t AllPartitionsInfoNumber;

    
    GPTHeader* GetGPTHeader(AHCI::Port* port);
    bool SetGPTHeader(AHCI::Port* port, GPTHeader* newGPTHeader);
    void InitGPTHeader(AHCI::Port* port);
    
    

    GUIDPartitionEntryFormat* GetGUIDPartitionEntryFormat(AHCI::Port* port, uint64_t LBAAddress, uint8_t Which);
    bool SetGUIDPartitionEntryFormat(AHCI::Port* port, uint64_t LBAAddress, GUIDPartitionEntryFormat* newGuidPartitionEntryFormat, GPTHeader* GptHeader);

    uint64_t GetFirstFreeLBA(AHCI::Port* port);

    Partitions* GetAllPartitions(AHCI::Port* port);
    bool CreatPartition(AHCI::Port* port, size_t size, char* PartitionName, GUID* PartitionTypeGUID, uint64_t flags);

    uint64_t GetFreeSizePatition(AHCI::Port* port);

    GUID* GetReservedGUIDPartitionType();
    GUID* GetDataGUIDPartitionType();
    GUID* GetSystemGUIDPartitionType();

    GUIDPartitionEntryFormat* GetPartitionByGUID(AHCI::Port* port, GUID* guid);
    
    void AssignNamePartitonsGUID();

    

    class Partition{
        public:
            Partition(AHCI::Port* port, GUIDPartitionEntryFormat* partition);
            bool Read(uint64_t firstByte, size_t size, uintptr_t buffer);
            bool Write(uint64_t firstByte, size_t size, uintptr_t buffer);            
            GUIDPartitionEntryFormat* partition;
            AHCI::Port* port;
    };    
}