#include <kot/uisd/srvs/storage.h>

thread_t srv_storage_callback_thread = NULL;
uisd_storage_t* StorageData = NULL;

void Srv_Storage_Initialize(){
    StorageData = (uisd_storage_t*)FindControllerUISD(ControllerTypeEnum_Storage);
    if(StorageData != NULL){
        process_t Proc = Sys_GetProcess();

        thread_t StoragethreadKeyCallback = NULL;
        Sys_Createthread(Proc, &Srv_Storage_Callback, PriviledgeApp, NULL, &StoragethreadKeyCallback);
        srv_storage_callback_thread = MakeShareableThread(StoragethreadKeyCallback, PriviledgeService);
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
    

    KResult Status = Sys_Execthread(StorageData->RemoveDevice, &parameters, ExecutionTypeQueu, NULL);
    if(Status == KSUCCESS && IsAwait){
        Sys_Pause(false);
    }
    return callback;
}


/* NotifyOnNewPartitionByGUIDType */

KResult Srv_Storage_NotifyOnNewPartitionByGUIDType_Callback(KResult Status, struct srv_storage_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3){
    return Status;
}

struct srv_storage_callback_t* Srv_Storage_NotifyOnNewPartitionByGUIDType(struct GUID_t* PartitionTypeGUID, thread_t ThreadToNotify, process_t ProcessToNotify, bool IsAwait){
    if(!srv_storage_callback_thread) Srv_Storage_Initialize();
    
    thread_t self = Sys_Getthread();

    thread_t shareabbleThreadToNotify = MakeShareableThreadToProcess(ThreadToNotify, StorageData->ControllerHeader.Process);

    struct srv_storage_callback_t* callback = (struct srv_storage_callback_t*)malloc(sizeof(struct srv_storage_callback_t));
    callback->Self = self;
    callback->Data = shareabbleThreadToNotify;
    callback->Size = NULL;
    callback->IsAwait = IsAwait;
    callback->Status = KBUSY;
    callback->Handler = &Srv_Storage_NotifyOnNewPartitionByGUIDType_Callback;

    struct arguments_t parameters;
    parameters.arg[0] = srv_storage_callback_thread;
    parameters.arg[1] = callback;
    parameters.arg[2] = shareabbleThreadToNotify;
    parameters.arg[3] = ProcessToNotify;

    struct ShareDataWithArguments_t data;
    data.Data = PartitionTypeGUID;
    data.Size = sizeof(struct GUID_t);
    data.ParameterPosition = 0x4;

    KResult Status = Sys_Execthread(StorageData->NotifyOnNewPartitionByGUIDType, &parameters, ExecutionTypeQueu, &data);
    if(Status == KSUCCESS && IsAwait){
        Sys_Pause(false);
    }
    return callback;
}


/* MountPartition */

KResult Srv_Storage_MountPartition_Callback(KResult Status, struct srv_storage_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3){
    return Status;
}

struct srv_storage_callback_t* Srv_Storage_MountPartition(thread_t VFSMountThread, struct srv_storage_fs_server_functions_t* FSServerFunctions, bool IsAwait){
    if(!srv_storage_callback_thread) Srv_Storage_Initialize();
    
    thread_t self = Sys_Getthread();

    struct srv_storage_callback_t* callback = (struct srv_storage_callback_t*)malloc(sizeof(struct srv_storage_callback_t));
    callback->Self = self;
    callback->Data = NULL;
    callback->Size = NULL;
    callback->IsAwait = IsAwait;
    callback->Status = KBUSY;
    callback->Handler = &Srv_Storage_MountPartition_Callback;

    struct arguments_t parameters;
    parameters.arg[0] = srv_storage_callback_thread;
    parameters.arg[1] = callback;
    parameters.arg[2] = true; // Is mount function

    struct ShareDataWithArguments_t data;
    data.Data = FSServerFunctions;
    data.Size = sizeof(struct srv_storage_fs_server_functions_t);
    data.ParameterPosition = 0x3;

    KResult Status = Sys_Execthread(VFSMountThread, &parameters, ExecutionTypeQueu, &data);
    if(Status == KSUCCESS && IsAwait){
        Sys_Pause(false);
    }
    return callback;
}


/* UnmountPartition */

KResult Srv_Storage_UnmountPartition_Callback(KResult Status, struct srv_storage_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3){
    return Status;
}

struct srv_storage_callback_t* Srv_Storage_UnmountPartition(thread_t VFSUnmountThread, bool IsAwait){
    if(!srv_storage_callback_thread) Srv_Storage_Initialize();
    
    thread_t self = Sys_Getthread();

    struct srv_storage_callback_t* callback = (struct srv_storage_callback_t*)malloc(sizeof(struct srv_storage_callback_t));
    callback->Self = self;
    callback->Data = NULL;
    callback->Size = NULL;
    callback->IsAwait = IsAwait;
    callback->Status = KBUSY;
    callback->Handler = &Srv_Storage_UnmountPartition_Callback;

    struct arguments_t parameters;
    parameters.arg[0] = srv_storage_callback_thread;
    parameters.arg[1] = callback;
    parameters.arg[2] = false; // Is mount function

    KResult Status = Sys_Execthread(VFSUnmountThread, &parameters, ExecutionTypeQueu, NULL);
    if(Status == KSUCCESS && IsAwait){
        Sys_Pause(false);
    }
    return callback;
}


/* VFS */


/* VFSLoginApp */

KResult Srv_Storage_VFSLoginApp_Callback(KResult Status, struct srv_storage_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3){
    Callback->Data = GP0;
    return Status;
}

struct srv_storage_callback_t* Srv_Storage_VFSLoginApp(process_t Process, authorization_t Authorization, permissions_t Permissions, char* Path, bool IsAwait){
    if(!srv_storage_callback_thread) Srv_Storage_Initialize();
    
    thread_t self = Sys_Getthread();

    struct srv_storage_callback_t* callback = (struct srv_storage_callback_t*)malloc(sizeof(struct srv_storage_callback_t));
    callback->Self = self;
    callback->Data = NULL;
    callback->Size = NULL;
    callback->IsAwait = IsAwait;
    callback->Status = KBUSY;
    callback->Handler = &Srv_Storage_VFSLoginApp_Callback;

    struct arguments_t parameters;
    parameters.arg[0] = srv_storage_callback_thread;
    parameters.arg[1] = callback;
    parameters.arg[2] = Process;
    parameters.arg[3] = Authorization;
    parameters.arg[4] = Permissions;

    struct ShareDataWithArguments_t Data;
    Data.Data = Path;
    Data.Size = strlen(Path) + 1; // add '\0' char
    Data.ParameterPosition = 0x5;

    KResult Status = Sys_Execthread(StorageData->VFSLoginApp, &parameters, ExecutionTypeQueu, &Data);
    if(Status == KSUCCESS && IsAwait){
        Sys_Pause(false);
    }
    return callback;
}

// TODO Change user data


/* Removefile */

KResult Srv_Storage_Removefile_Callback(KResult Status, struct srv_storage_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3){
    return Status;
}

struct srv_storage_callback_t* Srv_Storage_Removefile(char* Path, permissions_t Permissions, bool IsAwait){
    if(!srv_storage_callback_thread) Srv_Storage_Initialize();
    
    thread_t self = Sys_Getthread();

    struct srv_storage_callback_t* callback = (struct srv_storage_callback_t*)malloc(sizeof(struct srv_storage_callback_t));
    callback->Self = self;
    callback->Data = NULL;
    callback->Size = NULL;
    callback->IsAwait = IsAwait;
    callback->Status = KBUSY;
    callback->Handler = &Srv_Storage_MountPartition_Callback;

    struct arguments_t parameters;
    parameters.arg[0] = srv_storage_callback_thread;
    parameters.arg[1] = callback;
    parameters.arg[2] = Client_VFS_File_Remove;

    struct ShareDataWithArguments_t data;
    data.Data = Path;
    data.Size = strlen(Path) + 1; // add '\0' char
    data.ParameterPosition = 0x3;

    KResult Status = Sys_Execthread(KotSpecificData.VFSHandler, &parameters, ExecutionTypeQueu, &data);
    if(Status == KSUCCESS && IsAwait){
        Sys_Pause(false);
    }
    return callback;
}


/* Openfile */

KResult Srv_Storage_Openfile_Callback(KResult Status, struct srv_storage_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3){
    return Status;
}

struct srv_storage_callback_t* Srv_Storage_Openfile(char* Path, permissions_t Permissions, bool IsAwait){
    if(!srv_storage_callback_thread) Srv_Storage_Initialize();
    
    thread_t self = Sys_Getthread();

    struct srv_storage_callback_t* callback = (struct srv_storage_callback_t*)malloc(sizeof(struct srv_storage_callback_t));
    callback->Self = self;
    callback->Data = NULL;
    callback->Size = NULL;
    callback->IsAwait = IsAwait;
    callback->Status = KBUSY;
    callback->Handler = &Srv_Storage_Openfile_Callback;

    struct arguments_t parameters;
    parameters.arg[0] = srv_storage_callback_thread;
    parameters.arg[1] = callback;
    parameters.arg[2] = Client_VFS_File_Remove;

    struct ShareDataWithArguments_t data;
    data.Data = Path;
    data.Size = strlen(Path) + 1; // add '\0' char
    data.ParameterPosition = 0x3;

    KResult Status = Sys_Execthread(KotSpecificData.VFSHandler, &parameters, ExecutionTypeQueu, &data);
    if(Status == KSUCCESS && IsAwait){
        Sys_Pause(false);
    }
    return callback;
}
