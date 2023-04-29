#include <kot/uisd/srvs/storage/device.h>
#include <kot/math.h>
#include <stdlib.h>
#include <string.h>

namespace Kot{
    kot_process_t ShareableProcessUISDStorage;

    KResult Srv_StorageInitializeDeviceAccess(struct srv_storage_space_info_t* StorageSpace, struct srv_storage_device_t** StorageDevice){
        uint64_t BufferType = NULL;
        uint64_t BufferSize = NULL;
        
        Sys_GetInfoMemoryField(StorageSpace->BufferRWKey, &BufferType, &BufferSize);
        if(BufferType == MemoryFieldTypeShareSpaceRW){
            struct srv_storage_device_t* Device = (struct srv_storage_device_t*)malloc(sizeof(struct srv_storage_device_t));
            Device->BufferRWBase = GetFreeAlignedSpace(BufferSize);
            Device->BufferRWSize = BufferSize;
            
            Sys_AcceptMemoryField(Sys_GetProcess(), StorageSpace->BufferRWKey, &Device->BufferRWBase);

            memcpy((void*)&Device->SpaceInfo, (void*)StorageSpace, sizeof(struct srv_storage_space_info_t));
            *StorageDevice = Device;

            kot_thread_t CallbackRequestHandlerThread = NULL;
            Sys_CreateThread(Sys_GetProcess(), (uintptr_t)&Srv_CallbackRequestHandler, PriviledgeApp, NULL, &CallbackRequestHandlerThread);
            InitializeThread(CallbackRequestHandlerThread);
            Device->CallbackRequestHandlerThread = MakeShareableThreadToProcess(CallbackRequestHandlerThread, Device->SpaceInfo.DriverProc);

            kot_thread_t CallbackCreateSpaceHandlerThread = NULL;
            Sys_CreateThread(Sys_GetProcess(), (uintptr_t)&Srv_CallbackCreateSpaceHandler, PriviledgeApp, NULL, &CallbackCreateSpaceHandlerThread);
            InitializeThread(CallbackCreateSpaceHandlerThread);
            Device->CallbackCreateSpaceHandlerThread = MakeShareableThreadToProcess(CallbackCreateSpaceHandlerThread, Device->SpaceInfo.DriverProc);

            return KSUCCESS;
        }
        return KFAIL;
    }

    KResult Srv_CallbackCreateSpaceHandler(KResult Status, struct srv_storage_device_callback_t* CallbackData, struct srv_storage_space_info_t* SpaceInfo){
        CallbackData->Data = (uintptr_t)malloc(sizeof(struct srv_storage_space_info_t));
        memcpy((void*)CallbackData->Data, (void*)SpaceInfo, sizeof(struct srv_storage_space_info_t));
        Sys_Unpause(CallbackData->MainThread);
        Sys_Close(KSUCCESS);
    }

    KResult Srv_CallbackRequestHandler(KResult Status, struct srv_storage_device_callback_t* CallbackData, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3){
        CallbackData->Status = Status;
        CallbackData->Data = GP0;
        Sys_Unpause(CallbackData->MainThread);
        Sys_Close(KSUCCESS);
    }

    KResult Srv_SendRequest(struct srv_storage_device_t* StorageDevice, uint64_t Start, size64_t Size, bool IsWrite){
        struct srv_storage_device_callback_t* callbackData = (struct srv_storage_device_callback_t*)malloc(sizeof(struct srv_storage_device_callback_t));
        callbackData->MainThread = Sys_Getthread();

        struct kot_arguments_t Parameters;
        Parameters.arg[0] = StorageDevice->CallbackRequestHandlerThread;
        Parameters.arg[1] = (uint64_t)callbackData;
        Parameters.arg[2] = STORAGE_SINGLE_REQUEST;
        Parameters.arg[3] = Start;
        Parameters.arg[4] = Size;
        Parameters.arg[5] = IsWrite;
        Sys_ExecThread(StorageDevice->SpaceInfo.RequestToDeviceThread, &Parameters, ExecutionTypeQueu, NULL);
        Sys_Pause(false);
        return KSUCCESS;
    }

    struct srv_storage_device_callback_t* Srv_SendMultipleRequests(struct srv_storage_device_t* StorageDevice, srv_storage_multiple_requests_t* Requests){
        if(!ShareableProcessUISDStorage){
            ShareableProcessUISDStorage = ShareProcessKey(Sys_GetProcess());
        }
        
        struct srv_storage_device_callback_t* callbackData = (struct srv_storage_device_callback_t*)malloc(sizeof(struct srv_storage_device_callback_t));
        callbackData->MainThread = Sys_Getthread();

        struct kot_arguments_t Parameters;
        Parameters.arg[0] = StorageDevice->CallbackRequestHandlerThread;
        Parameters.arg[1] = (uint64_t)callbackData;
        Parameters.arg[2] = STORAGE_MULTIPLE_REQUESTS;
        Parameters.arg[4] = ShareableProcessUISDStorage;

        struct ShareDataWithArguments_t Data;
        Data.Size = Requests->RequestsCount * sizeof(srv_storage_request_t) + sizeof(srv_storage_multiple_requests_t);
        Data.Data = (uintptr_t)Requests;
        Data.ParameterPosition = 0x3;

        atomicAcquire(&StorageDevice->Lock, 0);
        Sys_ExecThread(StorageDevice->SpaceInfo.RequestToDeviceThread, &Parameters, ExecutionTypeQueu, &Data);
        Sys_Pause(false);
        atomicUnlock(&StorageDevice->Lock, 0);
        return callbackData;
    }

    uint64_t Srv_GetBufferStartingAddress(struct srv_storage_device_t* StorageDevice, uint64_t Start){
        return (uint64_t)StorageDevice->BufferRWBase + (Start % StorageDevice->SpaceInfo.BufferRWAlignement);
    }

    KResult Srv_ReadDevice(struct srv_storage_device_t* StorageDevice, uintptr_t Buffer, uint64_t Start, size64_t Size){
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
            Srv_SendRequest(StorageDevice, StartInIteration, SizeToReadInIteration, false);
            memcpy((void*)AddressDst, (void*)Srv_GetBufferStartingAddress(StorageDevice, StartInIteration), SizeToReadInIteration);
            atomicUnlock(&StorageDevice->Lock, 0);

            StartInIteration += SizeToReadInIteration;
            AddressDst += SizeToReadInIteration;
            SizeToRead -= SizeToReadInIteration;
        }
        return KSUCCESS;
    }

    KResult Srv_WriteDevice(struct srv_storage_device_t* StorageDevice, uintptr_t Buffer, uint64_t Start, size64_t Size){
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
            memcpy((void*)Srv_GetBufferStartingAddress(StorageDevice, StartInIteration), (void*)AddressDst, SizeToWriteInIteration);
            Srv_SendRequest(StorageDevice, StartInIteration, SizeToWriteInIteration, true);
            atomicUnlock(&StorageDevice->Lock, 0);

            StartInIteration += SizeToWriteInIteration;
            AddressDst += SizeToWriteInIteration;
            SizeToWrite -= SizeToWriteInIteration;
        }
        return KSUCCESS;
    }

}