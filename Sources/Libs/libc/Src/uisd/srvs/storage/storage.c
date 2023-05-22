#include <stdio.h>

thread_t srv_storage_callback_thread = NULL;
uisd_storage_t* StorageData = NULL;

void Srv_Storage_Initialize(){
    StorageData = (uisd_storage_t*)FindControllerUISD(ControllerTypeEnum_Storage);
    if(StorageData != NULL){
        process_t Proc = Sys_GetProcess();

        thread_t StoragethreadKeyCallback = NULL;
        Sys_CreateThread(Proc, &Srv_Storage_Callback, PriviledgeApp, NULL, &StoragethreadKeyCallback);
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
    
    thread_t self = Sys_GetThread();

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
    

    KResult Status = Sys_ExecThread(StorageData->AddDevice, &parameters, ExecutionTypeQueu, &data);
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
    
    thread_t self = Sys_GetThread();

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
    

    KResult Status = Sys_ExecThread(StorageData->RemoveDevice, &parameters, ExecutionTypeQueu, NULL);
    if(Status == KSUCCESS && IsAwait){
        Sys_Pause(false);
    }
    return callback;
}


/* NotifyOnNewPartitionByGUIDType */

KResult Srv_Storage_NotifyOnNewPartitionByGUIDType_Callback(KResult Status, struct srv_storage_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3){
    return Status;
}

struct srv_storage_callback_t* Srv_Storage_NotifyOnNewPartitionByGUIDType(GUID_t* PartitionTypeGUID, thread_t ThreadToNotify, process_t ProcessToNotify, bool IsAwait){
    if(!srv_storage_callback_thread) Srv_Storage_Initialize();
    
    thread_t self = Sys_GetThread();

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
    data.Size = sizeof(GUID_t);
    data.ParameterPosition = 0x4;

    KResult Status = Sys_ExecThread(StorageData->NotifyOnNewPartitionByGUIDType, &parameters, ExecutionTypeQueu, &data);
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
    
    thread_t self = Sys_GetThread();

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

    KResult Status = Sys_ExecThread(VFSMountThread, &parameters, ExecutionTypeQueu, &data);
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
    
    thread_t self = Sys_GetThread();

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

    KResult Status = Sys_ExecThread(VFSUnmountThread, &parameters, ExecutionTypeQueu, NULL);
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

struct srv_storage_callback_t* Srv_Storage_VFSLoginApp(process_t Process, authorization_t Authorization, kot_permissions_t Permissions, char* Path, bool IsAwait){
    if(!srv_storage_callback_thread) Srv_Storage_Initialize();
    
    thread_t self = Sys_GetThread();

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

    KResult Status = Sys_ExecThread(StorageData->VFSLoginApp, &parameters, ExecutionTypeQueu, &Data);
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

struct srv_storage_callback_t* Srv_Storage_Removefile(char* Path, bool IsAwait){
    if(!srv_storage_callback_thread) Srv_Storage_Initialize();
    
    thread_t self = Sys_GetThread();

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

    KResult Status = Sys_ExecThread(KotSpecificData.VFSHandler, &parameters, ExecutionTypeQueu, &data);
    if(Status == KSUCCESS && IsAwait){
        Sys_Pause(false);
    }
    return callback;
}


/* Openfile */

KResult Srv_Storage_Openfile_Callback(KResult Status, struct srv_storage_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3){
    if(Status == KSUCCESS){
        struct srv_storage_fs_server_open_file_data_t* FileData = (struct srv_storage_fs_server_open_file_data_t*)GP0;
        file_t* File = (file_t*)malloc(sizeof(file_t));
        File->FileThreadHandler = FileData->Dispatcher;
        File->FileProcessHandler = FileData->FSDriverProc;
        Callback->Data = (uint64_t)File;
        Callback->Size = sizeof(file_t);
    }
    return Status;
}

struct srv_storage_callback_t* Srv_Storage_Openfile(char* Path, kot_permissions_t Permissions, process_t Target, bool IsAwait){
    if(!srv_storage_callback_thread) Srv_Storage_Initialize();
    
    thread_t self = Sys_GetThread();

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
    parameters.arg[2] = Client_VFS_File_Open;
    parameters.arg[3] = Permissions;
    parameters.arg[5] = Target;

    struct ShareDataWithArguments_t data;
    data.Data = Path;
    data.Size = strlen(Path) + 1; // add '\0' char
    data.ParameterPosition = 0x4;

    KResult Status = Sys_ExecThread(KotSpecificData.VFSHandler, &parameters, ExecutionTypeQueu, &data);
    if(Status == KSUCCESS && IsAwait){
        Sys_Pause(false);
    }
    return callback;
}


/* Rename */

KResult Srv_Storage_Rename_Callback(KResult Status, struct srv_storage_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3){
    return Status;
}

struct srv_storage_callback_t* Srv_Storage_Rename(char* OldPath, char* NewPath, bool IsAwait){
    if(!srv_storage_callback_thread) Srv_Storage_Initialize();
    
    thread_t self = Sys_GetThread();

    struct srv_storage_callback_t* callback = (struct srv_storage_callback_t*)malloc(sizeof(struct srv_storage_callback_t));
    callback->Self = self;
    callback->Data = NULL;
    callback->Size = NULL;
    callback->IsAwait = IsAwait;
    callback->Status = KBUSY;
    callback->Handler = &Srv_Storage_Rename_Callback;

    struct arguments_t parameters;
    parameters.arg[0] = srv_storage_callback_thread;
    parameters.arg[1] = callback;
    parameters.arg[2] = Client_VFS_Rename;

    size64_t PathOldSize = strlen(OldPath) + 1;
    size64_t PathNewSize = strlen(NewPath) + 1;

    uint64_t RenameDataSize = sizeof(struct srv_storage_fs_server_rename_t) + PathOldSize + PathNewSize;
    struct srv_storage_fs_server_rename_t* RenameData = (struct srv_storage_fs_server_rename_t*)malloc(RenameData);

    RenameData->OldPathPosition = sizeof(struct srv_storage_fs_server_rename_t);
    RenameData->NewPathPosition = sizeof(struct srv_storage_fs_server_rename_t) + PathOldSize;

    memcpy((void*)((uint64_t)RenameData + RenameData->OldPathPosition), OldPath, PathOldSize);
    memcpy((void*)((uint64_t)RenameData + RenameData->NewPathPosition), NewPath, PathNewSize);

    struct ShareDataWithArguments_t data;
    data.Data = RenameData;
    data.Size = RenameDataSize; // add '\0' char
    data.ParameterPosition = 0x3;

    KResult Status = Sys_ExecThread(KotSpecificData.VFSHandler, &parameters, ExecutionTypeQueu, &data);
    if(Status == KSUCCESS && IsAwait){
        Sys_Pause(false);
    }
    return callback;
}


/* DirCreate */

KResult Srv_Storage_DirCreate_Callback(KResult Status, struct srv_storage_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3){
    struct srv_storage_fs_server_open_dir_data_t* DirData = (struct srv_storage_fs_server_open_dir_data_t*)GP0;
    directory_t* Dir = (directory_t*)malloc(sizeof(directory_t));
    Dir->DirThreadHandler = DirData->Dispatcher;
    Dir->DirProcessHandler = DirData->FSDriverProc;
    Callback->Data = (uint64_t)Dir;
    Callback->Size = sizeof(file_t);
    return Status;
}

struct srv_storage_callback_t* Srv_Storage_DirCreate(char* Path, mode_t Mode, bool IsAwait){
    if(!srv_storage_callback_thread) Srv_Storage_Initialize();
    
    thread_t self = Sys_GetThread();

    struct srv_storage_callback_t* callback = (struct srv_storage_callback_t*)malloc(sizeof(struct srv_storage_callback_t));
    callback->Self = self;
    callback->Data = NULL;
    callback->Size = NULL;
    callback->IsAwait = IsAwait;
    callback->Status = KBUSY;
    callback->Handler = &Srv_Storage_DirCreate_Callback;

    struct arguments_t parameters;
    parameters.arg[0] = srv_storage_callback_thread;
    parameters.arg[1] = callback;
    parameters.arg[2] = Client_VFS_Dir_Create;
    parameters.arg[4] = Mode;

    struct ShareDataWithArguments_t data;
    data.Data = Path;
    data.Size = strlen(Path) + 1; // add '\0' char
    data.ParameterPosition = 0x3;

    KResult Status = Sys_ExecThread(KotSpecificData.VFSHandler, &parameters, ExecutionTypeQueu, &data);
    if(Status == KSUCCESS && IsAwait){
        Sys_Pause(false);
    }
    return callback;
}


/* DirRemove */

KResult Srv_Storage_DirRemove_Callback(KResult Status, struct srv_storage_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3){
    return Status;
}

struct srv_storage_callback_t* Srv_Storage_DirRemove(char* Path, bool IsAwait){
    if(!srv_storage_callback_thread) Srv_Storage_Initialize();
    
    thread_t self = Sys_GetThread();

    struct srv_storage_callback_t* callback = (struct srv_storage_callback_t*)malloc(sizeof(struct srv_storage_callback_t));
    callback->Self = self;
    callback->Data = NULL;
    callback->Size = NULL;
    callback->IsAwait = IsAwait;
    callback->Status = KBUSY;
    callback->Handler = &Srv_Storage_DirRemove_Callback;

    struct arguments_t parameters;
    parameters.arg[0] = srv_storage_callback_thread;
    parameters.arg[1] = callback;
    parameters.arg[2] = Client_VFS_Dir_Remove;

    struct ShareDataWithArguments_t data;
    data.Data = Path;
    data.Size = strlen(Path) + 1; // add '\0' char
    data.ParameterPosition = 0x3;

    KResult Status = Sys_ExecThread(KotSpecificData.VFSHandler, &parameters, ExecutionTypeQueu, &data);
    if(Status == KSUCCESS && IsAwait){
        Sys_Pause(false);
    }
    return callback;
}


/* DirOpen */

KResult Srv_Storage_DirOpen_Callback(KResult Status, struct srv_storage_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3){
    if(Status == KSUCCESS){
        struct srv_storage_fs_server_open_dir_data_t* DirData = (struct srv_storage_fs_server_open_dir_data_t*)GP0;
        directory_t* Dir = (file_t*)malloc(sizeof(directory_t));
        Dir->DirThreadHandler = DirData->Dispatcher;
        Dir->DirProcessHandler = DirData->FSDriverProc;
        Callback->Data = (uint64_t)Dir;
        Callback->Size = sizeof(file_t);
    }
    return Status;
}

struct srv_storage_callback_t* Srv_Storage_DirOpen(char* Path, process_t Target, bool IsAwait){
    if(!srv_storage_callback_thread) Srv_Storage_Initialize();
    
    thread_t self = Sys_GetThread();

    struct srv_storage_callback_t* callback = (struct srv_storage_callback_t*)malloc(sizeof(struct srv_storage_callback_t));
    callback->Self = self;
    callback->Data = NULL;
    callback->Size = NULL;
    callback->IsAwait = IsAwait;
    callback->Status = KBUSY;
    callback->Handler = &Srv_Storage_DirOpen_Callback;

    struct arguments_t parameters;
    parameters.arg[0] = srv_storage_callback_thread;
    parameters.arg[1] = callback;
    parameters.arg[2] = Client_VFS_Dir_Open;
    parameters.arg[4] = Target;

    struct ShareDataWithArguments_t data;
    data.Data = Path;
    data.Size = strlen(Path) + 1; // add '\0' char
    data.ParameterPosition = 0x3;

    KResult Status = Sys_ExecThread(KotSpecificData.VFSHandler, &parameters, ExecutionTypeQueu, &data);
    if(Status == KSUCCESS && IsAwait){
        Sys_Pause(false);
    }
    return callback;
}



/* File specific */

/* Closefile */

KResult Srv_Storage_Closefile_Callback(KResult Status, struct srv_storage_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3){
    return Status;
}

struct srv_storage_callback_t* Srv_Storage_Closefile(file_t* File, bool IsAwait){
    if(!srv_storage_callback_thread) Srv_Storage_Initialize();
    
    thread_t self = Sys_GetThread();

    struct srv_storage_callback_t* callback = (struct srv_storage_callback_t*)malloc(sizeof(struct srv_storage_callback_t));
    callback->Self = self;
    callback->Data = NULL;
    callback->Size = NULL;
    callback->IsAwait = IsAwait;
    callback->Status = KBUSY;
    callback->Handler = &Srv_Storage_Closefile_Callback;

    struct arguments_t parameters;
    parameters.arg[0] = srv_storage_callback_thread;
    parameters.arg[1] = callback;
    parameters.arg[2] = File_Function_Close;

    KResult Status = Sys_ExecThread(File->FileThreadHandler, &parameters, ExecutionTypeQueu, NULL);
    if(Status == KSUCCESS && IsAwait){
        Sys_Pause(false);
    }
    return callback;
}

/* Getfilesize */

KResult Srv_Storage_Getfilesize_Callback(KResult Status, struct srv_storage_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3){
    if(Status == KSUCCESS){
        Callback->Data = GP0;
        Callback->Size = sizeof(size64_t);
    }
    return Status;
}

struct srv_storage_callback_t* Srv_Storage_Getfilesize(file_t* File, bool IsAwait){
    if(!srv_storage_callback_thread) Srv_Storage_Initialize();
    
    thread_t self = Sys_GetThread();

    struct srv_storage_callback_t* callback = (struct srv_storage_callback_t*)malloc(sizeof(struct srv_storage_callback_t));
    callback->Self = self;
    callback->Data = NULL;
    callback->Size = NULL;
    callback->IsAwait = IsAwait;
    callback->Status = KBUSY;
    callback->Handler = &Srv_Storage_Getfilesize_Callback;

    struct arguments_t parameters;
    parameters.arg[0] = srv_storage_callback_thread;
    parameters.arg[1] = callback;
    parameters.arg[2] = File_Function_GetSize;

    KResult Status = Sys_ExecThread(File->FileThreadHandler, &parameters, ExecutionTypeQueu, NULL);
    if(Status == KSUCCESS && IsAwait){
        Sys_Pause(false);
    }
    return callback;
}

/* Readfile */

KResult Srv_Storage_Readfile_Callback(KResult Status, struct srv_storage_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3){
    if(Status == KSUCCESS){
        uint64_t MemoryType;
        if(Sys_GetInfoMemoryField((kot_key_mem_t)GP0, &MemoryType, &Callback->Size) == KSUCCESS){
            if(MemoryType == MemoryFieldTypeSendSpaceRO){
                return Sys_AcceptMemoryField(Sys_GetProcess(), (kot_key_mem_t)GP0, &Callback->Data);
            }
        }
    }
    return Status;
}

struct srv_storage_callback_t* Srv_Storage_Readfile(file_t* File, void* Buffer, uint64_t Start, size64_t Size, bool IsAwait){
    if(!srv_storage_callback_thread) Srv_Storage_Initialize();
    
    thread_t self = Sys_GetThread();

    struct srv_storage_callback_t* callback = (struct srv_storage_callback_t*)malloc(sizeof(struct srv_storage_callback_t));
    callback->Self = self;
    callback->Data = Buffer;
    callback->Size = NULL;
    callback->IsAwait = IsAwait;
    callback->Status = KBUSY;
    callback->Handler = &Srv_Storage_Readfile_Callback;

    struct arguments_t parameters;
    parameters.arg[0] = srv_storage_callback_thread;
    parameters.arg[1] = callback;
    parameters.arg[2] = File_Function_Read;
    parameters.arg[3] = Start;
    parameters.arg[4] = Size;

    KResult Status = Sys_ExecThread(File->FileThreadHandler, &parameters, ExecutionTypeQueu, NULL);
    if(Status == KSUCCESS && IsAwait){
        Sys_Pause(false);
    }
    return callback;
}

/* Writefile */

KResult Srv_Storage_Writefile_Callback(KResult Status, struct srv_storage_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3){
    Sys_CloseMemoryField(Sys_GetProcess(), *(kot_key_mem_t*)Callback->Data, *(void**)(Callback->Data + 8));
    return Status;
}

struct srv_storage_callback_t* Srv_Storage_Writefile(file_t* File, void* Buffer, uint64_t Start, size64_t Size, bool IsDataEnd, bool IsAwait){
    if(!srv_storage_callback_thread) Srv_Storage_Initialize();
    
    thread_t self = Sys_GetThread();

    struct srv_storage_callback_t* callback = (struct srv_storage_callback_t*)malloc(sizeof(struct srv_storage_callback_t));
    callback->Self = self;
    callback->Size = NULL;
    callback->Data = malloc(sizeof(kot_key_mem_t) + sizeof(void*));
    callback->IsAwait = IsAwait;
    callback->Status = KBUSY;
    callback->Handler = &Srv_Storage_Readfile_Callback;

    kot_key_mem_t BufferKey;
    kot_key_mem_t BufferKeyShareable;

    *(kot_key_mem_t*)callback->Data = BufferKey;
    *(void**)(callback->Data + 8) = Buffer;

    Sys_CreateMemoryField(Sys_GetProcess(), Size, &Buffer, &BufferKey, MemoryFieldTypeSendSpaceRO);
    
    Sys_Keyhole_CloneModify(BufferKey, &BufferKeyShareable, File->FileProcessHandler, KeyholeFlagPresent, PriviledgeApp);

    struct arguments_t parameters;
    parameters.arg[0] = srv_storage_callback_thread;
    parameters.arg[1] = callback;
    parameters.arg[2] = File_Function_Write;
    parameters.arg[3] = BufferKeyShareable;
    parameters.arg[4] = Start;
    parameters.arg[5] = IsDataEnd;

    KResult Status = Sys_ExecThread(File->FileThreadHandler, &parameters, ExecutionTypeQueu, NULL);
    if(Status == KSUCCESS && IsAwait){
        Sys_Pause(false);
    }
    return callback;
}


/* Directory specific */


/* Closedir */

KResult Srv_Storage_Closedir_Callback(KResult Status, struct srv_storage_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3){
    return Status;
}

struct srv_storage_callback_t* Srv_Storage_Closedir(directory_t* Dir, bool IsAwait){
    if(!srv_storage_callback_thread) Srv_Storage_Initialize();
    
    thread_t self = Sys_GetThread();

    struct srv_storage_callback_t* callback = (struct srv_storage_callback_t*)malloc(sizeof(struct srv_storage_callback_t));
    callback->Self = self;
    callback->Data = NULL;
    callback->Size = NULL;
    callback->IsAwait = IsAwait;
    callback->Status = KBUSY;
    callback->Handler = &Srv_Storage_Closedir_Callback;

    struct arguments_t parameters;
    parameters.arg[0] = srv_storage_callback_thread;
    parameters.arg[1] = callback;
    parameters.arg[2] = Dir_Function_Close;

    KResult Status = Sys_ExecThread(Dir->DirThreadHandler, &parameters, ExecutionTypeQueu, NULL);
    if(Status == KSUCCESS && IsAwait){
        Sys_Pause(false);
    }
    return callback;
}

/* Getdircount */

KResult Srv_Storage_Getdircount_Callback(KResult Status, struct srv_storage_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3){
    if(Status == KSUCCESS){
        Callback->Data = GP0;
        Callback->Size = sizeof(size64_t);
    }
    return Status;
}

struct srv_storage_callback_t* Srv_Storage_Getdircount(directory_t* Dir, bool IsAwait){
    if(!srv_storage_callback_thread) Srv_Storage_Initialize();
    
    thread_t self = Sys_GetThread();

    struct srv_storage_callback_t* callback = (struct srv_storage_callback_t*)malloc(sizeof(struct srv_storage_callback_t));
    callback->Self = self;
    callback->Data = NULL;
    callback->Size = NULL;
    callback->IsAwait = IsAwait;
    callback->Status = KBUSY;
    callback->Handler = &Srv_Storage_Getdircount_Callback;

    struct arguments_t parameters;
    parameters.arg[0] = srv_storage_callback_thread;
    parameters.arg[1] = callback;
    parameters.arg[2] = Dir_Function_GetCount;

    KResult Status = Sys_ExecThread(Dir->DirThreadHandler, &parameters, ExecutionTypeQueu, NULL);
    if(Status == KSUCCESS && IsAwait){
        Sys_Pause(false);
    }
    return callback;
}

/* Readdir */

KResult Srv_Storage_Readdir_Callback(KResult Status, struct srv_storage_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3){
    if(Status == KSUCCESS){
        Callback->Data = malloc(GP1);
        Callback->Size = GP1;
        memcpy(Callback->Data, GP0, GP1);
    }else{
        Callback->Data = NULL;
        Callback->Size = NULL;            
    }
    return Status;
}

struct srv_storage_callback_t* Srv_Storage_Readdir(directory_t* Dir, uint64_t IndexStart, size64_t IndexNumber, bool IsAwait){
    if(!srv_storage_callback_thread) Srv_Storage_Initialize();
    
    thread_t self = Sys_GetThread();

    struct srv_storage_callback_t* callback = (struct srv_storage_callback_t*)malloc(sizeof(struct srv_storage_callback_t));
    callback->Self = self;
    callback->Data = NULL;
    callback->Size = NULL;
    callback->IsAwait = IsAwait;
    callback->Status = KBUSY;
    callback->Handler = &Srv_Storage_Readdir_Callback;

    struct arguments_t parameters;
    parameters.arg[0] = srv_storage_callback_thread;
    parameters.arg[1] = callback;
    parameters.arg[2] = Dir_Function_Read;
    parameters.arg[3] = IndexStart;
    parameters.arg[4] = IndexNumber;

    KResult Status = Sys_ExecThread(Dir->DirThreadHandler, &parameters, ExecutionTypeQueu, NULL);
    if(Status == KSUCCESS && IsAwait){
        Sys_Pause(false);
    }
    return callback;
}


/* NewDev */

KResult Srv_Storage_NewDev_Callback(KResult Status, struct srv_storage_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3){
    return Status;
}

struct srv_storage_callback_t* Srv_Storage_NewDev(char* Name, struct srv_storage_fs_server_functions_t* FSServerFunctions, bool IsAwait){
    if(!srv_storage_callback_thread) Srv_Storage_Initialize();
    
    thread_t self = Sys_GetThread();

    struct srv_storage_callback_t* callback = (struct srv_storage_callback_t*)malloc(sizeof(struct srv_storage_callback_t));
    callback->Self = self;
    callback->Size = NULL;
    callback->Data = NULL;
    callback->IsAwait = IsAwait;
    callback->Status = KBUSY;
    callback->Handler = &Srv_Storage_MountPartition_Callback;

    struct arguments_t parameters;
    parameters.arg[0] = srv_storage_callback_thread;
    parameters.arg[1] = callback;

    size64_t SizeBufferArg = sizeof(struct srv_storage_fs_server_functions_t) + strlen(Name) + 1;
    void* BufferArg = malloc(SizeBufferArg);

    memcpy(BufferArg, FSServerFunctions, sizeof(struct srv_storage_fs_server_functions_t));
    memcpy((void*)(((uint64_t)BufferArg) + sizeof(struct srv_storage_fs_server_functions_t)), Name, strlen(Name) + 1);

    struct ShareDataWithArguments_t data;
    data.Data = BufferArg;
    data.Size = SizeBufferArg;
    data.ParameterPosition = 0x2;

    KResult Status = Sys_ExecThread(StorageData->NewDev, &parameters, ExecutionTypeQueu, &data);
    free(BufferArg);
    if(Status == KSUCCESS && IsAwait){
        Sys_Pause(false);
    }
    return callback;
}