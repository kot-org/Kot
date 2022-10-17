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
    bool IsMount;
    uint64_t Start;
    uint64_t Size;
    GUID_t PartitionTypeGUID;

    struct srv_storage_fs_server_functions_t FSServerFunctions;

    storage_device_t* Device;
    struct srv_storage_space_info_t* Space;
};

void InitializePartition();
struct partition_t* NewPartition(struct storage_device_t* Device, uint64_t Start, uint64_t Size, GUID_t* PartitionTypeGUID);
struct partition_t* GetPartition(uint64_t Index);
uint64_t NotifyOnNewPartitionByGUIDType(GUID_t* PartitionTypeGUID);
struct partition_t* GetPartitionByGUIDType_WL(uint64_t Index, GUID_t* PartitionTypeGUID);
KResult MountPartition(uint64_t Index, GUID_t* PartitionTypeGUID, struct srv_storage_fs_server_functions_t* FSServerFunctions);
KResult UnmountPartition(uint64_t Index, GUID_t* PartitionTypeGUID);

void LoadPartitionSystem(storage_device_t* Device);