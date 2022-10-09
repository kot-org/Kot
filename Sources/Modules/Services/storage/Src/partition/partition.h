#pragma once

#include <core/main.h>
#include <partition/mbr/mbr.h>
#include <partition/gpt/gpt.h>
#include <kot/uisd/srvs/storage.h>

struct device_partitions_t{
    // Device
    struct storage_device_t* Device;

    // MBR
    KResult LoadMBRHeader();
    bool IsMBRDisk();
    bool IsGPTDisk();
    bool IsMBRHeaderLoaded;
    struct MBRHeader_t* MBRHeader;

    // GPT
    KResult LoadGPTHeader();
    KResult CheckPartitions();
    KResult LoadPartitions();
    bool IsGPTHeaderLoaded;
    bool IsGPTPartitionsLoaded;
    struct GPTHeader_t* GPTHeader;
    struct GPTPartitionEntry_t* GPTPartitionEntries;
};

struct partition_t{
    uint64_t Index;
    uint64_t Start;
    uint64_t Size;
    struct srv_storage_space_info_t* Space;
};

void InitializePartition();
partition_t* NewPartition(struct storage_device_t* Device, uint64_t Start, uint64_t Size);
partition_t* GetPartition(uint64_t Index);
void MountPartition(partition_t* Self);
void UnmountPartition(partition_t* Self);

void LoadPartitionSystem(storage_device_t* Device);