#include <srv/srv.h>

Device** Devices = NULL;
uint64_t DevicesIndex = 0;
uint64_t SrvLock;

void SrvAddDevice(Device* Device){
    atomicAcquire(&SrvLock, 0);
    // Add device to internal handler
    Devices[DevicesIndex] = Device;
    Device->InternalID = DevicesIndex;
    DevicesIndex++;

    // Add device to external handler
    srv_storage_device_info_t Info;

    // Create thread Handler
    uisd_storage_t* StorageData = (uisd_storage_t*)FindControllerUISD(ControllerTypeEnum_Storage);

    process_t Proc = Sys_GetProcess();

    thread_t SrvReadWriteHandlerThread = NULL;
    Sys_Createthread(Proc, (uintptr_t)&SrvReadWriteHandler, PriviledgeApp, &SrvReadWriteHandlerThread);
    Info.ReadWriteThread = MakeShareableThreadToProcess(SrvReadWriteHandlerThread, StorageData->ControllerHeader.Process);
    Info.ReadWriteArg = DevicesIndex - 1;

    Info.BufferRWKey = Device->BufferKey;
    Info.BufferRWAlignement = Device->BufferAlignement;
    Info.BufferRWUsableSize = Device->BufferUsableSize;
    Info.DeviceSize = Device->GetSize();
    memcpy(&Info.SerialNumber, Device->GetSerialNumber(), SerialNumberSize);
    memcpy(&Info.DriveModelNumber, Device->GetSerialNumber(), DriveModelNumberSize);
    srv_storage_callback_t* callback = Srv_Storage_AddDevice(&Info, true);
    Device->ExternalID = (uint64_t)callback->Data;
    free(callback);
    atomicUnlock(&SrvLock, 0);
}

void SrvRemoveDevice(Device* Device){
    atomicAcquire(&SrvLock, 0);

    // Remove device to internal handler
    Devices[Device->InternalID] = NULL;

    // Remove device to external handler
    srv_storage_callback_t* callback = Srv_Storage_RemoveDevice(Device->ExternalID, true);
    atomicUnlock(&SrvLock, 0);
}

Device* SrvGetDevice(uint64_t Index){
    if(Index >= DevicesIndex) return NULL;
    return Devices[Index];
}

void SrvReadWriteHandler(thread_t Callback, uint64_t CallbackArg, uint64_t Index, uint64_t Start, size64_t Size, bool IsWrite){
    KResult Statu = KFAIL;
    if(IsWrite){
        Statu = SrvWrite(Index, Start, Size);
    }else{
        Statu = SrvRead(Index, Start, Size);
    }

    arguments_t arguments{
        .arg[0] = Statu,            /* Status */
        .arg[1] = CallbackArg,      /* CallbackArg */
        .arg[2] = NULL,             /* GP0 */
        .arg[3] = NULL,             /* GP1 */
        .arg[4] = NULL,             /* GP2 */
        .arg[5] = NULL,             /* GP3 */
    };

    Sys_Execthread(Callback, &arguments, ExecutionTypeQueu, NULL);
    Sys_Close(KSUCCESS);
}

KResult SrvRead(uint64_t Index, uint64_t Start, size64_t Size){
    Device* self = SrvGetDevice(Index);
    if(self){
        return self->Read(Start, Size);
    }
    return KFAIL;
}

KResult SrvWrite(uint64_t Index, uint64_t Start, size64_t Size){
    Device* self = SrvGetDevice(Index);
    if(self){
        return self->Write(Start, Size);
    }
    return KFAIL;
}