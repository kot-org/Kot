#include <device/device.h>

vector_t* DeviceList;

KResult InitializeDeviceHandling(){
    DeviceList = vector_create();

}

// no need to verify if devicelist is create because this function can't be call before so we'll call InitializeDeviceHandling before initialising the server
uint64_t AddDevice(thread_t ReadWriteThread, uint64_t ReadWriteArg, ksmem_t BufferRW, uint64_t BufferRWAlignement, storage_device_info_t* Info){
    uint64_t BufferType = NULL;
    uint64_t BufferSize = NULL;
    Sys_GetInfoMemoryField(BufferRW, &BufferType, &BufferSize);
    if(BufferType == MemoryFieldTypeShareSpaceRW){
        storage_device_t* Device = (storage_device_t*)malloc(sizeof(storage_device_t));
        Device->ReadWriteThread = ReadWriteThread;
        Device->ReadWriteArg = ReadWriteArg;
        Device->BufferRWKey = BufferRW;
        Device->BufferRWBase = getFreeAlignedSpace(BufferSize);
        Device->BufferRWSize = BufferSize;
        Device->BufferRWAlignement = BufferRWAlignement;
        
        Sys_AcceptMemoryField(Sys_GetProcess(), Device->BufferRWKey, &Device->BufferRWBase);

        memcpy(&Device->Info, Info, sizeof(storage_device_info_t));
        return vector_push(DeviceList, Device);
    }
    return NULL;
}

void RemoveDevice(uint64_t Index){
    storage_device_t* Device = (storage_device_t*)vector_get(DeviceList, Index);
    Sys_CloseMemoryField(Sys_GetProcess(), Device->BufferRWKey, Device->BufferRWBase);
    vector_remove(DeviceList, Index);
}
