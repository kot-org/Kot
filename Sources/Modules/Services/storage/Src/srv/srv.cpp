#include <srv/srv.h>

uisd_storage_t* SrvData;

KResult InitialiseSrv(){
    process_t proc = Sys_GetProcess();

    uintptr_t address = GetFreeAlignedSpace(sizeof(uisd_storage_t));
    ksmem_t key = NULL;
    Sys_CreateMemoryField(proc, sizeof(uisd_storage_t), &address, &key, MemoryFieldTypeShareSpaceRO);

    SrvData = (uisd_storage_t*)address;
    SrvData->ControllerHeader.IsReadWrite = false;
    SrvData->ControllerHeader.Version = Storage_Srv_Version;
    SrvData->ControllerHeader.VendorID = Kot_VendorID;
    SrvData->ControllerHeader.Type = ControllerTypeEnum_Storage;
    SrvData->ControllerHeader.Process = ShareProcessKey(proc);

    /* AddDevice */
    thread_t AddDeviceThread = NULL;
    Sys_CreateThread(proc, (uintptr_t)&AddDeviceSrv, PriviledgeApp, NULL, &AddDeviceThread);
    SrvData->AddDevice = MakeShareableThread(AddDeviceThread, PriviledgeDriver);

    /* RemoveDevice */
    thread_t RemoveDeviceThread = NULL;
    Sys_CreateThread(proc, (uintptr_t)&RemoveDeviceSrv, PriviledgeApp, NULL, &RemoveDeviceThread);
    SrvData->RemoveDevice = MakeShareableThread(RemoveDeviceThread, PriviledgeDriver);

    /* NotifyOnNewPartitionByGUIDType */
    thread_t CountPartitionByGUIDTypeThread = NULL;
    Sys_CreateThread(proc, (uintptr_t)&NotifyOnNewPartitionByGUIDTypeSrv, PriviledgeApp, NULL, &CountPartitionByGUIDTypeThread);
    SrvData->NotifyOnNewPartitionByGUIDType = MakeShareableThread(CountPartitionByGUIDTypeThread, PriviledgeDriver);

    /* VFSLoginApp */
    thread_t VFSLoginAppThread = NULL;
    Sys_CreateThread(proc, (uintptr_t)&VFSLoginApp, PriviledgeApp, NULL, &VFSLoginAppThread);
    SrvData->VFSLoginApp = MakeShareableThread(VFSLoginAppThread, PriviledgeDriver);
    
    uisd_callbackInfo_t* info = CreateControllerUISD(ControllerTypeEnum_Storage, key, true);   
    free(info);

    return KSUCCESS;
}

KResult AddDeviceSrv(thread_t Callback, uint64_t CallbackArg, srv_storage_device_info_t* Info){
    KResult Status = KFAIL;
    storage_device_t* Device = NULL;
    if(Info){
        Status = AddDevice(Info, &Device);
    }
    
    arguments_t arguments{
        .arg[0] = Status,            /* Status */
        .arg[1] = CallbackArg,      /* CallbackArg */
        .arg[2] = (uint64_t)Device, /* DeviceIndex */
        .arg[3] = NULL,             /* GP1 */
        .arg[4] = NULL,             /* GP2 */
        .arg[5] = NULL,             /* GP3 */
    };

    Sys_ExecThread(Callback, &arguments, ExecutionTypeQueu, NULL);
    Sys_Close(KSUCCESS);
}

KResult RemoveDeviceSrv(thread_t Callback, uint64_t CallbackArg, storage_device_t* Device){
    KResult Status = RemoveDevice(Device);
    
    arguments_t arguments{
        .arg[0] = Status,            /* Status */
        .arg[1] = CallbackArg,      /* CallbackArg */
        .arg[2] = NULL,             /* GP0 */
        .arg[3] = NULL,             /* GP1 */
        .arg[4] = NULL,             /* GP2 */
        .arg[5] = NULL,             /* GP3 */
    };

    Sys_ExecThread(Callback, &arguments, ExecutionTypeQueu, NULL);
    Sys_Close(KSUCCESS);
}

KResult NotifyOnNewPartitionByGUIDTypeSrv(thread_t Callback, uint64_t CallbackArg, thread_t ThreadToNotify, process_t ProcessToNotify, GUID_t* PartitionTypeGUID){
    KResult Status = NotifyOnNewPartitionByGUIDType(PartitionTypeGUID, ThreadToNotify, ProcessToNotify);
    
    arguments_t arguments{
        .arg[0] = Status,            /* Status */
        .arg[1] = CallbackArg,      /* CallbackArg */
        .arg[2] = NULL,             /* GP0 */
        .arg[3] = NULL,             /* GP1 */
        .arg[4] = NULL,             /* GP2 */
        .arg[5] = NULL,             /* GP3 */
    };

    Sys_ExecThread(Callback, &arguments, ExecutionTypeQueu, NULL);
    Sys_Close(KSUCCESS);
}