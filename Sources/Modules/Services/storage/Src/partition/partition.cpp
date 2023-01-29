#include <partition/partition.h>

vector_t* PartitionsList;
vector_t* PartitionsListNotify;

uint64_t PartitionLock = NULL;

void InitializePartition(){
    PartitionsList = vector_create();
    PartitionsListNotify = vector_create();
}

partition_t* NewPartition(storage_device_t* Device, uint64_t Start, uint64_t Size, GUID_t* PartitionTypeGUID){
    atomicAcquire(&PartitionLock, 0);
    partition_t* Self = (partition_t*)malloc(sizeof(partition_t));
    Self->Start = Start;
    Self->Size = Size;
    
    memcpy(&Self->PartitionTypeGUID, PartitionTypeGUID, sizeof(GUID_t)); // Copy GUID

    Self->Device = Device;
    
    Self->IsMount = false;
    Self->Index = vector_push(PartitionsList, Self);

    Self->SpaceList = vector_create();

    for(uint64_t i = 0; i < PartitionsListNotify->length; i++){
        notify_info_t* NotifyInfo = (notify_info_t*)vector_get(PartitionsListNotify, i);
        if(memcmp(&Self->PartitionTypeGUID, NotifyInfo->GUIDTarget, sizeof(GUID_t))){
            srv_storage_space_info_t* Space;
            Self->Device->CreateSpace(Start, Size, &Space);
            vector_push(Self->SpaceList, Space);

            thread_t VFSMountThread;
            thread_t VFSMountThreadPrivate;

            Sys_Createthread(Sys_GetProcess(), (uintptr_t)&VFSMount, PriviledgeDriver, (uint64_t)Self, &VFSMountThreadPrivate);
            VFSMountThread = MakeShareableThreadToProcess(VFSMountThreadPrivate, NotifyInfo->ProcessToNotify);

            arguments_t Parameters{
                .arg[0] = i,
                .arg[2] = VFSProcess,
                .arg[3] = VFSMountThread,
            };
            srv_storage_space_info_t SpaceInfo;

            memcpy(&SpaceInfo, Space, sizeof(srv_storage_space_info_t));

            SpaceInfo.CreateProtectedDeviceSpaceThread = MakeShareableThreadToProcess(Space->CreateProtectedDeviceSpaceThread, NotifyInfo->ProcessToNotify);
            SpaceInfo.RequestToDeviceThread = MakeShareableThreadToProcess(Space->RequestToDeviceThread, NotifyInfo->ProcessToNotify);

            ShareDataWithArguments_t Data{
                .Data = &SpaceInfo,
                .Size = sizeof(srv_storage_space_info_t),
                .ParameterPosition = 0x1,
            };

            Sys_Execthread(NotifyInfo->ThreadToNotify, &Parameters, ExecutionTypeQueu, &Data);
        }
    }
    atomicUnlock(&PartitionLock, 0);
    return Self;
}

partition_t* GetPartition(uint64_t Index){
    atomicAcquire(&PartitionLock, 0);
    partition_t* Partition = (partition_t*)vector_get(PartitionsList, Index);
    atomicUnlock(&PartitionLock, 0);
    return Partition;
}

uint64_t NotifyOnNewPartitionByGUIDType(GUID_t* GUIDTarget, thread_t ThreadToNotify, process_t ProcessToNotify){
    if(GUIDTarget != NULL){
        atomicAcquire(&PartitionLock, 0);
        for(uint64_t i = 0; i < PartitionsListNotify->length; i++){
            notify_info_t* NotifyInfo = (notify_info_t*)vector_get(PartitionsListNotify, i);
            if(memcmp(&GUIDTarget, NotifyInfo->GUIDTarget, sizeof(GUID_t))){
                atomicUnlock(&PartitionLock, 0);
                return KFAIL;
            }
        }
    
        notify_info_t* NotifyInfo = (notify_info_t*)malloc(sizeof(notify_info_t));
        NotifyInfo->GUIDTarget = GUIDTarget;
        NotifyInfo->ThreadToNotify = ThreadToNotify;
        NotifyInfo->ProcessToNotify = ProcessToNotify;
        vector_push(PartitionsListNotify, NotifyInfo);

        for(uint64_t i = 1; i < PartitionsList->length; i++){
            partition_t* Partition = (partition_t*)vector_get(PartitionsList, i);
            if(memcmp(&Partition->PartitionTypeGUID, NotifyInfo->GUIDTarget, sizeof(GUID_t))){
                srv_storage_space_info_t* Space;
                Partition->Device->CreateSpace(Partition->Start, Partition->Size, &Space);
                vector_push(Partition->SpaceList, Space);

                thread_t VFSMountThread;
                thread_t VFSMountThreadPrivate;

                Sys_Createthread(Sys_GetProcess(), (uintptr_t)&VFSMount, PriviledgeDriver, (uint64_t)Partition, &VFSMountThreadPrivate);
                VFSMountThread = MakeShareableThreadToProcess(VFSMountThreadPrivate, NotifyInfo->ProcessToNotify);

                arguments_t Parameters{
                    .arg[0] = i,
                    .arg[2] = VFSProcess,
                    .arg[3] = VFSMountThread,
                };
                srv_storage_space_info_t SpaceInfo;
                memcpy(&SpaceInfo, Space, sizeof(srv_storage_space_info_t));

                SpaceInfo.CreateProtectedDeviceSpaceThread = MakeShareableThreadToProcess(Space->CreateProtectedDeviceSpaceThread, NotifyInfo->ProcessToNotify);
                SpaceInfo.RequestToDeviceThread = MakeShareableThreadToProcess(Space->RequestToDeviceThread, NotifyInfo->ProcessToNotify);

                ShareDataWithArguments_t Data{
                    .Data = &SpaceInfo,
                    .Size = sizeof(srv_storage_space_info_t),
                    .ParameterPosition = 0x1,
                };

                Sys_Execthread(NotifyInfo->ThreadToNotify, &Parameters, ExecutionTypeQueu, &Data);
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
        partition_t* Partition = (partition_t*)vector_get(PartitionsList, PartitonID);
        if(!Partition->IsMount){
            Partition->StaticVolumeMountPoint = Partition->Index;
            Partition->IsMount = true;
            Status = KSUCCESS;
        }
    }
    atomicUnlock(&PartitionLock, 0);
    return Status;
}

KResult UnmountPartition(uint64_t PartitonID){
    KResult Status = KFAIL;

    atomicAcquire(&PartitionLock, 0);
    if(PartitonID < PartitionsList->length){
        partition_t* Partition = (partition_t*)vector_get(PartitionsList, PartitonID);
        if(Partition->IsMount){
            Partition->IsMount = false;
            Status = KSUCCESS;
        }
    }
    atomicUnlock(&PartitionLock, 0);
    return Status;
}

void LoadPartitionSystem(storage_device_t* Device){
    device_partitions_t* Partitons = (device_partitions_t*)calloc(sizeof(device_partitions_t));
    Partitons->Device = Device;
    Partitons->LoadPartitions();
}