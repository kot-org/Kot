#include <device/device.h>

vector_t* DeviceList;

KResult InitializeDeviceHandling(){
    DeviceList = vector_create();
    return KSUCCESS;
}

// no need to verify if devicelist is create because this function can't be call before so we'll call InitializeDeviceHandling before initialising the server
KResult AddDevice(srv_storage_device_info_t* Info, uint64_t* DeviceIndex){
    uint64_t BufferType = NULL;
    uint64_t BufferSize = NULL;

    Sys_GetInfoMemoryField(Info->BufferRWKey, &BufferType, &BufferSize);
    if(BufferType == MemoryFieldTypeShareSpaceRW){
        storage_device_t* Device = (storage_device_t*)malloc(sizeof(storage_device_t));
        Device->BufferRWBase = getFreeAlignedSpace(BufferSize);
        Device->BufferRWSize = BufferSize;
        
        Sys_AcceptMemoryField(Sys_GetProcess(), Info->BufferRWKey, &Device->BufferRWBase);

        memcpy(&Device->Info, Info, sizeof(srv_storage_device_info_t));
        *DeviceIndex = vector_push(DeviceList, Device);
        return KSUCCESS;
    }
    return KFAIL;
}

KResult RemoveDevice(uint64_t Index){
    storage_device_t* Device = (storage_device_t*)vector_get(DeviceList, Index);
    Sys_CloseMemoryField(Sys_GetProcess(), Device->Info.BufferRWKey, Device->BufferRWBase);
    vector_remove(DeviceList, Index);
    return KSUCCESS;
}

KResult storage_device_t::ReadDevice(uint64_t Start, size64_t Size){
    uint64_t RequestNum = DivideRoundUp(Size, Info.BufferRWUsableSize);
    for(uint64_t i = 0; i < RequestNum; i++){
        
    }
}

KResult storage_device_t::WriteDevice(uint64_t Start, size64_t Size){

}