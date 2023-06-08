#include <srv/srv.h>

kot_uisd_storage_t* SrvData;

KResult InitialiseSrv(){
    kot_process_t proc = kot_Sys_GetProcess();

    void* address = kot_GetFreeAlignedSpace(sizeof(kot_uisd_storage_t));
    kot_key_mem_t key = NULL;
    kot_Sys_CreateMemoryField(proc, sizeof(kot_uisd_storage_t), &address, &key, MemoryFieldTypeShareSpaceRO);

    SrvData = (kot_uisd_storage_t*)address;
    memset(SrvData, 0, sizeof(kot_uisd_storage_t)); // Clear data

    SrvData->ControllerHeader.IsReadWrite = false;
    SrvData->ControllerHeader.Version = Storage_Srv_Version;
    SrvData->ControllerHeader.VendorID = Kot_VendorID;
    SrvData->ControllerHeader.Type = ControllerTypeEnum_Storage;
    SrvData->ControllerHeader.Process = kot_ShareProcessKey(proc);

    /* AddDevice */
    kot_thread_t AddDeviceThread = NULL;
    kot_Sys_CreateThread(proc, (void*)&AddDeviceSrv, PriviledgeApp, NULL, &AddDeviceThread);
    SrvData->AddDevice = kot_MakeShareableThread(AddDeviceThread, PriviledgeDriver);

    /* RemoveDevice */
    kot_thread_t RemoveDeviceThread = NULL;
    kot_Sys_CreateThread(proc, (void*)&RemoveDeviceSrv, PriviledgeApp, NULL, &RemoveDeviceThread);
    SrvData->RemoveDevice = kot_MakeShareableThread(RemoveDeviceThread, PriviledgeDriver);

    /* NotifyOnNewPartitionByGUIDType */
    kot_thread_t CountPartitionByGUIDTypeThread = NULL;
    kot_Sys_CreateThread(proc, (void*)&NotifyOnNewPartitionByGUIDTypeSrv, PriviledgeApp, NULL, &CountPartitionByGUIDTypeThread);
    SrvData->NotifyOnNewPartitionByGUIDType = kot_MakeShareableThread(CountPartitionByGUIDTypeThread, PriviledgeDriver);

    /* VFSLoginApp */
    kot_thread_t VFSLoginAppThread = NULL;
    kot_Sys_CreateThread(proc, (void*)&VFSLoginApp, PriviledgeApp, NULL, &VFSLoginAppThread);
    SrvData->VFSLoginApp = kot_MakeShareableThread(VFSLoginAppThread, PriviledgeDriver);

    /* NewDev */
    kot_thread_t NewDevThread = NULL;
    kot_Sys_CreateThread(proc, (void*)&NewDev, PriviledgeApp, NULL, &NewDevThread);
    SrvData->NewDev = kot_MakeShareableThread(NewDevThread, PriviledgeService);
    
    kot_uisd_callbackInfo_t* info = kot_CreateControllerUISD(ControllerTypeEnum_Storage, key, true);   
    free(info);

    return KSUCCESS;
}

KResult AddDeviceSrv(kot_thread_t Callback, uint64_t CallbackArg, kot_srv_storage_device_info_t* Info){
    KResult Status = KFAIL;
    storage_device_t* Device = NULL;
    if(Info){
        Status = AddDevice(Info, &Device);
    }
    
    kot_arguments_t arguments{
        .arg[0] = Status,            /* Status */
        .arg[1] = CallbackArg,      /* CallbackArg */
        .arg[2] = (uint64_t)Device, /* DeviceIndex */
        .arg[3] = NULL,             /* GP1 */
        .arg[4] = NULL,             /* GP2 */
        .arg[5] = NULL,             /* GP3 */
    };

    kot_Sys_ExecThread(Callback, &arguments, ExecutionTypeQueu, NULL);
    kot_Sys_Close(KSUCCESS);
}

KResult RemoveDeviceSrv(kot_thread_t Callback, uint64_t CallbackArg, storage_device_t* Device){
    KResult Status = RemoveDevice(Device);
    
    kot_arguments_t arguments{
        .arg[0] = Status,            /* Status */
        .arg[1] = CallbackArg,      /* CallbackArg */
        .arg[2] = NULL,             /* GP0 */
        .arg[3] = NULL,             /* GP1 */
        .arg[4] = NULL,             /* GP2 */
        .arg[5] = NULL,             /* GP3 */
    };

    kot_Sys_ExecThread(Callback, &arguments, ExecutionTypeQueu, NULL);
    kot_Sys_Close(KSUCCESS);
}

KResult NotifyOnNewPartitionByGUIDTypeSrv(kot_thread_t Callback, uint64_t CallbackArg, kot_thread_t ThreadToNotify, kot_process_t ProcessToNotify, kot_GUID_t* PartitionTypeGUID){
    KResult Status = NotifyOnNewPartitionByGUIDType(PartitionTypeGUID, ThreadToNotify, ProcessToNotify);
    
    kot_arguments_t arguments{
        .arg[0] = Status,            /* Status */
        .arg[1] = CallbackArg,      /* CallbackArg */
        .arg[2] = NULL,             /* GP0 */
        .arg[3] = NULL,             /* GP1 */
        .arg[4] = NULL,             /* GP2 */
        .arg[5] = NULL,             /* GP3 */
    };

    kot_Sys_ExecThread(Callback, &arguments, ExecutionTypeQueu, NULL);
    kot_Sys_Close(KSUCCESS);
}