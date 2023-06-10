#include <srv/srv.h>

uint64_t SrvLock = 0;
kot_uisd_storage_t* StorageHandler;

void SrvAddDevice(Device* Device){
    atomicAcquire(&SrvLock, 0);

    // Add device to external handler
    kot_srv_storage_device_info_t Info;

    StorageHandler = (kot_uisd_storage_t*)kot_FindControllerUISD(ControllerTypeEnum_Storage);

    // Create threads handler
    kot_process_t Proc = kot_Sys_GetProcess();

    /* CreateProtectedDeviceSpaceThread */
    kot_thread_t SrvCreateProtectedSpaceThread = NULL;
    kot_Sys_CreateThread(Proc, (void*)&SrvCreateProtectedSpace, PriviledgeDriver, (uint64_t)Device->DefaultSpace, &SrvCreateProtectedSpaceThread);
    Info.MainSpace.CreateProtectedDeviceSpaceThread = kot_MakeShareableThreadToProcess(SrvCreateProtectedSpaceThread, StorageHandler->ControllerHeader.Process);

    /* RequestToDeviceThread */
    kot_thread_t SrvRequestHandlerThread = NULL;
    kot_Sys_CreateThread(Proc, (void*)&SrvRequestHandler, PriviledgeApp, (uint64_t)Device->DefaultSpace, &SrvRequestHandlerThread);
    Info.MainSpace.RequestToDeviceThread = kot_MakeShareableThreadToProcess(SrvRequestHandlerThread, StorageHandler->ControllerHeader.Process);

    Info.MainSpace.BufferRWKey = Device->DefaultSpace->BufferKey;
    Info.MainSpace.SpaceSize = Device->DefaultSpace->Size;
    Info.MainSpace.BufferRWAlignement = Device->BufferAlignement;
    Info.MainSpace.BufferRWUsableSize = Device->BufferUsableSize;
    Info.MainSpace.DriverProc = kot_ShareProcessKey(Proc);
    Info.DeviceSize = Device->GetSize();

    memcpy(&Info.SerialNumber, Device->GetSerialNumber(), Serial_Number_Size);
    memcpy(&Info.DriveModelNumber, Device->GetSerialNumber(), Drive_Model_Number_Size);
    

    kot_srv_storage_callback_t* callback = kot_Srv_Storage_AddDevice(&Info, true);
    Device->ExternalID = (uint64_t)callback->Data;

    free(callback);
    atomicUnlock(&SrvLock, 0);
}

void SrvRemoveDevice(kot_thread_t Callback, uint64_t CallbackArg){
    atomicAcquire(&SrvLock, 0);
    Space_t* Space = (Space_t*)kot_Sys_GetExternalDataThread();
    // Remove device to external handler
    kot_srv_storage_callback_t* callback = kot_Srv_Storage_RemoveDevice(Space->StorageDevice->ExternalID, true);
    atomicUnlock(&SrvLock, 0);
}

void SrvCreateProtectedSpace(kot_thread_t Callback, uint64_t CallbackArg, uint64_t Start, uint64_t Size){
    KResult Status = KFAIL;

    Space_t* ActualSpaceLocalInfo = (Space_t*)kot_Sys_GetExternalDataThread();

    if(ActualSpaceLocalInfo->Start > Start){
        Start = ActualSpaceLocalInfo->Start;
    }

    if(ActualSpaceLocalInfo->Size > Size){
        Size = ActualSpaceLocalInfo->Size;
    }
    Space_t* SpaceLocalInfo = ActualSpaceLocalInfo->StorageDevice->CreateSpace(Start, Size);

    kot_srv_storage_space_info_t SpaceInfo;

    kot_process_t Proc = kot_Sys_GetProcess();

    /* CreateProtectedDeviceSpaceThread */
    kot_thread_t SrvCreateProtectedSpaceThread = NULL;
    kot_Sys_CreateThread(Proc, (void*)&SrvCreateProtectedSpace, PriviledgeApp, (uint64_t)SpaceLocalInfo, &SrvCreateProtectedSpaceThread);
    SpaceInfo.CreateProtectedDeviceSpaceThread = kot_MakeShareableSpreadThreadToProcess(SrvCreateProtectedSpaceThread, StorageHandler->ControllerHeader.Process);

    /* RequestToDeviceThread */
    kot_thread_t SrvRequestHandlerThread = NULL;
    kot_Sys_CreateThread(Proc, (void*)&SrvRequestHandler, PriviledgeApp, (uint64_t)SpaceLocalInfo, &SrvRequestHandlerThread);
    SpaceInfo.RequestToDeviceThread = kot_MakeShareableSpreadThreadToProcess(SrvRequestHandlerThread, StorageHandler->ControllerHeader.Process);

    SpaceInfo.BufferRWKey = SpaceLocalInfo->BufferKey;
    SpaceInfo.BufferRWAlignement = ActualSpaceLocalInfo->StorageDevice->BufferAlignement;
    SpaceInfo.BufferRWUsableSize = ActualSpaceLocalInfo->StorageDevice->BufferUsableSize;
    SpaceInfo.SpaceSize = Size;

    kot_ShareDataWithArguments_t data{
        .Data = &SpaceInfo,
        .Size = sizeof(kot_srv_storage_space_info_t),
        .ParameterPosition = 0x2,
    };
    
    kot_arguments_t arguments{
        .arg[0] = Status,            /* Status */
        .arg[1] = CallbackArg,      /* CallbackArg */
        .arg[2] = NULL,             /* SpaceInfo */
        .arg[3] = NULL,             /* GP1 */
        .arg[4] = NULL,             /* GP2 */
        .arg[5] = NULL,             /* GP3 */
    };

    kot_Sys_ExecThread(Callback, &arguments, ExecutionTypeQueu, &data);
    kot_Sys_Close(KSUCCESS);
}

Space_t* SpaceTest;

void SrvSingleRequestHandler(kot_thread_t Callback, uint64_t CallbackArg, uint64_t Start, size64_t Size, bool IsWrite){
    KResult Status = KFAIL;

    Space_t* Space = (Space_t*)kot_Sys_GetExternalDataThread();
    if(Size <= Space->StorageDevice->BufferUsableSize){
        if((Start + (uint64_t)Size) <= Space->Size){
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

    kot_arguments_t arguments{
        .arg[0] = Status,           /* Status */
        .arg[1] = CallbackArg,      /* CallbackArg */
        .arg[2] = NULL,             /* GP0 */
        .arg[3] = NULL,             /* GP1 */
        .arg[4] = NULL,             /* GP2 */
        .arg[5] = NULL,             /* GP3 */
    };

    kot_Sys_ExecThread(Callback, &arguments, ExecutionTypeQueu, NULL);
    kot_Sys_Close(KSUCCESS);
}

void SrvMultipleRequestHandler(kot_thread_t Callback, uint64_t CallbackArg, kot_srv_storage_multiple_requests_t* Requests, kot_process_t Process){
    KResult Status = KFAIL;

    Space_t* Space = (Space_t*)kot_Sys_GetExternalDataThread();
    void* Buffer = malloc(Requests->TotalSize);
    if(Requests->IsWrite){
        uint64_t Type;
        size64_t Size;
        kot_Sys_GetInfoMemoryField(Requests->MemoryKey, &Type, &Size);
        if(Type == MemoryFieldTypeSendSpaceRO && Size == Requests->TotalSize){
            kot_Sys_AcceptMemoryField(kot_Sys_GetProcess(), Requests->MemoryKey, &Buffer);
        }else{
            free(Buffer);
            return;
        }
    }

    for(uint64_t i = 0; i < Requests->RequestsCount; i++){
        kot_srv_storage_request_t Request = Requests->Requests[i];

        if(Request.Size <= Space->StorageDevice->BufferUsableSize){
            if((Request.Start + (uint64_t)Request.Size) <= Space->Size){
                Request.Start += Space->Start;

                uint64_t RequestNum = DIV_ROUND_UP(Request.Size, Space->StorageDevice->BufferUsableSize);
                uint64_t StartInIteration = Request.Start;
                uint64_t SizeToRead = Request.Size;
                uint64_t AddressDst = (uint64_t)Buffer + Request.BufferOffset;
                uint64_t SizeToProcessInIteration = SizeToRead;

                for(uint64_t i = 0; i < RequestNum; i++){
                    if(SizeToRead > Space->StorageDevice->BufferUsableSize){
                        SizeToProcessInIteration = Space->StorageDevice->BufferUsableSize;
                    }else{
                        SizeToProcessInIteration = SizeToRead;
                    }

                    if(SizeToProcessInIteration + AddressDst > (uintptr_t)Buffer + Requests->TotalSize){
                        Status = KBUSY;
                        break;
                    }

                    atomicAcquire(&Space->StorageDevice->DeviceLock, 0);
                    Space->StorageDevice->LoadSpace(Space);
                    if(Requests->IsWrite){
                        memcpy((void*)((uint64_t)Space->BufferVirtual + (StartInIteration % Space->StorageDevice->BufferAlignement)), (void*)AddressDst, SizeToProcessInIteration);
                        Status = Space->StorageDevice->Write(Space, StartInIteration, SizeToProcessInIteration);
                    }else{
                        Status = Space->StorageDevice->Read(Space, StartInIteration, SizeToProcessInIteration);
                        memcpy((void*)AddressDst, (void*)((uint64_t)Space->BufferVirtual + (StartInIteration % Space->StorageDevice->BufferAlignement)), SizeToProcessInIteration);
                    }
                    atomicUnlock(&Space->StorageDevice->DeviceLock, 0);

                    StartInIteration += SizeToProcessInIteration;
                    AddressDst += SizeToProcessInIteration;
                    SizeToRead -= SizeToProcessInIteration;
                }
            }
        }
    }


    kot_arguments_t arguments{
        .arg[0] = Status,           /* Status */
        .arg[1] = CallbackArg,      /* CallbackArg */
        .arg[2] = NULL,             /* GP0 */
        .arg[3] = NULL,             /* GP1 */
        .arg[4] = NULL,             /* GP2 */
        .arg[5] = NULL,             /* GP3 */
    };
    
    if(!Requests->IsWrite){
        kot_key_mem_t LocalKey;
        kot_Sys_CreateMemoryField(kot_Sys_GetProcess(), Requests->TotalSize, &Buffer, &LocalKey, MemoryFieldTypeSendSpaceRO);
        kot_Sys_Keyhole_CloneModify(LocalKey, &arguments.arg[2], Process, KeyholeFlagPresent | KeyholeFlagCloneable | KeyholeFlagEditable, PriviledgeApp);
    }

    kot_Sys_ExecThread(Callback, &arguments, ExecutionTypeQueu, NULL);
    kot_Sys_Close(KSUCCESS);
}

void SrvRequestHandler(kot_thread_t Callback, uint64_t CallbackArg, uint64_t RequestType, uint64_t GP0, uint64_t GP1, uint64_t GP2){
    if(RequestType == STORAGE_SINGLE_REQUEST){
        SrvSingleRequestHandler(Callback, CallbackArg, GP0, GP1, GP2);
    }else if(RequestType == STORAGE_MULTIPLE_REQUESTS){
        SrvMultipleRequestHandler(Callback, CallbackArg, (kot_srv_storage_multiple_requests_t*)GP0, GP1);
    }

    kot_arguments_t arguments{
        .arg[0] = KFAIL,            /* Status */
        .arg[1] = CallbackArg,      /* CallbackArg */
        .arg[2] = NULL,             /* GP0 */
        .arg[3] = NULL,             /* GP1 */
        .arg[4] = NULL,             /* GP2 */
        .arg[5] = NULL,             /* GP3 */
    };

    kot_Sys_ExecThread(Callback, &arguments, ExecutionTypeQueu, NULL);
    kot_Sys_Close(KSUCCESS);
}