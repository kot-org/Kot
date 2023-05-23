#include <kot/uisd/srvs/storage/device.h>
#include <kot/math.h>
#include <stdlib.h>
#include <string.h>

extern "C" {

kot_process_t kot_ShareableProcessUISDStorage;

KResult kot_Srv_StorageInitializeDeviceAccess(struct kot_srv_storage_space_info_t* StorageSpace, struct kot_srv_storage_device_t** StorageDevice){
    uint64_t BufferType = NULL;
    uint64_t BufferSize = NULL;
    
    kot_Sys_GetInfoMemoryField(StorageSpace->BufferRWKey, &BufferType, &BufferSize);
    if(BufferType == MemoryFieldTypeShareSpaceRW){
        struct kot_srv_storage_device_t* Device = (struct kot_srv_storage_device_t*)malloc(sizeof(struct kot_srv_storage_device_t));
        Device->BufferRWBase = kot_GetFreeAlignedSpace(BufferSize);
        Device->BufferRWSize = BufferSize;
        
        kot_Sys_AcceptMemoryField(kot_Sys_GetProcess(), StorageSpace->BufferRWKey, &Device->BufferRWBase);

        memcpy((void*)&Device->SpaceInfo, (void*)StorageSpace, sizeof(struct kot_srv_storage_space_info_t));
        *StorageDevice = Device;

        kot_thread_t CallbackRequestHandlerThread = NULL;
        kot_Sys_CreateThread(kot_Sys_GetProcess(), (void*)&kot_Srv_CallbackRequestHandler, PriviledgeApp, NULL, &CallbackRequestHandlerThread);
        Device->CallbackRequestHandlerThread = kot_MakeShareableThreadToProcess(CallbackRequestHandlerThread, Device->SpaceInfo.DriverProc);

        kot_thread_t CallbackCreateSpaceHandlerThread = NULL;
        kot_Sys_CreateThread(kot_Sys_GetProcess(), (void*)&kot_Srv_CallbackCreateSpaceHandler, PriviledgeApp, NULL, &CallbackCreateSpaceHandlerThread);
        Device->CallbackCreateSpaceHandlerThread = kot_MakeShareableThreadToProcess(CallbackCreateSpaceHandlerThread, Device->SpaceInfo.DriverProc);

        return KSUCCESS;
    }
    return KFAIL;
}

KResult kot_Srv_CallbackCreateSpaceHandler(KResult Status, struct kot_srv_storage_device_callback_t* CallbackData, struct kot_srv_storage_space_info_t* SpaceInfo){
    CallbackData->Data = (void*)malloc(sizeof(struct kot_srv_storage_space_info_t));
    memcpy((void*)CallbackData->Data, (void*)SpaceInfo, sizeof(struct kot_srv_storage_space_info_t));
    kot_Sys_Unpause(CallbackData->MainThread);
    kot_Sys_Close(KSUCCESS);
}

KResult kot_Srv_CallbackRequestHandler(KResult Status, struct kot_srv_storage_device_callback_t* CallbackData, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3){
    CallbackData->Status = Status;
    CallbackData->Data = (void*)GP0;
    kot_Sys_Unpause(CallbackData->MainThread);
    kot_Sys_Close(KSUCCESS);
}

KResult kot_Srv_SendRequest(struct kot_srv_storage_device_t* StorageDevice, uint64_t Start, size64_t Size, bool IsWrite){
    struct kot_srv_storage_device_callback_t* callbackData = (struct kot_srv_storage_device_callback_t*)malloc(sizeof(struct kot_srv_storage_device_callback_t));
    callbackData->MainThread = kot_Sys_GetThread();

    struct kot_arguments_t Parameters;
    Parameters.arg[0] = StorageDevice->CallbackRequestHandlerThread;
    Parameters.arg[1] = (uint64_t)callbackData;
    Parameters.arg[2] = STORAGE_SINGLE_REQUEST;
    Parameters.arg[3] = Start;
    Parameters.arg[4] = Size;
    Parameters.arg[5] = IsWrite;
    kot_Sys_ExecThread(StorageDevice->SpaceInfo.RequestToDeviceThread, &Parameters, ExecutionTypeQueu, NULL);
    kot_Sys_Pause(false);
    return KSUCCESS;
}

struct kot_srv_storage_device_callback_t* kot_Srv_SendMultipleRequests(struct kot_srv_storage_device_t* StorageDevice, kot_srv_storage_multiple_requests_t* Requests){
    if(!kot_ShareableProcessUISDStorage){
        kot_ShareableProcessUISDStorage = kot_ShareProcessKey(kot_Sys_GetProcess());
    }
    
    struct kot_srv_storage_device_callback_t* callbackData = (struct kot_srv_storage_device_callback_t*)malloc(sizeof(struct kot_srv_storage_device_callback_t));
    callbackData->MainThread = kot_Sys_GetThread();

    struct kot_arguments_t Parameters;
    Parameters.arg[0] = StorageDevice->CallbackRequestHandlerThread;
    Parameters.arg[1] = (uint64_t)callbackData;
    Parameters.arg[2] = STORAGE_MULTIPLE_REQUESTS;
    Parameters.arg[4] = kot_ShareableProcessUISDStorage;

    struct kot_ShareDataWithArguments_t Data;
    Data.Size = Requests->RequestsCount * sizeof(kot_srv_storage_request_t) + sizeof(kot_srv_storage_multiple_requests_t);
    Data.Data = (void*)Requests;
    Data.ParameterPosition = 0x3;

    atomicAcquire(&StorageDevice->Lock, 0);
    kot_Sys_ExecThread(StorageDevice->SpaceInfo.RequestToDeviceThread, &Parameters, ExecutionTypeQueu, &Data);
    kot_Sys_Pause(false);
    atomicUnlock(&StorageDevice->Lock, 0);
    return callbackData;
}

uint64_t kot_Srv_GetBufferStartingAddress(struct kot_srv_storage_device_t* StorageDevice, uint64_t Start){
    return (uint64_t)StorageDevice->BufferRWBase + (Start % StorageDevice->SpaceInfo.BufferRWAlignement);
}

KResult kot_Srv_ReadDevice(struct kot_srv_storage_device_t* StorageDevice, void* Buffer, uint64_t Start, size64_t Size){
    uint64_t RequestNum = DIV_ROUND_UP(Size, StorageDevice->SpaceInfo.BufferRWUsableSize);
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
        kot_Srv_SendRequest(StorageDevice, StartInIteration, SizeToReadInIteration, false);
        memcpy((void*)AddressDst, (void*)kot_Srv_GetBufferStartingAddress(StorageDevice, StartInIteration), SizeToReadInIteration);
        atomicUnlock(&StorageDevice->Lock, 0);

        StartInIteration += SizeToReadInIteration;
        AddressDst += SizeToReadInIteration;
        SizeToRead -= SizeToReadInIteration;
    }
    return KSUCCESS;
}

KResult kot_Srv_WriteDevice(struct kot_srv_storage_device_t* StorageDevice, void* Buffer, uint64_t Start, size64_t Size){
    uint64_t RequestNum = DIV_ROUND_UP(Size, StorageDevice->SpaceInfo.BufferRWUsableSize);
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
        memcpy((void*)kot_Srv_GetBufferStartingAddress(StorageDevice, StartInIteration), (void*)AddressDst, SizeToWriteInIteration);
        kot_Srv_SendRequest(StorageDevice, StartInIteration, SizeToWriteInIteration, true);
        atomicUnlock(&StorageDevice->Lock, 0);

        StartInIteration += SizeToWriteInIteration;
        AddressDst += SizeToWriteInIteration;
        SizeToWrite -= SizeToWriteInIteration;
    }
    return KSUCCESS;
}

}