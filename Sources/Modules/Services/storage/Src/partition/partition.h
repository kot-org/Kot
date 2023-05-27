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
    kot_GUID_t PartitionTypeGUID;

    struct kot_srv_storage_fs_server_functions_t FSServerFunctions;

    struct storage_device_t* Device;

    kot_vector_t* SpaceList;

    uint64_t StaticVolumeMountPoint;
    uint64_t DynamicVolumeMountPoint;
};

struct notify_info_t{
    kot_GUID_t GUIDTarget;
    kot_thread_t ThreadToNotify;
    kot_process_t ProcessToNotify;
};


extern kot_vector_t* PartitionsList;

void InitializePartition();
struct partition_t* NewPartition(struct storage_device_t* Device, uint64_t Start, uint64_t Size, kot_GUID_t* PartitionTypeGUID);
struct partition_t* GetPartition(uint64_t Index);
uint64_t NotifyOnNewPartitionByGUIDType(kot_GUID_t* PartitionTypeGUID, kot_thread_t ThreadToNotify, kot_process_t ProcessToNotify);
KResult MountPartition(uint64_t PartitonID);
KResult UnmountPartition(uint64_t PartitonID);

void LoadPartitionSystem(storage_device_t* Device);