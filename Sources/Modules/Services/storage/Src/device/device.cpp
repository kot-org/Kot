#include <device/device.h>

KResult AddDevice(srv_storage_device_info_t* Info, storage_device_t** DevicePointer){
    uint64_t BufferType = NULL;
    uint64_t BufferSize = NULL;
    
    Sys_GetInfoMemoryField(Info->MainSpace.BufferRWKey, &BufferType, &BufferSize);
    if(BufferType == MemoryFieldTypeShareSpaceRW){
        storage_device_t* Device = (storage_device_t*)malloc(sizeof(storage_device_t));
        Device->BufferRWBase = getFreeAlignedSpace(BufferSize);
        Device->BufferRWSize = BufferSize;
        
        Sys_AcceptMemoryField(Sys_GetProcess(), Info->MainSpace.BufferRWKey, &Device->BufferRWBase);

        memcpy(&Device->Info, Info, sizeof(srv_storage_device_info_t));
        *DevicePointer = Device;

        thread_t CallbackRequestHandlerThread = NULL;
        Sys_Createthread(Sys_GetProcess(), (uintptr_t)&CallbackRequestHandler, PriviledgeApp, NULL, &CallbackRequestHandlerThread);
        Device->CallbackRequestHandlerThread = MakeShareableThreadToProcess(CallbackRequestHandlerThread, Device->Info.DriverProc);

        thread_t CallbackCreateSpaceHandlerThread = NULL;
        Sys_Createthread(Sys_GetProcess(), (uintptr_t)&CallbackCreateSpaceHandler, PriviledgeApp, NULL, &CallbackCreateSpaceHandlerThread);
        Device->CallbackCreateSpaceHandlerThread = MakeShareableThreadToProcess(CallbackCreateSpaceHandlerThread, Device->Info.DriverProc);
        LoadPartitionSystem(Device);

        return KSUCCESS;
    }
    return KFAIL;
}

KResult RemoveDevice(storage_device_t* Device){
    Sys_CloseMemoryField(Sys_GetProcess(), Device->Info.MainSpace.BufferRWKey, Device->BufferRWBase);
    return KSUCCESS;
}

KResult CallbackCreateSpaceHandler(KResult Statu, storage_callback_t* CallbackData, srv_storage_space_info_t* SpaceInfo){
    CallbackData->Data = malloc(sizeof(srv_storage_space_info_t));
    memcpy(CallbackData->Data, SpaceInfo, sizeof(srv_storage_space_info_t));
    Sys_Unpause(CallbackData->MainThread);
    Sys_Close(KSUCCESS);
}

KResult storage_device_t::CreateSpace(uint64_t Start, size64_t Size, srv_storage_space_info_t** SpaceInfo){
    storage_callback_t* callbackData = (storage_callback_t*)malloc(sizeof(storage_callback_t));
    callbackData->MainThread = Sys_Getthread();

    arguments_t parameters;
    parameters.arg[0] = CallbackCreateSpaceHandlerThread;
    parameters.arg[1] = (uint64_t)callbackData;
    parameters.arg[2] = Start;
    parameters.arg[3] = Size;
    Sys_Execthread(Info.MainSpace.CreateProtectedDeviceSpaceThread, &parameters, ExecutionTypeQueu, NULL);
    Sys_Pause(false);
    *SpaceInfo = (srv_storage_space_info_t*)callbackData->Data;
    return KSUCCESS;
}

KResult CallbackRequestHandler(KResult Statu, thread_t MainThread){
    Sys_Unpause(MainThread);
    Sys_Close(KSUCCESS);
}

KResult storage_device_t::SendRequest(uint64_t Start, size64_t Size, bool IsWrite){
    storage_callback_t* callbackData = (storage_callback_t*)malloc(sizeof(storage_callback_t));
    callbackData->MainThread = Sys_Getthread();

    arguments_t parameters;
    parameters.arg[0] = CallbackRequestHandlerThread;
    parameters.arg[1] = Sys_Getthread();
    parameters.arg[2] = Start;
    parameters.arg[3] = Size;
    parameters.arg[4] = IsWrite;
    Sys_Execthread(Info.MainSpace.ReadWriteDeviceThread, &parameters, ExecutionTypeQueu, NULL);
    Sys_Pause(false);
    return KSUCCESS;
}

uint64_t storage_device_t::GetBufferStartingAddress(uint64_t Start){
    return (uint64_t)BufferRWBase + (Start % Info.MainSpace.BufferRWAlignement);
}

KResult storage_device_t::ReadDevice(uintptr_t Buffer, uint64_t Start, size64_t Size){
    uint64_t RequestNum = DivideRoundUp(Size, Info.MainSpace.BufferRWUsableSize);
    uint64_t StartInIteration = Start;
    uint64_t SizeToRead = Size;
    uint64_t AddressDst = (uint64_t)Buffer;
    uint64_t SizeToReadInIteration = SizeToRead;

    for(uint64_t i = 0; i < RequestNum; i++){
        if(SizeToRead > Info.MainSpace.BufferRWUsableSize){
            SizeToReadInIteration = Info.MainSpace.BufferRWUsableSize;
        }else{
            SizeToReadInIteration = SizeToRead;
        }

        atomicAcquire(&Lock, 0);
        SendRequest(StartInIteration, SizeToReadInIteration, false);
        memcpy((uintptr_t)AddressDst, (uintptr_t)GetBufferStartingAddress(StartInIteration), SizeToReadInIteration);
        atomicUnlock(&Lock, 0);

        StartInIteration += SizeToReadInIteration;
        AddressDst += SizeToReadInIteration;
        SizeToRead -= SizeToReadInIteration;
    }
    return KSUCCESS;
}

KResult storage_device_t::WriteDevice(uintptr_t Buffer, uint64_t Start, size64_t Size){
    uint64_t RequestNum = DivideRoundUp(Size, Info.MainSpace.BufferRWUsableSize);
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
        memcpy((uintptr_t)GetBufferStartingAddress(StartInIteration), (uintptr_t)AddressDst, SizeToWriteInIteration);
        SendRequest(StartInIteration, SizeToWriteInIteration, true);
        atomicUnlock(&Lock, 0);

        StartInIteration += SizeToWriteInIteration;
        AddressDst += SizeToWriteInIteration;
        SizeToWrite -= SizeToWriteInIteration;
    }
    return KSUCCESS;
}