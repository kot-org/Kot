#include <srv/srv.h>

uisd_storage_t* SrvData;

KResult InitialiseSrv(){
    process_t proc = Sys_GetProcess();

    uintptr_t address = getFreeAlignedSpace(sizeof(uisd_storage_t));
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
    Sys_Createthread(proc, (uintptr_t)&AddDeviceSrv, PriviledgeApp, NULL, &AddDeviceThread);
    SrvData->AddDevice = MakeShareableThread(AddDeviceThread, PriviledgeDriver);

    /* RemoveDevice */
    thread_t RemoveDeviceThread = NULL;
    Sys_Createthread(proc, (uintptr_t)&RemoveDeviceSrv, PriviledgeApp, NULL, &RemoveDeviceThread);
    SrvData->RemoveDevice = MakeShareableThread(RemoveDeviceThread, PriviledgeDriver);


    /* GetPartitionToMountNumber */
    thread_t GetPartitionToMountNumberThread = NULL;
    Sys_Createthread(proc, (uintptr_t)&GetPartitionToMountNumberSrv, PriviledgeApp, NULL, &GetPartitionToMountNumberThread);
    SrvData->GetPartitionToMountNumber = MakeShareableThread(GetPartitionToMountNumberThread, PriviledgeDriver);

    /* GetPartitionToMountAccess */
    thread_t GetPartitionToMountAccessThread = NULL;
    Sys_Createthread(proc, (uintptr_t)&GetPartitionToMountAccessSrv, PriviledgeApp, NULL, &GetPartitionToMountAccessThread);
    SrvData->GetPartitionToMountAccess = MakeShareableThread(GetPartitionToMountAccessThread, PriviledgeDriver);
    
    uisd_callbackInfo_t* info = CreateControllerUISD(ControllerTypeEnum_Storage, key, true);   
    free(info); 

    return KSUCCESS;
}

KResult AddDeviceSrv(thread_t Callback, uint64_t CallbackArg, srv_storage_device_info_t* Info){
    KResult Statu = KFAIL;
    storage_device_t* Device = NULL;
    if(Info){
        Statu = AddDevice(Info, &Device);
    }
    
    arguments_t arguments{
        .arg[0] = Statu,            /* Status */
        .arg[1] = CallbackArg,      /* CallbackArg */
        .arg[2] = (uint64_t)Device, /* DeviceIndex */
        .arg[3] = NULL,             /* GP1 */
        .arg[4] = NULL,             /* GP2 */
        .arg[5] = NULL,             /* GP3 */
    };

    Sys_Execthread(Callback, &arguments, ExecutionTypeQueu, NULL);
    Sys_Close(KSUCCESS);
}

KResult RemoveDeviceSrv(thread_t Callback, uint64_t CallbackArg, storage_device_t* Device){
    KResult Statu = RemoveDevice(Device);
    
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

KResult GetPartitionToMountNumberSrv(thread_t Callback, uint64_t CallbackArg){
    KResult Statu = KFAIL;
    
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

KResult GetPartitionToMountAccessSrv(thread_t Callback, uint64_t CallbackArg){
    KResult Statu = KFAIL;
    
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