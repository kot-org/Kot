#include <device/device.h>

KResult AddDevice(kot_srv_storage_device_info_t* Info, storage_device_t** DevicePointer){
    uint64_t BufferType = NULL;
    uint64_t BufferSize = NULL;
    
    kot_Sys_GetInfoMemoryField(Info->MainSpace.BufferRWKey, &BufferType, &BufferSize);
    if(BufferType == MemoryFieldTypeShareSpaceRW){
        storage_device_t* Device = (storage_device_t*)malloc(sizeof(storage_device_t));
        Device->BufferRWBase = kot_GetFreeAlignedSpace(BufferSize);
        Device->BufferRWSize = BufferSize;

        Device->Lock = 0;
        
        kot_Sys_AcceptMemoryField(kot_Sys_GetProcess(), Info->MainSpace.BufferRWKey, &Device->BufferRWBase);

        memcpy(&Device->Info, Info, sizeof(kot_srv_storage_device_info_t));
        *DevicePointer = Device;

        kot_thread_t CallbackRequestHandlerThread = NULL;
        kot_Sys_CreateThread(kot_Sys_GetProcess(), (void*)&CallbackRequestHandler, PriviledgeApp, NULL, &CallbackRequestHandlerThread);
        Device->CallbackRequestHandlerThread = kot_MakeShareableThreadToProcess(CallbackRequestHandlerThread, Info->MainSpace.DriverProc);

        kot_thread_t CallbackCreateSpaceHandlerThread = NULL;
        kot_Sys_CreateThread(kot_Sys_GetProcess(), (void*)&CallbackCreateSpaceHandler, PriviledgeApp, NULL, &CallbackCreateSpaceHandlerThread);
        Device->CallbackCreateSpaceHandlerThread = kot_MakeShareableThreadToProcess(CallbackCreateSpaceHandlerThread, Info->MainSpace.DriverProc);
        LoadPartitionSystem(Device);

        return KSUCCESS;
    }
    return KFAIL;
}

KResult RemoveDevice(storage_device_t* Device){
    kot_Sys_CloseMemoryField(kot_Sys_GetProcess(), Device->Info.MainSpace.BufferRWKey, Device->BufferRWBase);
    return KSUCCESS;
}

KResult CallbackCreateSpaceHandler(KResult Status, storage_callback_t* CallbackData, kot_srv_storage_space_info_t* SpaceInfo){
    CallbackData->Data = (void*)malloc(sizeof(kot_srv_storage_space_info_t));
    memcpy((void*)CallbackData->Data, SpaceInfo, sizeof(kot_srv_storage_space_info_t));
    kot_Sys_Unpause(CallbackData->MainThread);
    kot_Sys_Close(KSUCCESS);
}

KResult storage_device_t::CreateSpace(uint64_t Start, size64_t Size, kot_srv_storage_space_info_t** SpaceInfo){
    storage_callback_t* callbackData = (storage_callback_t*)malloc(sizeof(storage_callback_t));
    callbackData->MainThread = kot_Sys_GetThread();

    kot_arguments_t parameters;
    parameters.arg[0] = CallbackCreateSpaceHandlerThread;
    parameters.arg[1] = (uint64_t)callbackData;
    parameters.arg[2] = Start;
    parameters.arg[3] = Size;
    kot_Sys_ExecThread(Info.MainSpace.CreateProtectedDeviceSpaceThread, &parameters, ExecutionTypeQueu, NULL);
    kot_Sys_Pause(false);
    *SpaceInfo = (kot_srv_storage_space_info_t*)callbackData->Data;
    return KSUCCESS;
}

KResult CallbackRequestHandler(KResult Status, storage_callback_t* CallbackData){
    CallbackData->Status = Status;
    kot_Sys_Unpause(CallbackData->MainThread);
    kot_Sys_Close(KSUCCESS);
}

KResult storage_device_t::SendRequest(uint64_t Start, size64_t Size, bool IsWrite){
    storage_callback_t* CallbackData = (storage_callback_t*)malloc(sizeof(storage_callback_t));
    CallbackData->Status = KBUSY;
    CallbackData->MainThread = kot_Sys_GetThread();

    kot_arguments_t parameters;
    parameters.arg[0] = CallbackRequestHandlerThread;
    parameters.arg[1] = (uint64_t)CallbackData;
    parameters.arg[2] = STORAGE_SINGLE_REQUEST;
    parameters.arg[3] = Start;
    parameters.arg[4] = Size;
    parameters.arg[5] = IsWrite;
    kot_Sys_ExecThread(Info.MainSpace.RequestToDeviceThread, &parameters, ExecutionTypeQueu, NULL);
    kot_Sys_Pause(false);

    KResult Status = CallbackData->Status;
    free(CallbackData);
    return Status;
}

uint64_t storage_device_t::GetBufferStartingAddress(uint64_t Start){
    return (uint64_t)BufferRWBase + (Start % Info.MainSpace.BufferRWAlignement);
}

KResult storage_device_t::ReadDevice(void* Buffer, uint64_t Start, size64_t Size){
    uint64_t RequestNum = DIV_ROUND_UP(Size, Info.MainSpace.BufferRWUsableSize);
    uint64_t StartInIteration = Start;
    uint64_t SizeToRead = Size;
    uint64_t AddressDst = (uint64_t)Buffer;
    uint64_t SizeToReadInIteration = SizeToRead;

    KResult Status = KSUCCESS;

    for(uint64_t i = 0; i < RequestNum; i++){
        if(SizeToRead > Info.MainSpace.BufferRWUsableSize){
            SizeToReadInIteration = Info.MainSpace.BufferRWUsableSize;
        }else{
            SizeToReadInIteration = SizeToRead;
        }

        atomicAcquire(&Lock, 0);
        Status = SendRequest(StartInIteration, SizeToReadInIteration, false);
        if(Status != KSUCCESS){
            atomicUnlock(&Lock, 0);
            return Status;
        }
        memcpy((void*)AddressDst, (void*)GetBufferStartingAddress(StartInIteration), SizeToReadInIteration);
        atomicUnlock(&Lock, 0);

        StartInIteration += SizeToReadInIteration;
        AddressDst += SizeToReadInIteration;
        SizeToRead -= SizeToReadInIteration;
    }
    return KSUCCESS;
}

KResult storage_device_t::WriteDevice(void* Buffer, uint64_t Start, size64_t Size){
    uint64_t RequestNum = DIV_ROUND_UP(Size, Info.MainSpace.BufferRWUsableSize);
    uint64_t StartInIteration = Start;
    uint64_t SizeToWrite = Size;
    uint64_t AddressDst = (uint64_t)Buffer;
    uint64_t SizeToWriteInIteration = SizeToWrite;

    for(uint64_t i = 0; i < RequestNum; i++){
        if(SizeToWrite > Info.MainSpace.BufferRWUsableSize){
            SizeToWriteInIteration = Info.MainSpace.BufferRWUsableSize;
        }else{
            SizeToWriteInIteration = SizeToWrite;
        }

        atomicAcquire(&Lock, 0);
        memcpy((void*)GetBufferStartingAddress(StartInIteration), (void*)AddressDst, SizeToWriteInIteration);
        SendRequest(StartInIteration, SizeToWriteInIteration, true);
        atomicUnlock(&Lock, 0);

        StartInIteration += SizeToWriteInIteration;
        AddressDst += SizeToWriteInIteration;
        SizeToWrite -= SizeToWriteInIteration;
    }
    return KSUCCESS;
}