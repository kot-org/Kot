#include <partition/partition.h>

vector_t* partitions;

void InitializePartition(){
    partitions = vector_create();
}

partition_t* NewPartition(storage_device_t* Device, uint64_t Start, uint64_t Size){
    partition_t* Self = (partition_t*)malloc(sizeof(partition_t));
    Self->Start = Start;
    Self->Size = Size;
    Self->Index = vector_push(partitions, Self);

    Printlog("Partition found !!");
    return Self;
}

partition_t* GetPartition(uint64_t Index){
    return (partition_t*)vector_get(partitions, Index);
}

void MountPartition(partition_t* Self){
    vector_remove(partitions, Self->Index);
}

void UnmountPartition(partition_t* Self){
    Self->Index = vector_push(partitions, Self);
}

void LoadPartitionSystem(storage_device_t* Device){
    device_partitions_t* Partitons = (device_partitions_t*)calloc(sizeof(device_partitions_t));
    Partitons->Device = Device;
    Partitons->LoadPartitions();
}