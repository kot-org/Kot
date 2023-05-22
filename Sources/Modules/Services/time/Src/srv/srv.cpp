#include <srv/srv.h>

uisd_time_t* SrvData;

KResult InitialiseServer(){
    process_t proc = Sys_GetProcess();

    void* address = GetFreeAlignedSpace(sizeof(uisd_time_t));
    kot_key_mem_t key = NULL;
    Sys_CreateMemoryField(proc, sizeof(uisd_time_t), &address, &key, MemoryFieldTypeShareSpaceRO);

    SrvData = (uisd_time_t*)address;
    memset(SrvData, 0, sizeof(uisd_time_t)); // Clear data

    SrvData->ControllerHeader.IsReadWrite = false;
    SrvData->ControllerHeader.Version = Time_Srv_Version;
    SrvData->ControllerHeader.VendorID = Kot_VendorID;
    SrvData->ControllerHeader.Type = ControllerTypeEnum_Time;
    SrvData->ControllerHeader.Process = kot_ShareProcessKey(proc);


    /* SetTimePointerKey */
    thread_t SetTimePointerKeyThread = NULL;
    kot_Sys_CreateThread(proc, (void*)&SetTimePointerKeySrv, PriviledgeApp, NULL, &SetTimePointerKeyThread);
    SrvData->SetTimePointerKey = kot_MakeShareableThread(SetTimePointerKeyThread, PriviledgeDriver);

    /* SetTickPointerKey */
    thread_t SetTickPointerKeyThread = NULL;
    kot_Sys_CreateThread(proc, (void*)&SetTickPointerKeySrv, PriviledgeApp, NULL, &SetTickPointerKeyThread);
    SrvData->SetTickPointerKey = kot_MakeShareableThread(SetTickPointerKeyThread, PriviledgeDriver);

    CreateControllerUISD(ControllerTypeEnum_Time, key, true);
    return KSUCCESS;
}

KResult SetTimePointerKeySrv(thread_t Callback, uint64_t CallbackArg, kot_key_mem_t TimePointerKey){
    KResult Status = KFAIL;

    SrvData->TimePointerKey = TimePointerKey;
    
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

KResult SetTickPointerKeySrv(thread_t Callback, uint64_t CallbackArg, kot_key_mem_t TickPointerKey, uint64_t TickPeriod){
    KResult Status = KFAIL;

    SrvData->TickPointerKey = TickPointerKey;
    SrvData->TickPeriod = TickPeriod;
    
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