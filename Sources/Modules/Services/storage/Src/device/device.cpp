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

        thread_t CallbackRequestHandlerThread = NULL;
        Sys_Createthread(Sys_GetProcess(), (uintptr_t)&CallbackRequestHandler, PriviledgeApp, &CallbackRequestHandlerThread);
        Device->CallbackRequestHandlerThread = MakeShareableThreadToProcess(CallbackRequestHandlerThread, Device->Info.DriverProc);
        // test
        uintptr_t buffer = malloc(0x1000);
        memset(buffer, 0xff, 0x203);
        Device->WriteDevice(buffer, 0x102, 0x203);
        Device->ReadDevice(buffer, 0x0, 0x1000);
        for(uint64_t i = 0x200; i < 0x210; i++){
            std::printf("%x", ((uint8_t*)buffer)[i]);
        }            
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


KResult CallbackRequestHandler(KResult Statu, thread_t Caller){
    Sys_Unpause(Caller);
    Sys_Close(KSUCCESS);
}

KResult storage_device_t::SendRequest(uint64_t Start, size64_t Size, bool IsWrite){
    arguments_t parameters;
    parameters.arg[0] = CallbackRequestHandlerThread;
    parameters.arg[1] = Sys_Getthread();
    parameters.arg[2] = Info.ReadWriteArg;
    parameters.arg[3] = Start;
    parameters.arg[4] = Size;
    parameters.arg[5] = IsWrite;
    Sys_Execthread(Info.ReadWriteThread, &parameters, ExecutionTypeQueu, NULL);
    Sys_Pause(false);
    return KSUCCESS;
}

uint64_t storage_device_t::GetBufferStartingAddress(uint64_t Start){
    return (uint64_t)BufferRWBase + (Start % Info.BufferRWAlignement);
}

KResult storage_device_t::ReadDevice(uintptr_t Buffer, uint64_t Start, size64_t Size){
    uint64_t RequestNum = DivideRoundUp(Size, Info.BufferRWUsableSize);
    uint64_t StartInIteration = Start;
    uint64_t SizeToRead = Size;
    uint64_t AddressDst = (uint64_t)Buffer;
    uint64_t SizeToReadInIteration = SizeToRead;

    for(uint64_t i = 0; i < RequestNum; i++){
        if(SizeToRead > Info.BufferRWUsableSize){
            SizeToReadInIteration = Info.BufferRWUsableSize;
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
    uint64_t RequestNum = DivideRoundUp(Size, Info.BufferRWUsableSize);
    uint64_t StartInIteration = Start;
    uint64_t SizeToWrite = Size;
    uint64_t AddressDst = (uint64_t)Buffer;
    uint64_t SizeToWriteInIteration = SizeToWrite;

    for(uint64_t i = 0; i < RequestNum; i++){
        if(SizeToWrite > Info.BufferRWUsableSize){
            SizeToWriteInIteration = Info.BufferRWUsableSize;
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