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

KResult AddDeviceSrv(thread_t ReadWriteThread, uint64_t ReadWriteArg, ksmem_t BufferRW, uint64_t BufferRWAlignement, storage_device_info_t* Info){
    if(Info){
        AddDevice(ReadWriteThread, ReadWriteArg, BufferRW, BufferRWAlignement, Info);
        Sys_Close(KSUCCESS);
    }
    
    Sys_Close(KFAIL);
}

KResult RemoveDeviceSrv(uint64_t Index){
    RemoveDevice(Index);
    Sys_Close(KSUCCESS);
}