#include <srv/srv.h>

uint64_t SrvLock;

void SrvAddDevice(Device* Device){
    atomicAcquire(&SrvLock, 0);

    // Add device to external handler
    srv_storage_device_info_t Info;

    // Create thread Handler
    uisd_storage_t* StorageData = (uisd_storage_t*)FindControllerUISD(ControllerTypeEnum_Storage);

    process_t Proc = Sys_GetProcess();

    thread_t SrvReadWriteHandlerThread = NULL;
    Sys_Createthread(Proc, (uintptr_t)&SrvReadWriteHandler, PriviledgeApp, &SrvReadWriteHandlerThread);
    Info.ReadWriteThread = MakeShareableThreadToProcess(SrvReadWriteHandlerThread, StorageData->ControllerHeader.Process);
    Info.ReadWriteArg = (uint64_t)Device;

    Info.BufferRWKey = Device->BufferKey;
    Info.BufferRWAlignement = Device->BufferAlignement;
    Info.BufferRWUsableSize = Device->BufferUsableSize;
    Info.DeviceSize = Device->GetSize();

    memcpy(&Info.SerialNumber, Device->GetSerialNumber(), SerialNumberSize);
    memcpy(&Info.DriveModelNumber, Device->GetSerialNumber(), DriveModelNumberSize);
    
    Info.DriverProc = ShareProcessKey(Proc);

    srv_storage_callback_t* callback = Srv_Storage_AddDevice(&Info, true);
    Device->ExternalID = (uint64_t)callback->Data;

    free(callback);
    atomicUnlock(&SrvLock, 0);
}

void SrvRemoveDevice(Device* Device){
    atomicAcquire(&SrvLock, 0);

    // Remove device to external handler
    srv_storage_callback_t* callback = Srv_Storage_RemoveDevice(Device->ExternalID, true);
    atomicUnlock(&SrvLock, 0);
}

void SrvReadWriteHandler(thread_t Callback, uint64_t CallbackArg, Device* Device, uint64_t Start, size64_t Size, bool IsWrite){
    KResult Statu = KFAIL;
    if(Device){
        if(IsWrite){
            Statu = SrvWrite(Device, Start, Size);
        }else{
            Statu = SrvRead(Device, Start, Size);
        }
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

KResult SrvRead(Device* Device, uint64_t Start, size64_t Size){
    if(Size <= Device->BufferUsableSize){
        return Device->Read(Start, Size);
    }
    return KFAIL;
}

KResult SrvWrite(Device* Device, uint64_t Start, size64_t Size){;
    if(Size <= Device->BufferUsableSize){
        return Device->Write(Start, Size);
    }
    return KFAIL;
}