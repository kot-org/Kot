#include <srv/srv.h>

uisd_time_t* SrvData;

KResult InitialiseServer(){
    process_t proc = Sys_GetProcess();

    uintptr_t address = GetFreeAlignedSpace(sizeof(uisd_time_t));
    ksmem_t key = NULL;
    Sys_CreateMemoryField(proc, sizeof(uisd_time_t), &address, &key, MemoryFieldTypeShareSpaceRO);

    SrvData = (uisd_time_t*)address;
    memset(SrvData, 0, sizeof(uisd_time_t)); // Clear data

    SrvData->ControllerHeader.IsReadWrite = false;
    SrvData->ControllerHeader.Version = Time_Srv_Version;
    SrvData->ControllerHeader.VendorID = Kot_VendorID;
    SrvData->ControllerHeader.Type = ControllerTypeEnum_Time;
    SrvData->ControllerHeader.Process = ShareProcessKey(proc);


    /* SetTimePointerKey */
    thread_t SetTimePointerKeyThread = NULL;
    Sys_CreateThread(proc, (uintptr_t)&SetTimePointerKeySrv, PriviledgeApp, NULL, &SetTimePointerKeyThread);
    SrvData->SetTimePointerKey = MakeShareableThread(SetTimePointerKeyThread, PriviledgeDriver);

    /* SetTickPointerKey */
    thread_t SetTickPointerKeyThread = NULL;
    Sys_CreateThread(proc, (uintptr_t)&SetTickPointerKeySrv, PriviledgeApp, NULL, &SetTickPointerKeyThread);
    SrvData->SetTickPointerKey = MakeShareableThread(SetTickPointerKeyThread, PriviledgeDriver);

    CreateControllerUISD(ControllerTypeEnum_Time, key, true);
    return KSUCCESS;
}

KResult SetTimePointerKeySrv(thread_t Callback, uint64_t CallbackArg, ksmem_t TimePointerKey){
    KResult Status = KFAIL;

    SrvData->TimePointerKey = TimePointerKey;
    
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

KResult SetTickPointerKeySrv(thread_t Callback, uint64_t CallbackArg, ksmem_t TickPointerKey, uint64_t TickPeriod){
    KResult Status = KFAIL;

    SrvData->TickPointerKey = TickPointerKey;
    SrvData->TickPeriod = TickPeriod;
    
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