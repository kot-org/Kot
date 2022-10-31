#include <partition/partition.h>

vector_t* PartitionsList;
vector_t* PartitionsListNotify;

uint64_t PartitionLock = NULL;

void InitializePartition(){
    PartitionsList = vector_create();
}

partition_t* NewPartition(storage_device_t* Device, uint64_t Start, uint64_t Size, GUID_t* PartitionTypeGUID){
    atomicAcquire(&PartitionLock, 0);
    partition_t* Self = (partition_t*)malloc(sizeof(partition_t));
    Self->Start = Start;
    Self->Size = Size;
    
    memcpy(&Self->PartitionTypeGUID, PartitionTypeGUID, sizeof(GUID_t)); // Copy GUID

    Self->Device = Device;
    Self->Device->CreateSpace(Start, Size, &Self->Space);
    
    Self->IsMount = false;
    Self->Index = vector_push(PartitionsList, Self);

    for(uint64_t i = 0; i < PartitionsListNotify->length; i++){
        notify_info_t* NotifyInfo = (notify_info_t*)vector_get(PartitionsListNotify, i);
        if(memcmp(&Self->PartitionTypeGUID, NotifyInfo->GUIDTarget, sizeof(GUID_t))){
            arguments_t Parameters{
                .arg[0] = i,
            };
            Sys_Execthread(NotifyInfo->ThreadToNotify, &Parameters, ExecutionTypeQueu, NULL);
        }
    }
    Printlog("Partition");
    atomicUnlock(&PartitionLock, 0);
    return Self;
}

partition_t* GetPartition(uint64_t Index){
    atomicAcquire(&PartitionLock, 0);
    partition_t* Partition = (partition_t*)vector_get(PartitionsList, Index);
    atomicUnlock(&PartitionLock, 0);
    return Partition;
}

uint64_t NotifyOnNewPartitionByGUIDType(GUID_t* GUIDTarget, thread_t ThreadToNotify){
    if(GUIDTarget != NULL){
        atomicAcquire(&PartitionLock, 0);
        notify_info_t* NotifyInfo = (notify_info_t*)malloc(sizeof(notify_info_t));
        NotifyInfo->GUIDTarget = GUIDTarget;
        NotifyInfo->ThreadToNotify = ThreadToNotify;

        for(uint64_t i = 0; i < PartitionsList->length; i++){
            partition_t* Partition = (partition_t*)vector_get(PartitionsList, i);
            if(memcmp(&Partition->PartitionTypeGUID, NotifyInfo->GUIDTarget, sizeof(GUID_t))){
                arguments_t Parameters{
                    .arg[0] = i,
                };
                Sys_Execthread(NotifyInfo->ThreadToNotify, &Parameters, ExecutionTypeQueu, NULL);
            }
        }
        atomicUnlock(&PartitionLock, 0);
    }

    return KSUCCESS;
}

partition_t* GetPartitionByGUIDType_WL(uint64_t Index, GUID_t* PartitionTypeGUID){
    uint64_t Counter = 0;
    if(PartitionTypeGUID != NULL){
        for(uint64_t i = 0; i < PartitionsList->length; i++){
            partition_t* Partition = (partition_t*)vector_get(PartitionsList, i);
            if(memcmp(&Partition->PartitionTypeGUID, PartitionTypeGUID, sizeof(GUID_t))){
                if(Counter == Index){
                    return Partition;
                }
                Counter++;
            }
        }
    }else if(Index < PartitionsList->length){
        partition_t* Partition = (partition_t*)vector_get(PartitionsList, Index);
        return Partition;
    }
    return NULL;
}

KResult MountPartition(uint64_t Index, GUID_t* PartitionTypeGUID, srv_storage_fs_server_functions_t* FSServerFunctions){
    KResult Status = KFAIL;

    atomicAcquire(&PartitionLock, 0);
    partition_t* Partition = GetPartitionByGUIDType_WL(Index, PartitionTypeGUID);
    if(!Partition->IsMount){
        // TODO: check keys
        Partition->FSServerFunctions.Rename = FSServerFunctions->Rename;
        Partition->FSServerFunctions.Remove = FSServerFunctions->Remove;
        Partition->FSServerFunctions.Fopen = FSServerFunctions->Fopen;
        Partition->FSServerFunctions.Mkdir = FSServerFunctions->Mkdir;
        Partition->FSServerFunctions.Readdir = FSServerFunctions->Readdir;
        Partition->FSServerFunctions.Flist = FSServerFunctions->Flist;
        Partition->IsMount = true;
    }
    atomicUnlock(&PartitionLock, 0);
    return Status;
}

KResult UnmountPartition(uint64_t Index, GUID_t* PartitionTypeGUID){
    KResult Status = KFAIL;

    atomicAcquire(&PartitionLock, 0);
    partition_t* Partition = GetPartitionByGUIDType_WL(Index, PartitionTypeGUID);
    if(Partition->IsMount){
        Partition->IsMount = false;
    }
    atomicUnlock(&PartitionLock, 0);
    return Status;
}

void LoadPartitionSystem(storage_device_t* Device){
    device_partitions_t* Partitons = (device_partitions_t*)calloc(sizeof(device_partitions_t));
    Partitons->Device = Device;
    Partitons->LoadPartitions();
}