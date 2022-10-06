#include <kot/uisd/srvs/storage.h>

thread_t srv_storage_callback_thread = NULL;
uisd_storage_t* StorageData = NULL;

void Srv_Storage_Initialize(){
    StorageData = (uisd_storage_t*)FindControllerUISD(ControllerTypeEnum_Storage);
    if(StorageData != NULL){
        process_t Proc = Sys_GetProcess();

        thread_t StoragethreadKeyCallback = NULL;
        Sys_Createthread(Proc, &Srv_Storage_Callback, PriviledgeApp, NULL, &StoragethreadKeyCallback);
        srv_storage_callback_thread = MakeShareableThreadToProcess(StoragethreadKeyCallback, StorageData->ControllerHeader.Process);
    }else{
        Sys_Close(KFAIL);
    }
}

void Srv_Storage_Callback(KResult Status, struct srv_storage_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3){
    Callback->Status = Callback->Handler(Status, Callback, GP0, GP1, GP2, GP3);
    if(Callback->IsAwait){
        Sys_Unpause(Callback->Self);
    }
    Sys_Close(KSUCCESS);    
}

/* Add device */

KResult Srv_Storage_AddDevice_Callback(KResult Status, struct srv_storage_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3){
    if(Status == KSUCCESS){
        Callback->Data = GP0;
        Callback->Size = (size64_t)sizeof(uint64_t);
    }
    return Status;
}

struct srv_storage_callback_t* Srv_Storage_AddDevice(struct srv_storage_device_info_t* Info, bool IsAwait){
    if(!srv_storage_callback_thread) Srv_Storage_Initialize();
    
    thread_t self = Sys_Getthread();

    struct srv_storage_callback_t* callback = (struct srv_storage_callback_t*)malloc(sizeof(struct srv_storage_callback_t));
    callback->Self = self;
    callback->Data = NULL;
    callback->Size = NULL;
    callback->IsAwait = IsAwait;
    callback->Status = KBUSY;
    callback->Handler = &Srv_Storage_AddDevice_Callback;

    struct ShareDataWithArguments_t data;
    if(Info != NULL){
        data.Data = Info;
        data.Size = sizeof(struct srv_storage_device_info_t);
        data.ParameterPosition = 0x2; 
    }

    struct arguments_t parameters;
    parameters.arg[0] = srv_storage_callback_thread;
    parameters.arg[1] = callback;
    

    KResult Status = Sys_Execthread(StorageData->AddDevice, &parameters, ExecutionTypeQueu, &data);
    if(Status == KSUCCESS && IsAwait){
        Sys_Pause(false);
    }
    return callback;    
}


/* Remove device */

KResult Srv_Storage_RemoveDevice_Callback(KResult Status, struct srv_storage_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3){
    return Status;
}

struct srv_storage_callback_t* Srv_Storage_RemoveDevice(uint64_t Index, bool IsAwait){
    if(!srv_storage_callback_thread) Srv_Storage_Initialize();
    
    thread_t self = Sys_Getthread();

    struct srv_storage_callback_t* callback = (struct srv_storage_callback_t*)malloc(sizeof(struct srv_storage_callback_t));
    callback->Self = self;
    callback->Data = NULL;
    callback->Size = NULL;
    callback->IsAwait = IsAwait;
    callback->Status = KBUSY;
    callback->Handler = &Srv_Storage_RemoveDevice_Callback;

    struct arguments_t parameters;
    parameters.arg[0] = srv_storage_callback_thread;
    parameters.arg[1] = callback;
    parameters.arg[2] = Index;
    

    KResult Status = Sys_Execthread(StorageData->AddDevice, &parameters, ExecutionTypeQueu, NULL);
    if(Status == KSUCCESS && IsAwait){
        Sys_Pause(false);
    }
    return callback;        
}