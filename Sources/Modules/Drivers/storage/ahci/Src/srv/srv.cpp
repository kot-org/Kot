#include <srv/srv.h>

uint64_t SrvLock;
uisd_storage_t* StorageHandler;

void SrvAddDevice(Device* Device){
    atomicAcquire(&SrvLock, 0);

    // Add device to external handler
    srv_storage_device_info_t Info;

    StorageHandler = (uisd_storage_t*)FindControllerUISD(ControllerTypeEnum_Storage);

    // Create threads handler
    process_t Proc = Sys_GetProcess();

    /* CreateProtectedDeviceSpaceThread */
    thread_t SrvCreateProtectedSpaceThread = NULL;
    Sys_Createthread(Proc, (uintptr_t)&SrvCreateProtectedSpace, PriviledgeDriver, (uint64_t)Device->DefaultSpace, &SrvCreateProtectedSpaceThread);
    Info.MainSpace.CreateProtectedDeviceSpaceThread = MakeShareableThreadToProcess(SrvCreateProtectedSpaceThread, StorageHandler->ControllerHeader.Process);

    /* RequestToDeviceThread */
    thread_t SrvRequestHandlerThread = NULL;
    Sys_Createthread(Proc, (uintptr_t)&SrvRequestHandler, PriviledgeApp, (uint64_t)Device->DefaultSpace, &SrvRequestHandlerThread);
    Info.MainSpace.RequestToDeviceThread = MakeShareableThreadToProcess(SrvRequestHandlerThread, StorageHandler->ControllerHeader.Process);

    Info.MainSpace.BufferRWKey = Device->DefaultSpace->BufferKey;
    Info.MainSpace.SpaceSize = Device->DefaultSpace->Size;
    Info.MainSpace.BufferRWAlignement = Device->BufferAlignement;
    Info.MainSpace.BufferRWUsableSize = Device->BufferUsableSize;
    Info.MainSpace.DriverProc = ShareProcessKey(Proc);
    Info.DeviceSize = Device->GetSize();

    memcpy(&Info.SerialNumber, Device->GetSerialNumber(), Serial_Number_Size);
    memcpy(&Info.DriveModelNumber, Device->GetSerialNumber(), Drive_Model_Number_Size);
    

    srv_storage_callback_t* callback = Srv_Storage_AddDevice(&Info, true);
    Device->ExternalID = (uint64_t)callback->Data;

    free(callback);
    atomicUnlock(&SrvLock, 0);
}

void SrvRemoveDevice(thread_t Callback, uint64_t CallbackArg){
    atomicAcquire(&SrvLock, 0);
    Space_t* Space = (Space_t*)Sys_GetExternalDataThread();
    // Remove device to external handler
    srv_storage_callback_t* callback = Srv_Storage_RemoveDevice(Space->StorageDevice->ExternalID, true);
    atomicUnlock(&SrvLock, 0);
}

void SrvCreateProtectedSpace(thread_t Callback, uint64_t CallbackArg, uint64_t Start, uint64_t Size){
    KResult Status = KFAIL;

    Space_t* ActualSpaceLocalInfo = (Space_t*)Sys_GetExternalDataThread();

    if(ActualSpaceLocalInfo->Start > Start){
        Start = ActualSpaceLocalInfo->Start;
    }

    if(ActualSpaceLocalInfo->Size > Size){
        Size = ActualSpaceLocalInfo->Size;
    }
    Space_t* SpaceLocalInfo = ActualSpaceLocalInfo->StorageDevice->CreateSpace(Start, Size);

    SpaceLocalInfo->Process = StorageHandler->ControllerHeader.Process;

    srv_storage_space_info_t SpaceInfo;

    process_t Proc = Sys_GetProcess();

    /* CreateProtectedDeviceSpaceThread */
    thread_t SrvCreateProtectedSpaceThread = NULL;
    Sys_Createthread(Proc, (uintptr_t)&SrvCreateProtectedSpace, PriviledgeApp, (uint64_t)SpaceLocalInfo, &SrvCreateProtectedSpaceThread);
    SpaceInfo.CreateProtectedDeviceSpaceThread = MakeShareableSpreadThreadToProcess(SrvCreateProtectedSpaceThread, StorageHandler->ControllerHeader.Process);

    /* RequestToDeviceThread */
    thread_t SrvRequestHandlerThread = NULL;
    Sys_Createthread(Proc, (uintptr_t)&SrvRequestHandler, PriviledgeApp, (uint64_t)SpaceLocalInfo, &SrvRequestHandlerThread);
    SpaceInfo.RequestToDeviceThread = MakeShareableSpreadThreadToProcess(SrvRequestHandlerThread, StorageHandler->ControllerHeader.Process);

    SpaceInfo.BufferRWKey = SpaceLocalInfo->BufferKey;
    SpaceInfo.BufferRWAlignement = ActualSpaceLocalInfo->StorageDevice->BufferAlignement;
    SpaceInfo.BufferRWUsableSize = ActualSpaceLocalInfo->StorageDevice->BufferUsableSize;
    SpaceInfo.SpaceSize = Size;

    ShareDataWithArguments_t data{
        .Data = &SpaceInfo,
        .Size = sizeof(srv_storage_space_info_t),
        .ParameterPosition = 0x2,
    };
    
    arguments_t arguments{
        .arg[0] = Status,            /* Status */
        .arg[1] = CallbackArg,      /* CallbackArg */
        .arg[2] = NULL,             /* SpaceInfo */
        .arg[3] = NULL,             /* GP1 */
        .arg[4] = NULL,             /* GP2 */
        .arg[5] = NULL,             /* GP3 */
    };

    Sys_Execthread(Callback, &arguments, ExecutionTypeQueu, &data);
    Sys_Close(KSUCCESS);
}

void SrvSingleRequestHandler(thread_t Callback, uint64_t CallbackArg, uint64_t Start, size64_t Size, bool IsWrite){
    KResult Status = KFAIL;

    Space_t* Space = (Space_t*)Sys_GetExternalDataThread();

    if(Size <= Space->StorageDevice->BufferUsableSize){
        if((Start + (uint64_t)Size) <= Space->StorageDevice->GetSize()){
            Start += Space->Start;
            atomicAcquire(&Space->StorageDevice->DeviceLock, 0);
            Space->StorageDevice->LoadSpace(Space);
            if(IsWrite){
                Status = Space->StorageDevice->Write(Space, Start, Size);
            }else{
                Status = Space->StorageDevice->Read(Space, Start, Size);
            }
            atomicUnlock(&Space->StorageDevice->DeviceLock, 0);
        }
    }

    arguments_t arguments{
        .arg[0] = Status,           /* Status */
        .arg[1] = CallbackArg,      /* CallbackArg */
        .arg[2] = NULL,             /* GP0 */
        .arg[3] = NULL,             /* GP1 */
        .arg[4] = NULL,             /* GP2 */
        .arg[5] = NULL,             /* GP3 */
    };

    Sys_Execthread(Callback, &arguments, ExecutionTypeQueu, NULL);
    Sys_Close(KSUCCESS);
}

void SrvMultipleRequestHandler(thread_t Callback, uint64_t CallbackArg, srv_storage_multiple_requests_t* Requests){
    KResult Status = KFAIL;

    Space_t* Space = (Space_t*)Sys_GetExternalDataThread();
    uintptr_t Buffer = malloc(Requests->TotalSize);
    if(Requests->IsWrite){
        uint64_t Type;
        size64_t Size;
        Sys_GetInfoMemoryField(Requests->MemoryKey, &Type, &Size);
        if(Type == MemoryFieldTypeSendSpaceRO && Size == Requests->TotalSize){
            Sys_AcceptMemoryField(Sys_GetProcess(), Requests->MemoryKey, &Buffer);
        }else{
            free(Buffer);
            return;
        }
    }

    for(uint64_t i = 0; i < Requests->RequestsCount; i++){
        srv_storage_request_t Request = Requests->Requests[i];

        if(Request.Size <= Space->StorageDevice->BufferUsableSize){
            if((Request.Start + (uint64_t)Request.Size) <= Space->StorageDevice->GetSize()){
                Request.Start += Space->Start;
                atomicAcquire(&Space->StorageDevice->DeviceLock, 0);
                Space->StorageDevice->LoadSpace(Space);
                if(Requests->IsWrite){
                    memcpy((uintptr_t)((uint64_t)Space->BufferVirtual + (Request.Start % Space->StorageDevice->BufferAlignement)), (uintptr_t)((uint64_t)Buffer + Request.BufferOffset), Request.Size);
                    Status = Space->StorageDevice->Write(Space, Request.Start, Request.Size);
                }else{
                    Status = Space->StorageDevice->Read(Space, Request.Start, Request.Size);
                    memcpy((uintptr_t)((uint64_t)Buffer + Request.BufferOffset), (uintptr_t)((uint64_t)Space->BufferVirtual + (Request.Start % Space->StorageDevice->BufferAlignement)), Request.Size);
                }
                atomicUnlock(&Space->StorageDevice->DeviceLock, 0);
            }
        }
    }

    arguments_t arguments{
        .arg[0] = Status,           /* Status */
        .arg[1] = CallbackArg,      /* CallbackArg */
        .arg[2] = NULL,             /* GP0 */
        .arg[3] = NULL,             /* GP1 */
        .arg[4] = NULL,             /* GP2 */
        .arg[5] = NULL,             /* GP3 */
    };
    
    if(!Requests->IsWrite){
        ksmem_t LocalKey;
        Sys_CreateMemoryField(Sys_GetProcess(), Requests->TotalSize, &Buffer, &LocalKey, MemoryFieldTypeSendSpaceRO);
        Sys_Keyhole_CloneModify(LocalKey, &arguments.arg[2], Space->Process, KeyholeFlagPresent | KeyholeFlagCloneable | KeyholeFlagEditable, PriviledgeApp);
    }

    Sys_Execthread(Callback, &arguments, ExecutionTypeQueu, NULL);
    Sys_Close(KSUCCESS);
}

void SrvRequestHandler(thread_t Callback, uint64_t CallbackArg, uint64_t RequestType, uint64_t GP0, uint64_t GP1, uint64_t GP2){
    if(RequestType == STORAGE_SINGLE_REQUEST){
        SrvSingleRequestHandler(Callback, CallbackArg, GP0, GP1, GP2);
    }else if(RequestType == STORAGE_MULTIPLE_REQUESTS){
        SrvMultipleRequestHandler(Callback, CallbackArg, (srv_storage_multiple_requests_t*)GP0);
    }

    arguments_t arguments{
        .arg[0] = KFAIL,            /* Status */
        .arg[1] = CallbackArg,      /* CallbackArg */
        .arg[2] = NULL,             /* GP0 */
        .arg[3] = NULL,             /* GP1 */
        .arg[4] = NULL,             /* GP2 */
        .arg[5] = NULL,             /* GP3 */
    };

    Sys_Execthread(Callback, &arguments, ExecutionTypeQueu, NULL);
    Sys_Close(KSUCCESS);
}