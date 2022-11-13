#include <kot/uisd/srvs/storage/device.h>

KResult Srv_StorageInitializeDeviceAccess(struct srv_storage_space_info_t* StorageSpace, struct srv_storage_device_t** StorageDevice){
    uint64_t BufferType = NULL;
    uint64_t BufferSize = NULL;
    
    Sys_GetInfoMemoryField(StorageSpace->BufferRWKey, &BufferType, &BufferSize);
    if(BufferType == MemoryFieldTypeShareSpaceRW){
        struct srv_storage_device_t* Device = (struct srv_storage_device_t*)malloc(sizeof(struct srv_storage_device_t));
        Device->BufferRWBase = GetFreeAlignedSpace(BufferSize);
        Device->BufferRWSize = BufferSize;
        
        Sys_AcceptMemoryField(Sys_GetProcess(), StorageSpace->BufferRWKey, &Device->BufferRWBase);

        memcpy(&Device->SpaceInfo, StorageSpace, sizeof(struct srv_storage_space_info_t));
        *StorageDevice = Device;

        thread_t CallbackRequestHandlerThread = NULL;
        Sys_Createthread(Sys_GetProcess(), (uintptr_t)&Srv_CallbackRequestHandler, PriviledgeApp, NULL, &CallbackRequestHandlerThread);
        Device->CallbackRequestHandlerThread = MakeShareableThreadToProcess(CallbackRequestHandlerThread, Device->SpaceInfo.DriverProc);

        thread_t CallbackCreateSpaceHandlerThread = NULL;
        Sys_Createthread(Sys_GetProcess(), (uintptr_t)&Srv_CallbackCreateSpaceHandler, PriviledgeApp, NULL, &CallbackCreateSpaceHandlerThread);
        Device->CallbackCreateSpaceHandlerThread = MakeShareableThreadToProcess(CallbackCreateSpaceHandlerThread, Device->SpaceInfo.DriverProc);

        return KSUCCESS;
    }
    return KFAIL;
}

KResult Srv_CallbackCreateSpaceHandler(KResult Statu, struct srv_storage_device_callback_t* CallbackData, struct srv_storage_space_info_t* SpaceInfo){
    CallbackData->Data = malloc(sizeof(struct srv_storage_space_info_t));
    memcpy(CallbackData->Data, SpaceInfo, sizeof(struct srv_storage_space_info_t));
    Sys_Unpause(CallbackData->MainThread);
    Sys_Close(KSUCCESS);
}

KResult Srv_CallbackRequestHandler(KResult Statu, thread_t MainThread){
    Sys_Unpause(MainThread);
    Sys_Close(KSUCCESS);
}

KResult Srv_SendRequest(struct srv_storage_device_t* StorageDevice, uint64_t Start, size64_t Size, bool IsWrite){
    struct srv_storage_device_callback_t* callbackData = (struct srv_storage_device_callback_t*)malloc(sizeof(struct srv_storage_device_callback_t));
    callbackData->MainThread = Sys_Getthread();

    struct arguments_t parameters;
    parameters.arg[0] = StorageDevice->CallbackRequestHandlerThread;
    parameters.arg[1] = Sys_Getthread();
    parameters.arg[2] = Start;
    parameters.arg[3] = Size;
    parameters.arg[4] = IsWrite;
    Sys_Execthread(StorageDevice->SpaceInfo.ReadWriteDeviceThread, &parameters, ExecutionTypeQueu, NULL);
    Sys_Pause(false);
    return KSUCCESS;
}

uint64_t Srv_GetBufferStartingAddress(struct srv_storage_device_t* StorageDevice, uint64_t Start){
    return (uint64_t)StorageDevice->BufferRWBase + (Start % StorageDevice->SpaceInfo.BufferRWAlignement);
}

KResult Srv_ReadDevice(struct srv_storage_device_t* StorageDevice, uintptr_t Buffer, uint64_t Start, size64_t Size){
    uint64_t RequestNum = DivideRoundUp(Size, StorageDevice->SpaceInfo.BufferRWUsableSize);
    uint64_t StartInIteration = Start;
    uint64_t SizeToRead = Size;
    uint64_t AddressDst = (uint64_t)Buffer;
    uint64_t SizeToReadInIteration = SizeToRead;

    for(uint64_t i = 0; i < RequestNum; i++){
        if(SizeToRead > StorageDevice->SpaceInfo.BufferRWUsableSize){
            SizeToReadInIteration = StorageDevice->SpaceInfo.BufferRWUsableSize;
        }else{
            SizeToReadInIteration = SizeToRead;
        }

        atomicAcquire(&StorageDevice->Lock, 0);
        Srv_SendRequest(StorageDevice, StartInIteration, SizeToReadInIteration, false);
        memcpy((uintptr_t)AddressDst, (uintptr_t)Srv_GetBufferStartingAddress(StorageDevice, StartInIteration), SizeToReadInIteration);
        atomicUnlock(&StorageDevice->Lock, 0);

        StartInIteration += SizeToReadInIteration;
        AddressDst += SizeToReadInIteration;
        SizeToRead -= SizeToReadInIteration;
    }
    return KSUCCESS;
}

KResult Srv_WriteDevice(struct srv_storage_device_t* StorageDevice, uintptr_t Buffer, uint64_t Start, size64_t Size){
    uint64_t RequestNum = DivideRoundUp(Size, StorageDevice->SpaceInfo.BufferRWUsableSize);
    uint64_t StartInIteration = Start;
    uint64_t SizeToWrite = Size;
    uint64_t AddressDst = (uint64_t)Buffer;
    uint64_t SizeToWriteInIteration = SizeToWrite;

    for(uint64_t i = 0; i < RequestNum; i++){
        if(SizeToWrite > StorageDevice->SpaceInfo.BufferRWUsableSize){
            SizeToWriteInIteration = StorageDevice->SpaceInfo.BufferRWUsableSize;
        }else{
            SizeToWriteInIteration = SizeToWrite;
        }

        atomicAcquire(&StorageDevice->Lock, 0);
        memcpy((uintptr_t)Srv_GetBufferStartingAddress(StorageDevice, StartInIteration), (uintptr_t)AddressDst, SizeToWriteInIteration);
        Srv_SendRequest(StorageDevice, StartInIteration, SizeToWriteInIteration, true);
        atomicUnlock(&StorageDevice->Lock, 0);

        StartInIteration += SizeToWriteInIteration;
        AddressDst += SizeToWriteInIteration;
        SizeToWrite -= SizeToWriteInIteration;
    }
    return KSUCCESS;
}