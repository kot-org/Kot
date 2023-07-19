#include <stdlib.h>
#include <string.h>
#include <sys/user.h>
#include <sys/ioccom.h>
#include <frg/string.hpp>
#include <frg/vector.hpp>
#include <mlibc/debug.hpp>
#include <kot/uisd/srvs/storage.h>

extern "C" {

kot_thread_t kot_srv_storage_callback_thread = NULL;
kot_uisd_storage_t* StorageData = NULL;

void Srv_Storage_Initialize(){
    StorageData = (kot_uisd_storage_t*)kot_FindControllerUISD(ControllerTypeEnum_Storage);
    if(StorageData != NULL){
        kot_process_t Proc = kot_Sys_GetProcess();

        kot_thread_t StoragethreadKeyCallback = NULL;
        kot_Sys_CreateThread(Proc, (void*)&kot_Srv_Storage_Callback, PriviledgeApp, NULL, &StoragethreadKeyCallback);
        kot_srv_storage_callback_thread = kot_MakeShareableThread(StoragethreadKeyCallback, PriviledgeService);
    }else{
        kot_Sys_Close(KFAIL);
    }
}

void kot_Srv_Storage_Callback(KResult Status, struct kot_srv_storage_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3){
    Callback->Status = Callback->Handler(Status, Callback, GP0, GP1, GP2, GP3);
    if(Callback->IsAwait){
        kot_Sys_Unpause(Callback->Self);
    }
    kot_Sys_Close(KSUCCESS);    
}

/* Add device */

KResult Srv_Storage_AddDevice_Callback(KResult Status, struct kot_srv_storage_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3){
    if(Status == KSUCCESS){
        Callback->Data = GP0;
        Callback->Size = (size64_t)sizeof(uint64_t);
    }
    return Status;
}

struct kot_srv_storage_callback_t* kot_Srv_Storage_AddDevice(struct kot_srv_storage_device_info_t* Info, bool IsAwait){
    if(!kot_srv_storage_callback_thread) Srv_Storage_Initialize();
    
    kot_thread_t self = kot_Sys_GetThread();

    struct kot_srv_storage_callback_t* callback = (struct kot_srv_storage_callback_t*)malloc(sizeof(struct kot_srv_storage_callback_t));
    callback->Self = self;
    callback->Data = NULL;
    callback->Size = NULL;
    callback->IsAwait = IsAwait;
    callback->Status = KBUSY;
    callback->Handler = &Srv_Storage_AddDevice_Callback;

    struct kot_ShareDataWithArguments_t data;
    if(Info != NULL){
        data.Data = (void*)Info;
        data.Size = sizeof(struct kot_srv_storage_device_info_t);
        data.ParameterPosition = 0x2; 
    }

    struct kot_arguments_t parameters;
    parameters.arg[0] = kot_srv_storage_callback_thread;
    parameters.arg[1] = (uint64_t)callback;
    

    KResult Status = kot_Sys_ExecThread(StorageData->AddDevice, &parameters, ExecutionTypeQueu, &data);
    if(Status == KSUCCESS && IsAwait){
        kot_Sys_Pause(false);
    }
    return callback;
}


/* Remove device */

KResult Srv_Storage_RemoveDevice_Callback(KResult Status, struct kot_srv_storage_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3){
    return Status;
}

struct kot_srv_storage_callback_t* kot_Srv_Storage_RemoveDevice(uint64_t Index, bool IsAwait){
    if(!kot_srv_storage_callback_thread) Srv_Storage_Initialize();
    
    kot_thread_t self = kot_Sys_GetThread();

    struct kot_srv_storage_callback_t* callback = (struct kot_srv_storage_callback_t*)malloc(sizeof(struct kot_srv_storage_callback_t));
    callback->Self = self;
    callback->Data = NULL;
    callback->Size = NULL;
    callback->IsAwait = IsAwait;
    callback->Status = KBUSY;
    callback->Handler = &Srv_Storage_RemoveDevice_Callback;

    struct kot_arguments_t parameters;
    parameters.arg[0] = kot_srv_storage_callback_thread;
    parameters.arg[1] = (uint64_t)callback;
    parameters.arg[2] = Index;
    

    KResult Status = kot_Sys_ExecThread(StorageData->RemoveDevice, &parameters, ExecutionTypeQueu, NULL);
    if(Status == KSUCCESS && IsAwait){
        kot_Sys_Pause(false);
    }
    return callback;
}


/* NotifyOnNewPartitionByGUIDType */

KResult Srv_Storage_NotifyOnNewPartitionByGUIDType_Callback(KResult Status, struct kot_srv_storage_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3){
    return Status;
}

struct kot_srv_storage_callback_t* kot_Srv_Storage_NotifyOnNewPartitionByGUIDType(kot_GUID_t* PartitionTypeGUID, kot_thread_t ThreadToNotify, kot_process_t ProcessToNotify, bool IsAwait){
    if(!kot_srv_storage_callback_thread) Srv_Storage_Initialize();
    
    kot_thread_t self = kot_Sys_GetThread();

    kot_thread_t shareabbleThreadToNotify = kot_MakeShareableThreadToProcess(ThreadToNotify, StorageData->ControllerHeader.Process);

    struct kot_srv_storage_callback_t* callback = (struct kot_srv_storage_callback_t*)malloc(sizeof(struct kot_srv_storage_callback_t));
    callback->Self = self;
    callback->Data = shareabbleThreadToNotify;
    callback->Size = NULL;
    callback->IsAwait = IsAwait;
    callback->Status = KBUSY;
    callback->Handler = &Srv_Storage_NotifyOnNewPartitionByGUIDType_Callback;

    struct kot_arguments_t parameters;
    parameters.arg[0] = kot_srv_storage_callback_thread;
    parameters.arg[1] = (uint64_t)callback;
    parameters.arg[2] = shareabbleThreadToNotify;
    parameters.arg[3] = ProcessToNotify;

    struct kot_ShareDataWithArguments_t data;
    data.Data = (void*)PartitionTypeGUID;
    data.Size = sizeof(kot_GUID_t);
    data.ParameterPosition = 0x4;

    KResult Status = kot_Sys_ExecThread(StorageData->NotifyOnNewPartitionByGUIDType, &parameters, ExecutionTypeQueu, &data);
    if(Status == KSUCCESS && IsAwait){
        kot_Sys_Pause(false);
    }
    return callback;
}


/* MountPartition */

KResult Srv_Storage_MountPartition_Callback(KResult Status, struct kot_srv_storage_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3){
    return Status;
}

struct kot_srv_storage_callback_t* kot_Srv_Storage_MountPartition(kot_thread_t VFSMountThread, struct kot_srv_storage_fs_server_functions_t* FSServerFunctions, bool IsAwait){
    if(!kot_srv_storage_callback_thread) Srv_Storage_Initialize();
    
    kot_thread_t self = kot_Sys_GetThread();

    struct kot_srv_storage_callback_t* callback = (struct kot_srv_storage_callback_t*)malloc(sizeof(struct kot_srv_storage_callback_t));
    callback->Self = self;
    callback->Data = NULL;
    callback->Size = NULL;
    callback->IsAwait = IsAwait;
    callback->Status = KBUSY;
    callback->Handler = &Srv_Storage_MountPartition_Callback;

    struct kot_arguments_t parameters;
    parameters.arg[0] = kot_srv_storage_callback_thread;
    parameters.arg[1] = (uint64_t)callback;
    parameters.arg[2] = true; // Is mount function

    struct kot_ShareDataWithArguments_t data;
    data.Data = (void*)FSServerFunctions;
    data.Size = sizeof(struct kot_srv_storage_fs_server_functions_t);
    data.ParameterPosition = 0x3;

    KResult Status = kot_Sys_ExecThread(VFSMountThread, &parameters, ExecutionTypeQueu, &data);
    if(Status == KSUCCESS && IsAwait){
        kot_Sys_Pause(false);
    }
    return callback;
}


/* UnmountPartition */

KResult Srv_Storage_UnmountPartition_Callback(KResult Status, struct kot_srv_storage_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3){
    return Status;
}

struct kot_srv_storage_callback_t* kot_Srv_Storage_UnmountPartition(kot_thread_t VFSUnmountThread, bool IsAwait){
    if(!kot_srv_storage_callback_thread) Srv_Storage_Initialize();
    
    kot_thread_t self = kot_Sys_GetThread();

    struct kot_srv_storage_callback_t* callback = (struct kot_srv_storage_callback_t*)malloc(sizeof(struct kot_srv_storage_callback_t));
    callback->Self = self;
    callback->Data = NULL;
    callback->Size = NULL;
    callback->IsAwait = IsAwait;
    callback->Status = KBUSY;
    callback->Handler = &Srv_Storage_UnmountPartition_Callback;

    struct kot_arguments_t parameters;
    parameters.arg[0] = kot_srv_storage_callback_thread;
    parameters.arg[1] = (uint64_t)callback;
    parameters.arg[2] = false; // Is mount function

    KResult Status = kot_Sys_ExecThread(VFSUnmountThread, &parameters, ExecutionTypeQueu, NULL);
    if(Status == KSUCCESS && IsAwait){
        kot_Sys_Pause(false);
    }
    return callback;
}


/* VFS */


/* VFSLoginApp */

KResult Srv_Storage_VFSLoginApp_Callback(KResult Status, struct kot_srv_storage_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3){
    Callback->Data = GP0;
    return Status;
}

struct kot_srv_storage_callback_t* kot_Srv_Storage_VFSLoginApp(kot_process_t Process, kot_authorization_t Authorization, kot_permissions_t Permissions, char* Path, bool IsAwait){
    if(!kot_srv_storage_callback_thread) Srv_Storage_Initialize();
    
    kot_thread_t self = kot_Sys_GetThread();

    struct kot_srv_storage_callback_t* callback = (struct kot_srv_storage_callback_t*)malloc(sizeof(struct kot_srv_storage_callback_t));
    callback->Self = self;
    callback->Data = NULL;
    callback->Size = NULL;
    callback->IsAwait = IsAwait;
    callback->Status = KBUSY;
    callback->Handler = &Srv_Storage_VFSLoginApp_Callback;

    struct kot_arguments_t parameters;
    parameters.arg[0] = kot_srv_storage_callback_thread;
    parameters.arg[1] = (uint64_t)callback;
    parameters.arg[2] = Process;
    parameters.arg[3] = Authorization;
    parameters.arg[4] = Permissions;

    struct kot_ShareDataWithArguments_t Data;
    Data.Data = (void*)Path;
    Data.Size = strlen(Path) + 1; // add '\0' char
    Data.ParameterPosition = 0x5;

    KResult Status = kot_Sys_ExecThread(StorageData->VFSLoginApp, &parameters, ExecutionTypeQueu, &Data);
    if(Status == KSUCCESS && IsAwait){
        kot_Sys_Pause(false);
    }
    return callback;
}

// TODO Change user data


/* Removefile */

KResult Srv_Storage_Removefile_Callback(KResult Status, struct kot_srv_storage_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3){
    return Status;
}

struct kot_srv_storage_callback_t* kot_Srv_Storage_Removefile(char* Path, bool IsAwait){
    if(!kot_srv_storage_callback_thread) Srv_Storage_Initialize();
    
    kot_thread_t self = kot_Sys_GetThread();

    struct kot_srv_storage_callback_t* callback = (struct kot_srv_storage_callback_t*)malloc(sizeof(struct kot_srv_storage_callback_t));
    callback->Self = self;
    callback->Data = NULL;
    callback->Size = NULL;
    callback->IsAwait = IsAwait;
    callback->Status = KBUSY;
    callback->Handler = &Srv_Storage_MountPartition_Callback;

    struct kot_arguments_t parameters;
    parameters.arg[0] = kot_srv_storage_callback_thread;
    parameters.arg[1] = (uint64_t)callback;
    parameters.arg[2] = Client_VFS_File_Remove;

    struct kot_ShareDataWithArguments_t data;
    data.Data = (void*)Path;
    data.Size = strlen(Path) + 1; // add '\0' char
    data.ParameterPosition = 0x3;

    KResult Status = kot_Sys_ExecThread(KotSpecificData.VFSHandler, &parameters, ExecutionTypeQueu, &data);
    if(Status == KSUCCESS && IsAwait){
        kot_Sys_Pause(false);
    }
    return callback;
}


/* Openfile */

KResult Srv_Storage_Openfile_Callback(KResult Status, struct kot_srv_storage_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3){
    if(Status == KSUCCESS){
        struct kot_srv_storage_fs_server_open_file_data_t* FileData = (struct kot_srv_storage_fs_server_open_file_data_t*)GP0;
        kot_file_t* File = (kot_file_t*)malloc(sizeof(kot_file_t));
        File->FileThreadHandler = FileData->Dispatcher;
        File->FileProcessHandler = FileData->FSDriverProc;
        Callback->Data = (uint64_t)File;
        Callback->Size = sizeof(kot_file_t);
    }
    return Status;
}

struct kot_srv_storage_callback_t* kot_Srv_Storage_Openfile(char* Path, kot_permissions_t Permissions, kot_process_t Target, bool IsAwait){
    if(!kot_srv_storage_callback_thread) Srv_Storage_Initialize();
    
    kot_thread_t self = kot_Sys_GetThread();

    struct kot_srv_storage_callback_t* callback = (struct kot_srv_storage_callback_t*)malloc(sizeof(struct kot_srv_storage_callback_t));
    callback->Self = self;
    callback->Data = NULL;
    callback->Size = NULL;
    callback->IsAwait = IsAwait;
    callback->Status = KBUSY;
    callback->Handler = &Srv_Storage_Openfile_Callback;

    struct kot_arguments_t parameters;
    parameters.arg[0] = kot_srv_storage_callback_thread;
    parameters.arg[1] = (uint64_t)callback;
    parameters.arg[2] = Client_VFS_File_Open;
    parameters.arg[3] = Permissions;
    parameters.arg[5] = Target;

    struct kot_ShareDataWithArguments_t data;
    data.Data = (void*)Path;
    data.Size = strlen(Path) + 1; // add '\0' char
    data.ParameterPosition = 0x4;

    KResult Status = kot_Sys_ExecThread(KotSpecificData.VFSHandler, &parameters, ExecutionTypeQueu, &data);
    if(Status == KSUCCESS && IsAwait){
        kot_Sys_Pause(false);
    }
    return callback;
}


/* Rename */

KResult Srv_Storage_Rename_Callback(KResult Status, struct kot_srv_storage_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3){
    return Status;
}

struct kot_srv_storage_callback_t* kot_Srv_Storage_Rename(char* OldPath, char* NewPath, bool IsAwait){
    if(!kot_srv_storage_callback_thread) Srv_Storage_Initialize();
    
    kot_thread_t self = kot_Sys_GetThread();

    struct kot_srv_storage_callback_t* callback = (struct kot_srv_storage_callback_t*)malloc(sizeof(struct kot_srv_storage_callback_t));
    callback->Self = self;
    callback->Data = NULL;
    callback->Size = NULL;
    callback->IsAwait = IsAwait;
    callback->Status = KBUSY;
    callback->Handler = &Srv_Storage_Rename_Callback;

    struct kot_arguments_t parameters;
    parameters.arg[0] = kot_srv_storage_callback_thread;
    parameters.arg[1] = (uint64_t)callback;
    parameters.arg[2] = Client_VFS_Rename;

    size64_t PathOldSize = strlen(OldPath) + 1;
    size64_t PathNewSize = strlen(NewPath) + 1;

    uint64_t RenameDataSize = sizeof(struct kot_srv_storage_fs_server_rename_t) + PathOldSize + PathNewSize;
    struct kot_srv_storage_fs_server_rename_t* RenameData = (struct kot_srv_storage_fs_server_rename_t*)malloc(RenameDataSize);

    RenameData->OldPathPosition = sizeof(struct kot_srv_storage_fs_server_rename_t);
    RenameData->NewPathPosition = sizeof(struct kot_srv_storage_fs_server_rename_t) + PathOldSize;

    memcpy((void*)((uint64_t)RenameData + RenameData->OldPathPosition), OldPath, PathOldSize);
    memcpy((void*)((uint64_t)RenameData + RenameData->NewPathPosition), NewPath, PathNewSize);

    struct kot_ShareDataWithArguments_t data;
    data.Data = (void*)RenameData;
    data.Size = RenameDataSize; // add '\0' char
    data.ParameterPosition = 0x3;

    KResult Status = kot_Sys_ExecThread(KotSpecificData.VFSHandler, &parameters, ExecutionTypeQueu, &data);
    if(Status == KSUCCESS && IsAwait){
        kot_Sys_Pause(false);
    }
    return callback;
}


/* DirCreate */

KResult Srv_Storage_DirCreate_Callback(KResult Status, struct kot_srv_storage_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3){
    struct kot_srv_storage_fs_server_open_dir_data_t* DirData = (struct kot_srv_storage_fs_server_open_dir_data_t*)GP0;
    kot_directory_t* Dir = (kot_directory_t*)malloc(sizeof(kot_directory_t));
    Dir->DirThreadHandler = DirData->Dispatcher;
    Dir->DirProcessHandler = DirData->FSDriverProc;
    Callback->Data = (uint64_t)Dir;
    Callback->Size = sizeof(kot_file_t);
    return Status;
}

struct kot_srv_storage_callback_t* kot_Srv_Storage_DirCreate(char* Path, mode_t Mode, bool IsAwait){
    if(!kot_srv_storage_callback_thread) Srv_Storage_Initialize();
    
    kot_thread_t self = kot_Sys_GetThread();

    struct kot_srv_storage_callback_t* callback = (struct kot_srv_storage_callback_t*)malloc(sizeof(struct kot_srv_storage_callback_t));
    callback->Self = self;
    callback->Data = NULL;
    callback->Size = NULL;
    callback->IsAwait = IsAwait;
    callback->Status = KBUSY;
    callback->Handler = &Srv_Storage_DirCreate_Callback;

    struct kot_arguments_t parameters;
    parameters.arg[0] = kot_srv_storage_callback_thread;
    parameters.arg[1] = (uint64_t)callback;
    parameters.arg[2] = Client_VFS_Dir_Create;
    parameters.arg[4] = Mode;

    struct kot_ShareDataWithArguments_t data;
    data.Data = (void*)Path;
    data.Size = strlen(Path) + 1; // add '\0' char
    data.ParameterPosition = 0x3;

    KResult Status = kot_Sys_ExecThread(KotSpecificData.VFSHandler, &parameters, ExecutionTypeQueu, &data);
    if(Status == KSUCCESS && IsAwait){
        kot_Sys_Pause(false);
    }
    return callback;
}


/* DirRemove */

KResult Srv_Storage_DirRemove_Callback(KResult Status, struct kot_srv_storage_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3){
    return Status;
}

struct kot_srv_storage_callback_t* kot_Srv_Storage_DirRemove(char* Path, bool IsAwait){
    if(!kot_srv_storage_callback_thread) Srv_Storage_Initialize();
    
    kot_thread_t self = kot_Sys_GetThread();

    struct kot_srv_storage_callback_t* callback = (struct kot_srv_storage_callback_t*)malloc(sizeof(struct kot_srv_storage_callback_t));
    callback->Self = self;
    callback->Data = NULL;
    callback->Size = NULL;
    callback->IsAwait = IsAwait;
    callback->Status = KBUSY;
    callback->Handler = &Srv_Storage_DirRemove_Callback;

    struct kot_arguments_t parameters;
    parameters.arg[0] = kot_srv_storage_callback_thread;
    parameters.arg[1] = (uint64_t)callback;
    parameters.arg[2] = Client_VFS_Dir_Remove;

    struct kot_ShareDataWithArguments_t data;
    data.Data = (void*)Path;
    data.Size = strlen(Path) + 1; // add '\0' char
    data.ParameterPosition = 0x3;

    KResult Status = kot_Sys_ExecThread(KotSpecificData.VFSHandler, &parameters, ExecutionTypeQueu, &data);
    if(Status == KSUCCESS && IsAwait){
        kot_Sys_Pause(false);
    }
    return callback;
}


/* DirOpen */

KResult Srv_Storage_DirOpen_Callback(KResult Status, struct kot_srv_storage_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3){
    if(Status == KSUCCESS){
        struct kot_srv_storage_fs_server_open_dir_data_t* DirData = (struct kot_srv_storage_fs_server_open_dir_data_t*)GP0;
        kot_directory_t* Dir = (kot_directory_t*)malloc(sizeof(kot_directory_t));
        Dir->DirThreadHandler = DirData->Dispatcher;
        Dir->DirProcessHandler = DirData->FSDriverProc;
        Callback->Data = (uint64_t)Dir;
        Callback->Size = sizeof(kot_directory_t);
    }
    return Status;
}

struct kot_srv_storage_callback_t* kot_Srv_Storage_DirOpen(char* Path, kot_process_t Target, bool IsAwait){
    if(!kot_srv_storage_callback_thread) Srv_Storage_Initialize();
    
    kot_thread_t self = kot_Sys_GetThread();

    struct kot_srv_storage_callback_t* callback = (struct kot_srv_storage_callback_t*)malloc(sizeof(struct kot_srv_storage_callback_t));
    callback->Self = self;
    callback->Data = NULL;
    callback->Size = NULL;
    callback->IsAwait = IsAwait;
    callback->Status = KBUSY;
    callback->Handler = &Srv_Storage_DirOpen_Callback;

    struct kot_arguments_t parameters;
    parameters.arg[0] = kot_srv_storage_callback_thread;
    parameters.arg[1] = (uint64_t)callback;
    parameters.arg[2] = Client_VFS_Dir_Open;
    parameters.arg[4] = Target;

    struct kot_ShareDataWithArguments_t data;
    data.Data = (void*)Path;
    data.Size = strlen(Path) + 1; // add '\0' char
    data.ParameterPosition = 0x3;

    KResult Status = kot_Sys_ExecThread(KotSpecificData.VFSHandler, &parameters, ExecutionTypeQueu, &data);
    if(Status == KSUCCESS && IsAwait){
        kot_Sys_Pause(false);
    }
    return callback;
}


/* GetCWD */

KResult Srv_Storage_GetCWD_Callback(KResult Status, struct kot_srv_storage_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3){
    if(Status == KSUCCESS){
        Callback->Data = (uintptr_t)malloc(GP1);
        Callback->Size = GP1;
        memcpy((void*)Callback->Data, (void*)GP0, GP1);
    }
    return Status;
}

struct kot_srv_storage_callback_t* kot_Srv_Storage_GetCWD(bool IsAwait){
    if(!kot_srv_storage_callback_thread) Srv_Storage_Initialize();
    
    kot_thread_t self = kot_Sys_GetThread();

    struct kot_srv_storage_callback_t* callback = (struct kot_srv_storage_callback_t*)malloc(sizeof(struct kot_srv_storage_callback_t));
    callback->Self = self;
    callback->Data = NULL;
    callback->Size = NULL;
    callback->IsAwait = IsAwait;
    callback->Status = KBUSY;
    callback->Handler = &Srv_Storage_GetCWD_Callback;

    struct kot_arguments_t parameters;
    parameters.arg[0] = kot_srv_storage_callback_thread;
    parameters.arg[1] = (uint64_t)callback;
    parameters.arg[2] = Client_VFS_Get_CWD;

    KResult Status = kot_Sys_ExecThread(KotSpecificData.VFSHandler, &parameters, ExecutionTypeQueu, NULL);
    if(Status == KSUCCESS && IsAwait){
        kot_Sys_Pause(false);
    }
    return callback;
}


/* SetCWD */

KResult Srv_Storage_SetCWD_Callback(KResult Status, struct kot_srv_storage_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3){
    return Status;
}

struct kot_srv_storage_callback_t* kot_Srv_Storage_SetCWD(char* Path, bool IsAwait){
    if(!kot_srv_storage_callback_thread) Srv_Storage_Initialize();
    
    kot_thread_t self = kot_Sys_GetThread();

    struct kot_srv_storage_callback_t* callback = (struct kot_srv_storage_callback_t*)malloc(sizeof(struct kot_srv_storage_callback_t));
    callback->Self = self;
    callback->Data = NULL;
    callback->Size = NULL;
    callback->IsAwait = IsAwait;
    callback->Status = KBUSY;
    callback->Handler = &Srv_Storage_SetCWD_Callback;

    struct kot_arguments_t parameters;
    parameters.arg[0] = kot_srv_storage_callback_thread;
    parameters.arg[1] = (uint64_t)callback;
    parameters.arg[2] = Client_VFS_Set_CWD;

    struct kot_ShareDataWithArguments_t data;
    data.Data = (void*)Path;
    data.Size = strlen(Path) + 1; // add '\0' char
    data.ParameterPosition = 0x3;

    KResult Status = kot_Sys_ExecThread(KotSpecificData.VFSHandler, &parameters, ExecutionTypeQueu, &data);
    if(Status == KSUCCESS && IsAwait){
        kot_Sys_Pause(false);
    }
    return callback;
}


/* File specific */

/* Closefile */

KResult Srv_Storage_Closefile_Callback(KResult Status, struct kot_srv_storage_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3){
    return Status;
}

struct kot_srv_storage_callback_t* kot_Srv_Storage_Closefile(kot_file_t* File, bool IsAwait){
    if(!kot_srv_storage_callback_thread) Srv_Storage_Initialize();
    
    kot_thread_t self = kot_Sys_GetThread();

    struct kot_srv_storage_callback_t* callback = (struct kot_srv_storage_callback_t*)malloc(sizeof(struct kot_srv_storage_callback_t));
    callback->Self = self;
    callback->Data = NULL;
    callback->Size = NULL;
    callback->IsAwait = IsAwait;
    callback->Status = KBUSY;
    callback->Handler = &Srv_Storage_Closefile_Callback;

    struct kot_arguments_t parameters;
    parameters.arg[0] = kot_srv_storage_callback_thread;
    parameters.arg[1] = (uint64_t)callback;
    parameters.arg[2] = File_Function_Close;

    KResult Status = kot_Sys_ExecThread(File->FileThreadHandler, &parameters, ExecutionTypeQueu, NULL);
    if(Status == KSUCCESS && IsAwait){
        kot_Sys_Pause(false);
    }
    return callback;
}

/* Getfilesize */

KResult Srv_Storage_Getfilesize_Callback(KResult Status, struct kot_srv_storage_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3){
    if(Status == KSUCCESS){
        Callback->Data = GP0;
        Callback->Size = sizeof(size64_t);
    }
    return Status;
}

struct kot_srv_storage_callback_t* kot_Srv_Storage_Getfilesize(kot_file_t* File, bool IsAwait){
    if(!kot_srv_storage_callback_thread) Srv_Storage_Initialize();
    
    kot_thread_t self = kot_Sys_GetThread();

    struct kot_srv_storage_callback_t* callback = (struct kot_srv_storage_callback_t*)malloc(sizeof(struct kot_srv_storage_callback_t));
    callback->Self = self;
    callback->Data = NULL;
    callback->Size = NULL;
    callback->IsAwait = IsAwait;
    callback->Status = KBUSY;
    callback->Handler = &Srv_Storage_Getfilesize_Callback;

    struct kot_arguments_t parameters;
    parameters.arg[0] = kot_srv_storage_callback_thread;
    parameters.arg[1] = (uint64_t)callback;
    parameters.arg[2] = File_Function_GetSize;

    KResult Status = kot_Sys_ExecThread(File->FileThreadHandler, &parameters, ExecutionTypeQueu, NULL);
    if(Status == KSUCCESS && IsAwait){
        kot_Sys_Pause(false);
    }
    return callback;
}

/* Readfile */

KResult Srv_Storage_Readfile_Callback(KResult Status, struct kot_srv_storage_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3){
    if(Status == KSUCCESS){
        uint64_t MemoryType;
        if(kot_Sys_GetInfoMemoryField((kot_key_mem_t)GP0, &MemoryType, &Callback->Size) == KSUCCESS){
            if(MemoryType == MemoryFieldTypeSendSpaceRO){
                return kot_Sys_AcceptMemoryField(kot_Sys_GetProcess(), (kot_key_mem_t)GP0, (void**)&Callback->Data);
            }
        }
    }
    return Status;
}

struct kot_srv_storage_callback_t* kot_Srv_Storage_Readfile(kot_file_t* File, void* Buffer, uint64_t Start, size64_t Size, bool IsAwait){
    if(!kot_srv_storage_callback_thread) Srv_Storage_Initialize();
    
    kot_thread_t self = kot_Sys_GetThread();

    struct kot_srv_storage_callback_t* callback = (struct kot_srv_storage_callback_t*)malloc(sizeof(struct kot_srv_storage_callback_t));
    callback->Self = self;
    callback->Data = (uint64_t)Buffer;
    callback->Size = NULL;
    callback->IsAwait = IsAwait;
    callback->Status = KBUSY;
    callback->Handler = &Srv_Storage_Readfile_Callback;

    struct kot_arguments_t parameters;
    parameters.arg[0] = kot_srv_storage_callback_thread;
    parameters.arg[1] = (uint64_t)callback;
    parameters.arg[2] = File_Function_Read;
    parameters.arg[3] = Start;
    parameters.arg[4] = Size;

    KResult Status = kot_Sys_ExecThread(File->FileThreadHandler, &parameters, ExecutionTypeQueu, NULL);
    if(Status == KSUCCESS && IsAwait){
        kot_Sys_Pause(false);
    }
    return callback;
}

/* Writefile */

KResult Srv_Storage_Writefile_Callback(KResult Status, struct kot_srv_storage_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3){
    kot_Sys_CloseMemoryField(kot_Sys_GetProcess(), *(kot_key_mem_t*)Callback->Data, *(void**)(Callback->Data + 8));
    return Status;
}

struct kot_srv_storage_callback_t* kot_Srv_Storage_Writefile(kot_file_t* File, void* Buffer, uint64_t Start, size64_t Size, bool IsDataEnd, bool IsAwait){
    if(!kot_srv_storage_callback_thread) Srv_Storage_Initialize();
    
    kot_thread_t self = kot_Sys_GetThread();

    struct kot_srv_storage_callback_t* callback = (struct kot_srv_storage_callback_t*)malloc(sizeof(struct kot_srv_storage_callback_t));
    callback->Self = self;
    callback->Size = NULL;
    callback->Data = (uint64_t)malloc(sizeof(kot_key_mem_t) + sizeof(void*));
    callback->IsAwait = IsAwait;
    callback->Status = KBUSY;
    callback->Handler = &Srv_Storage_Writefile_Callback;

    kot_key_mem_t BufferKey;
    kot_key_mem_t BufferKeyShareable;

    *(kot_key_mem_t*)callback->Data = BufferKey;
    *(void**)(callback->Data + 8) = Buffer;

    kot_Sys_CreateMemoryField(kot_Sys_GetProcess(), Size, &Buffer, &BufferKey, MemoryFieldTypeSendSpaceRO);
    
    kot_Sys_Keyhole_CloneModify(BufferKey, &BufferKeyShareable, File->FileProcessHandler, KeyholeFlagPresent, PriviledgeApp);

    struct kot_arguments_t parameters;
    parameters.arg[0] = kot_srv_storage_callback_thread;
    parameters.arg[1] = (uint64_t)callback;
    parameters.arg[2] = File_Function_Write;
    parameters.arg[3] = BufferKeyShareable;
    parameters.arg[4] = Start;
    parameters.arg[5] = IsDataEnd;

    KResult Status = kot_Sys_ExecThread(File->FileThreadHandler, &parameters, ExecutionTypeQueu, NULL);
    if(Status == KSUCCESS && IsAwait){
        kot_Sys_Pause(false);
    }
    return callback;
}

/* Ioctl */

KResult Srv_Storage_Ioctl_Callback(KResult Status, struct kot_srv_storage_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3){
    if(Status == KSUCCESS){
        Callback->Data = (uintptr_t)GP0;
        Callback->Size = sizeof(int);
    }
    return Status;
}

struct kot_srv_storage_callback_t* kot_Srv_Storage_Ioctl(kot_file_t* File, unsigned long Request, void* Arg, bool IsAwait){
    if(!kot_srv_storage_callback_thread) Srv_Storage_Initialize();
    
    kot_thread_t self = kot_Sys_GetThread();

    struct kot_srv_storage_callback_t* callback = (struct kot_srv_storage_callback_t*)malloc(sizeof(struct kot_srv_storage_callback_t));
    callback->Self = self;
    callback->Size = NULL;
    callback->Data = NULL;
    callback->IsAwait = IsAwait;
    callback->Status = KBUSY;
    callback->Handler = &Srv_Storage_Ioctl_Callback;

    struct kot_arguments_t parameters;
    parameters.arg[0] = kot_srv_storage_callback_thread;
    parameters.arg[1] = (uint64_t)callback;
    parameters.arg[2] = File_Function_Ioctl;
    parameters.arg[3] = (uint64_t)Request;
    parameters.arg[4] = (uint64_t)Arg;


    KResult Status = kot_Sys_ExecThread(File->FileThreadHandler, &parameters, ExecutionTypeQueu, NULL);
    if(Status == KSUCCESS && IsAwait){
        kot_Sys_Pause(false);
    }
    return callback;
}


/* Directory specific */


/* Closedir */

KResult Srv_Storage_Closedir_Callback(KResult Status, struct kot_srv_storage_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3){
    return Status;
}

struct kot_srv_storage_callback_t* kot_Srv_Storage_Closedir(kot_directory_t* Dir, bool IsAwait){
    if(!kot_srv_storage_callback_thread) Srv_Storage_Initialize();
    
    kot_thread_t self = kot_Sys_GetThread();

    struct kot_srv_storage_callback_t* callback = (struct kot_srv_storage_callback_t*)malloc(sizeof(struct kot_srv_storage_callback_t));
    callback->Self = self;
    callback->Data = NULL;
    callback->Size = NULL;
    callback->IsAwait = IsAwait;
    callback->Status = KBUSY;
    callback->Handler = &Srv_Storage_Closedir_Callback;

    struct kot_arguments_t parameters;
    parameters.arg[0] = kot_srv_storage_callback_thread;
    parameters.arg[1] = (uint64_t)callback;
    parameters.arg[2] = Dir_Function_Close;

    KResult Status = kot_Sys_ExecThread(Dir->DirThreadHandler, &parameters, ExecutionTypeQueu, NULL);
    if(Status == KSUCCESS && IsAwait){
        kot_Sys_Pause(false);
    }
    return callback;
}

/* Getdircount */

KResult Srv_Storage_Getdircount_Callback(KResult Status, struct kot_srv_storage_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3){
    if(Status == KSUCCESS){
        Callback->Data = GP0;
        Callback->Size = sizeof(size64_t);
    }
    return Status;
}

struct kot_srv_storage_callback_t* kot_Srv_Storage_Getdircount(kot_directory_t* Dir, bool IsAwait){
    if(!kot_srv_storage_callback_thread) Srv_Storage_Initialize();
    
    kot_thread_t self = kot_Sys_GetThread();

    struct kot_srv_storage_callback_t* callback = (struct kot_srv_storage_callback_t*)malloc(sizeof(struct kot_srv_storage_callback_t));
    callback->Self = self;
    callback->Data = NULL;
    callback->Size = NULL;
    callback->IsAwait = IsAwait;
    callback->Status = KBUSY;
    callback->Handler = &Srv_Storage_Getdircount_Callback;

    struct kot_arguments_t parameters;
    parameters.arg[0] = kot_srv_storage_callback_thread;
    parameters.arg[1] = (uint64_t)callback;
    parameters.arg[2] = Dir_Function_GetCount;

    KResult Status = kot_Sys_ExecThread(Dir->DirThreadHandler, &parameters, ExecutionTypeQueu, NULL);
    if(Status == KSUCCESS && IsAwait){
        kot_Sys_Pause(false);
    }
    return callback;
}

/* Readdir */

KResult Srv_Storage_Readdir_Callback(KResult Status, struct kot_srv_storage_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3){
    if(Status == KSUCCESS){
        Callback->Data = (uint64_t)malloc(GP1);
        Callback->Size = GP1;
        memcpy((void*)Callback->Data, (void*)GP0, GP1);
    }else{
        Callback->Data = NULL;
        Callback->Size = NULL;            
    }
    return Status;
}

struct kot_srv_storage_callback_t* kot_Srv_Storage_Readdir(kot_directory_t* Dir, uint64_t IndexStart, size64_t IndexNumber, bool IsAwait){
    if(!kot_srv_storage_callback_thread) Srv_Storage_Initialize();
    
    kot_thread_t self = kot_Sys_GetThread();

    struct kot_srv_storage_callback_t* callback = (struct kot_srv_storage_callback_t*)malloc(sizeof(struct kot_srv_storage_callback_t));
    callback->Self = self;
    callback->Data = NULL;
    callback->Size = NULL;
    callback->IsAwait = IsAwait;
    callback->Status = KBUSY;
    callback->Handler = &Srv_Storage_Readdir_Callback;

    struct kot_arguments_t parameters;
    parameters.arg[0] = kot_srv_storage_callback_thread;
    parameters.arg[1] = (uint64_t)callback;
    parameters.arg[2] = Dir_Function_Read;
    parameters.arg[3] = IndexStart;
    parameters.arg[4] = IndexNumber;

    KResult Status = kot_Sys_ExecThread(Dir->DirThreadHandler, &parameters, ExecutionTypeQueu, NULL);
    if(Status == KSUCCESS && IsAwait){
        kot_Sys_Pause(false);
    }
    return callback;
}


/* NewDev */

KResult Srv_Storage_NewDev_Callback(KResult Status, struct kot_srv_storage_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3){
    return Status;
}

struct kot_srv_storage_callback_t* kot_Srv_Storage_NewDev(char* Name, struct kot_srv_storage_fs_server_functions_t* FSServerFunctions, bool IsAwait){
    if(!kot_srv_storage_callback_thread) Srv_Storage_Initialize();
    
    kot_thread_t self = kot_Sys_GetThread();

    struct kot_srv_storage_callback_t* callback = (struct kot_srv_storage_callback_t*)malloc(sizeof(struct kot_srv_storage_callback_t));
    callback->Self = self;
    callback->Size = NULL;
    callback->Data = NULL;
    callback->IsAwait = IsAwait;
    callback->Status = KBUSY;
    callback->Handler = &Srv_Storage_MountPartition_Callback;

    struct kot_arguments_t parameters;
    parameters.arg[0] = kot_srv_storage_callback_thread;
    parameters.arg[1] = (uint64_t)callback;

    size64_t SizeBufferArg = sizeof(struct kot_srv_storage_fs_server_functions_t) + strlen(Name) + 1;
    void* BufferArg = (void*)malloc(SizeBufferArg);

    memcpy((void*)BufferArg, FSServerFunctions, sizeof(struct kot_srv_storage_fs_server_functions_t));
    memcpy((void*)(((uint64_t)BufferArg) + sizeof(struct kot_srv_storage_fs_server_functions_t)), Name, strlen(Name) + 1);

    struct kot_ShareDataWithArguments_t data;
    data.Data = BufferArg;
    data.Size = SizeBufferArg;
    data.ParameterPosition = 0x2;

    KResult Status = kot_Sys_ExecThread(StorageData->NewDev, &parameters, ExecutionTypeQueu, &data);
    free((void*)BufferArg);
    if(Status == KSUCCESS && IsAwait){
        kot_Sys_Pause(false);
    }
    return callback;
}

}