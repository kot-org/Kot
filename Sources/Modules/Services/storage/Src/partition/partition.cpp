#include <partition/partition.h>

kot_vector_t* PartitionsList;
kot_vector_t* PartitionsListNotify;

uint64_t PartitionLock = NULL;

void InitializePartition(){
    PartitionsList = kot_vector_create();
    PartitionsListNotify = kot_vector_create();
}

partition_t* NewPartition(storage_device_t* Device, uint64_t Start, uint64_t Size, kot_GUID_t* PartitionTypeGUID){
    atomicAcquire(&PartitionLock, 0);
    partition_t* Self = (partition_t*)malloc(sizeof(partition_t));
    Self->Start = Start;
    Self->Size = Size;
    
    memcpy(&Self->PartitionTypeGUID, PartitionTypeGUID, sizeof(kot_GUID_t)); // Copy GUID

    Self->Device = Device;
    
    Self->IsMount = false;
    Self->Index = kot_vector_push(PartitionsList, Self);

    Self->SpaceList = kot_vector_create();

    for(uint64_t i = 0; i < PartitionsListNotify->length; i++){
        notify_info_t* NotifyInfo = (notify_info_t*)kot_vector_get(PartitionsListNotify, i);
        if(memcmp(&Self->PartitionTypeGUID, NotifyInfo->GUIDTarget, sizeof(kot_GUID_t))){
            kot_srv_storage_space_info_t* Space;
            Self->Device->CreateSpace(Start, Size, &Space);
            kot_vector_push(Self->SpaceList, Space);

            kot_thread_t VFSMountThread;
            kot_thread_t VFSMountThreadPrivate;

            kot_Sys_CreateThread(kot_Sys_GetProcess(), (void*)&VFSMount, PriviledgeDriver, (uint64_t)Self, &VFSMountThreadPrivate);
            VFSMountThread = kot_MakeShareableThreadToProcess(VFSMountThreadPrivate, NotifyInfo->ProcessToNotify);

            kot_arguments_t Parameters{
                .arg[0] = i,
                .arg[2] = VFSProcess,
                .arg[3] = VFSMountThread,
            };
            kot_srv_storage_space_info_t SpaceInfo;

            memcpy(&SpaceInfo, Space, sizeof(kot_srv_storage_space_info_t));

            SpaceInfo.CreateProtectedDeviceSpaceThread = kot_MakeShareableThreadToProcess(Space->CreateProtectedDeviceSpaceThread, NotifyInfo->ProcessToNotify);
            SpaceInfo.RequestToDeviceThread = kot_MakeShareableThreadToProcess(Space->RequestToDeviceThread, NotifyInfo->ProcessToNotify);

            kot_ShareDataWithArguments_t Data{
                .Data = &SpaceInfo,
                .Size = sizeof(kot_srv_storage_space_info_t),
                .ParameterPosition = 0x1,
            };

            kot_Sys_ExecThread(NotifyInfo->ThreadToNotify, &Parameters, ExecutionTypeQueu, &Data);
        }
    }
    atomicUnlock(&PartitionLock, 0);
    return Self;
}

partition_t* GetPartition(uint64_t Index){
    atomicAcquire(&PartitionLock, 0);
    partition_t* Partition = (partition_t*)kot_vector_get(PartitionsList, Index);
    atomicUnlock(&PartitionLock, 0);
    return Partition;
}

uint64_t NotifyOnNewPartitionByGUIDType(kot_GUID_t* GUIDTarget, kot_thread_t ThreadToNotify, kot_process_t ProcessToNotify){
    if(GUIDTarget != NULL){
        atomicAcquire(&PartitionLock, 0);
        for(uint64_t i = 0; i < PartitionsListNotify->length; i++){
            notify_info_t* NotifyInfo = (notify_info_t*)kot_vector_get(PartitionsListNotify, i);
            if(memcmp(&GUIDTarget, NotifyInfo->GUIDTarget, sizeof(kot_GUID_t))){
                atomicUnlock(&PartitionLock, 0);
                return KFAIL;
            }
        }
    
        notify_info_t* NotifyInfo = (notify_info_t*)malloc(sizeof(notify_info_t));
        NotifyInfo->GUIDTarget = GUIDTarget;
        NotifyInfo->ThreadToNotify = ThreadToNotify;
        NotifyInfo->ProcessToNotify = ProcessToNotify;
        kot_vector_push(PartitionsListNotify, NotifyInfo);

        for(uint64_t i = 1; i < PartitionsList->length; i++){
            partition_t* Partition = (partition_t*)kot_vector_get(PartitionsList, i);
            if(memcmp(&Partition->PartitionTypeGUID, NotifyInfo->GUIDTarget, sizeof(kot_GUID_t))){
                kot_srv_storage_space_info_t* Space;
                Partition->Device->CreateSpace(Partition->Start, Partition->Size, &Space);
                kot_vector_push(Partition->SpaceList, Space);

                kot_thread_t VFSMountThread;
                kot_thread_t VFSMountThreadPrivate;

                kot_Sys_CreateThread(kot_Sys_GetProcess(), (void*)&VFSMount, PriviledgeDriver, (uint64_t)Partition, &VFSMountThreadPrivate);
                VFSMountThread = kot_MakeShareableThreadToProcess(VFSMountThreadPrivate, NotifyInfo->ProcessToNotify);

                kot_arguments_t Parameters{
                    .arg[0] = i,
                    .arg[2] = VFSProcess,
                    .arg[3] = VFSMountThread,
                };
                kot_srv_storage_space_info_t SpaceInfo;
                memcpy(&SpaceInfo, Space, sizeof(kot_srv_storage_space_info_t));

                SpaceInfo.CreateProtectedDeviceSpaceThread = kot_MakeShareableThreadToProcess(Space->CreateProtectedDeviceSpaceThread, NotifyInfo->ProcessToNotify);
                SpaceInfo.RequestToDeviceThread = kot_MakeShareableThreadToProcess(Space->RequestToDeviceThread, NotifyInfo->ProcessToNotify);

                kot_ShareDataWithArguments_t Data{
                    .Data = &SpaceInfo,
                    .Size = sizeof(kot_srv_storage_space_info_t),
                    .ParameterPosition = 0x1,
                };

                kot_Sys_ExecThread(NotifyInfo->ThreadToNotify, &Parameters, ExecutionTypeQueu, &Data);
            }
        }
        atomicUnlock(&PartitionLock, 0);
    }

    return KSUCCESS;
}

KResult MountPartition(uint64_t PartitonID){
    KResult Status = KFAIL;

    atomicAcquire(&PartitionLock, 0);
    if(PartitonID < PartitionsList->length){
        partition_t* Partition = (partition_t*)kot_vector_get(PartitionsList, PartitonID);
        if(!Partition->IsMount){
            Partition->StaticVolumeMountPoint = Partition->Index;
            Partition->IsMount = true;
            Status = KSUCCESS;
            ExecuteSystemAction(PartitonID);
        }
    }
    atomicUnlock(&PartitionLock, 0);
    return Status;
}

KResult UnmountPartition(uint64_t PartitonID){
    KResult Status = KFAIL;

    atomicAcquire(&PartitionLock, 0);
    if(PartitonID < PartitionsList->length){
        partition_t* Partition = (partition_t*)kot_vector_get(PartitionsList, PartitonID);
        if(Partition->IsMount){
            Partition->IsMount = false;
            Status = KSUCCESS;
        }
    }
    atomicUnlock(&PartitionLock, 0);
    return Status;
}

void LoadPartitionSystem(storage_device_t* Device){
    device_partitions_t* Partitons = (device_partitions_t*)calloc(sizeof(device_partitions_t), sizeof(device_partitions_t));
    Partitons->Device = Device;
    Partitons->LoadPartitions();
}