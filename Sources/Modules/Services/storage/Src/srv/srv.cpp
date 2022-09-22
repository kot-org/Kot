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

    /* Initialize device handling */
    InitializeDeviceHandling();

    /* AddDevice */
    thread_t AddDeviceThread = NULL;
    Sys_Createthread(proc, (uintptr_t)&AddDeviceSrv, PriviledgeApp, &AddDeviceThread);
    SrvData->AddDevice = MakeShareableThread(AddDeviceThread, PriviledgeDriver);

    /* RemoveDevice */
    thread_t RemoveDeviceThread = NULL;
    Sys_Createthread(proc, (uintptr_t)&RemoveDeviceSrv, PriviledgeApp, &RemoveDeviceThread);
    SrvData->RemoveDevice = MakeShareableThread(RemoveDeviceThread, PriviledgeDriver);
    
    uisd_callbackInfo_t* info = CreateControllerUISD(ControllerTypeEnum_Storage, key, true);   
    free(info); 

    return KSUCCESS;
}

KResult AddDeviceSrv(thread_t Callback, uint64_t CallbackArg, srv_storage_device_info_t* Info){
    KResult Statu = KFAIL;
    uint64_t DeviceIndex = 0;

    if(Info){
        Statu = AddDevice(Info, &DeviceIndex);
    }
    
    arguments_t arguments{
        .arg[0] = Statu,            /* Status */
        .arg[1] = CallbackArg,      /* CallbackArg */
        .arg[2] = DeviceIndex,      /* DeviceIndex */
        .arg[3] = NULL,             /* GP1 */
        .arg[4] = NULL,             /* GP2 */
        .arg[5] = NULL,             /* GP3 */
    };

    Sys_Execthread(Callback, &arguments, ExecutionTypeQueu, NULL);
    Sys_Close(KSUCCESS);
}

KResult RemoveDeviceSrv(thread_t Callback, uint64_t CallbackArg, uint64_t Index){
    KResult Statu = RemoveDevice(Index);
    
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