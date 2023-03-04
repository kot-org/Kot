#include <vfs/vfs.h>

process_t VFSProcess;

static client_vfs_dispatch_t VFSClientDispatcherFunctions[Client_VFS_Function_Count] = {
    [Client_VFS_File_Remove] = (client_vfs_dispatch_t)VFSFileRemove,
    [Client_VFS_File_Open] = (client_vfs_dispatch_t)VFSFileOpen,
    [Client_VFS_Rename] = (client_vfs_dispatch_t)VFSRename,
    [Client_VFS_Dir_Create] = (client_vfs_dispatch_t)VFSDirCreate,
    [Client_VFS_Dir_Remove] = (client_vfs_dispatch_t)VFSDirRemove,
    [Client_VFS_Dir_Open] = (client_vfs_dispatch_t)VFSDirOpen,
};

KResult InitializeVFS(){
    VFSProcess = ShareProcessKey(Sys_GetProcess());

    // RootPartition is initrd
    partition_t* RootPartition = (partition_t*)malloc(sizeof(partition_t));
    RootPartition->IsMount = true;
    RootPartition->StaticVolumeMountPoint = 0;
    RootPartition->DynamicVolumeMountPoint = 0;
    RootPartition->Index = 0;
    Sys_CreateThread(Sys_GetProcess(), (uintptr_t)&VFSfileOpenInitrd, PriviledgeService, NULL, &RootPartition->FSServerFunctions.Openfile);
    
    vector_push(PartitionsList, RootPartition);

    return KSUCCESS;
}

KResult ReadContextFile(ClientVFSContext* Context){
    return KSUCCESS;
}

KResult WriteContextFile(ClientVFSContext* Context){
    return KSUCCESS;
}

KResult VFSAskForAuthorization(ClientVFSContext* Context, authorization_t authorization){
    char Message[1024];

    if(authorization == FS_AUTHORIZATION_MEDIUM){
        sprintf(Message, " wants to access to file system with medium access : the software will be abble to read and write ALL FILES in the volume : s%x and d%x", Context->StaticVolumeMountPoint, Context->DynamicVolumeMountPoint);
    }else{
        sprintf(Message, " wants to access to file system with high access : the software will be abble to read and write ALL FILES in your PC");
    }

    validation_field_t ValidationsFields[] = {
        {
            .Value = "Allow and save my choice",
            .Radio = 0,
            .IsValidate = false
        },
        {
            .Value = "Allow for this session only",
            .Radio = 0,
            .IsValidate = false 
        }
    };

    autorization_field_t AuthorizationField{
        .PID = Sys_GetPIDThreadLauncher(),
        .Title = "File system access",
        .Message = Message,
        .ValidationFieldsCount = VFSValidationFieldsCount,
        .ValidationFields = (validation_field_t*)malloc(sizeof(validation_field_t) * VFSValidationFieldsCount)
    };
    memcpy(AuthorizationField.ValidationFields, ValidationsFields, sizeof(validation_field_t) * VFSValidationFieldsCount);

    if(GetAuthorization(&AuthorizationField, true) == KSUCCESS){
        if(AuthorizationField.ValidationFields[0].IsValidate){
            Context->Authorization = authorization;
            WriteContextFile(Context);
            return KSUCCESS;
        }else if(AuthorizationField.ValidationFields[1].IsValidate){
            Context->Authorization = authorization;
            return KSUCCESS;
        }
    }
    return KFAIL;
}

KResult GetVFSAbsolutePath(char** AbsolutePath, partition_t** Partition, char* Path){
    std::StringBuilder* Sb = new std::StringBuilder(Path);
    int64_t RelativePathStart = Sb->indexOf(":");

    partition_t* PartitionContext;

    if(RelativePathStart == -1){
        free(Sb);
        return KFAIL;
    }else{
        char* AccessTypeBuffer = Sb->substr(0, 1);
        char* VolumeBuffer = Sb->substr(1, RelativePathStart);
        *AbsolutePath = Sb->substr(RelativePathStart + 1, Sb->length());

        char AccessType = *AccessTypeBuffer;
        uint64_t Volume = atoi(VolumeBuffer);

        free(VolumeBuffer);

        if(*AccessTypeBuffer == 's'){
            // TODO
            assert(0);
        }else if(*AccessTypeBuffer == 'd'){
            if(Volume >= PartitionsList->length){
                free(Sb);
                free(*AbsolutePath);
                free(AccessTypeBuffer);
                return KFAIL;                
            }
            PartitionContext = (partition_t*)vector_get(PartitionsList, Volume);
            if(!PartitionContext->IsMount){
                free(Sb);
                free(*AbsolutePath);
                free(AccessTypeBuffer);
                return KFAIL;
            }
        }else{
            free(Sb);
            free(*AbsolutePath);
            free(AccessTypeBuffer);
            return KFAIL;
        }
        free(AccessTypeBuffer);
    }
    *Partition = PartitionContext;
    free(Sb);
    return KSUCCESS;
}

KResult GetVFSAccessData(char** RelativePath, partition_t** Partition, ClientVFSContext* Context, char* Path){
    std::StringBuilder* Sb = new std::StringBuilder(Path);
    int64_t RelativePathStart = Sb->indexOf(":");

    partition_t* PartitionContext;
    if(RelativePathStart == -1){
        PartitionContext = Context->Partition;
        Sb->append(Context->Path, 0);
        *RelativePath = Sb->toString();
    }else{
        char* AccessTypeBuffer = Sb->substr(0, 1);
        char* VolumeBuffer = Sb->substr(1, RelativePathStart);
        *RelativePath = Sb->substr(RelativePathStart + 1, Sb->length());

        char AccessType = *AccessTypeBuffer;
        uint64_t Volume = atoi(VolumeBuffer);

        free(VolumeBuffer);

        if(*AccessTypeBuffer == 's'){
            // TODO
            assert(0);
        }else if(*AccessTypeBuffer == 'd'){
            if(Context->DynamicVolumeMountPoint == Volume && strncmp(*RelativePath, Context->Path, Context->PathLength)){
                PartitionContext = Context->Partition;
            }else{
                if(Volume >= PartitionsList->length){
                    free(Sb);
                    free(*RelativePath);
                    free(AccessTypeBuffer);
                    return KFAIL;                
                }
                authorization_t AuthorizationNeed = (Volume == Context->StaticVolumeMountPoint) ? FS_AUTHORIZATION_MEDIUM : FS_AUTHORIZATION_HIGH;
                if(AuthorizationNeed > Context->Authorization){
                    if(Volume > PartitionsList->length) return KNOTALLOW;

                    if(VFSAskForAuthorization(Context, AuthorizationNeed) != KSUCCESS){
                        free(Sb);
                        free(*RelativePath);
                        free(AccessTypeBuffer);
                        return KNOTALLOW;
                    }
                }
                PartitionContext = (partition_t*)vector_get(PartitionsList, Volume);
                if(!PartitionContext->IsMount){
                    free(Sb);
                    free(*RelativePath);
                    free(AccessTypeBuffer);
                    return KFAIL;
                }
            }
        }else{
            free(Sb);
            free(*RelativePath);
            free(AccessTypeBuffer);
            return KFAIL;
        }
        free(AccessTypeBuffer);
    }

    *Partition = PartitionContext;
    free(Sb);
    return KSUCCESS;
}

KResult VFSMount(thread_t Callback, uint64_t CallbackArg, bool IsMount, srv_storage_fs_server_functions_t* StorageFSServerFunctions){
    KResult Status = KFAIL;

    partition_t* Partition = (partition_t*)Sys_GetExternalDataThread();
    if(IsMount){
        memcpy(&Partition->FSServerFunctions, StorageFSServerFunctions, sizeof(srv_storage_fs_server_functions_t));
        Status = MountPartition(Partition->Index);
    }else{
        Status = UnmountPartition(Partition->Index);
    }
    
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

KResult VFSLoginApp(thread_t Callback, uint64_t CallbackArg, process_t Process, authorization_t Authorization, permissions_t Permissions, char* Path){
    KResult Status = KFAIL;
    ClientVFSContext* Context = (ClientVFSContext*)malloc(sizeof(ClientVFSContext));
    Context->Authorization = Authorization;
    Context->Permissions = Permissions;
    Context->PathLength = NULL;
    GetVFSAbsolutePath(&Context->Path, &Context->Partition, Path);
    Context->PathLength = strlen(Context->Path);
    Context->StaticVolumeMountPoint = Context->Partition->StaticVolumeMountPoint;
    Context->DynamicVolumeMountPoint = Context->Partition->DynamicVolumeMountPoint;


    /* VFSClientDispatcher */
    thread_t VFSClientDispatcherThread = NULL;
    Sys_CreateThread(Sys_GetProcess(), (uintptr_t)&VFSClientDispatcher, PriviledgeApp, (uint64_t)Context, &VFSClientDispatcherThread);
    thread_t VFSClientShareableDispatcherThread = MakeShareableThreadToProcess(VFSClientDispatcherThread, Process);
    
    arguments_t arguments{
        .arg[0] = Status,                               /* Status */
        .arg[1] = CallbackArg,                          /* CallbackArg */
        .arg[2] = VFSClientShareableDispatcherThread,   /* VFSClientShareableDispatcherThread */
        .arg[3] = NULL,                                 /* GP1 */
        .arg[4] = NULL,                                 /* GP2 */
        .arg[5] = NULL,                                 /* GP3 */
    };

    Sys_ExecThread(Callback, &arguments, ExecutionTypeQueu, NULL);
    Sys_Close(KSUCCESS);    
}


// TODO : ChangeUserData

KResult VFSClientDispatcher(thread_t Callback, uint64_t CallbackArg, uint64_t Function, uint64_t GP0, uint64_t GP1, uint64_t GP2){
    if(Function >= Client_VFS_Function_Count){
        arguments_t arguments{
            .arg[0] = KFAIL,            /* Status */
            .arg[1] = CallbackArg,      /* CallbackArg */
            .arg[2] = NULL,             /* GP0 */
            .arg[3] = NULL,             /* GP1 */
            .arg[4] = NULL,             /* GP2 */
            .arg[5] = NULL,             /* GP3 */
        };

        Sys_ExecThread(Callback, &arguments, ExecutionTypeQueu, NULL);

        Sys_Close(KSUCCESS); 
    }
    
    ClientVFSContext* Context = (ClientVFSContext*)Sys_GetExternalDataThread();
    
    KResult Status = VFSClientDispatcherFunctions[Function](Callback, CallbackArg, Context, Context->Permissions, GP0, GP1, GP2);

    if(Status != KSUCCESS){
        arguments_t arguments{
            .arg[0] = Status,           /* Status */
            .arg[1] = CallbackArg,      /* CallbackArg */
            .arg[2] = NULL,             /* GP0 */
            .arg[3] = NULL,             /* GP1 */
            .arg[4] = NULL,             /* GP2 */
            .arg[5] = NULL,             /* GP3 */
        };

        Sys_ExecThread(Callback, &arguments, ExecutionTypeQueu, NULL);
    }
    Sys_Close(KSUCCESS);
}

KResult VFSFileRemove(thread_t Callback, uint64_t CallbackArg, ClientVFSContext* Context, permissions_t Permissions, char* Path){
    partition_t* Partition;
    char* RelativePath;

    KResult Status = GetVFSAccessData(&RelativePath, &Partition, Context, Path);

    if(Status != KSUCCESS){
        return Status; 
    }
    
    arguments_t arguments{
        .arg[0] = Callback,         /* Callback */
        .arg[1] = CallbackArg,      /* CallbackArg */
        .arg[2] = NULL,             /* Path */
        .arg[3] = Permissions,      /* Permissions */
        .arg[4] = NULL,             /* GP2 */
        .arg[5] = NULL,             /* GP3 */
    };

    ShareDataWithArguments_t Data{
        .Data = RelativePath,
        .Size = (size64_t)strlen(RelativePath) + 1,
        .ParameterPosition = 0x2,
    };

    Status = Sys_ExecThread(Partition->FSServerFunctions.Removefile, &arguments, ExecutionTypeQueu, &Data);

    free(RelativePath);
    
    return Status; 
}

KResult VFSFileOpen(thread_t Callback, uint64_t CallbackArg, ClientVFSContext* Context, permissions_t PermissionsContext, permissions_t Permissions, char* Path, process_t Target){
    partition_t* Partition;
    char* RelativePath;

    KResult Status = GetVFSAccessData(&RelativePath, &Partition, Context, Path);
    if(Status != KSUCCESS){
        return Status; 
    }

    if(PermissionsContext & 0b1){
        BIT_SET(Permissions, 0);
    }else{
        BIT_CLEAR(Permissions, 0);
    }

    Permissions = Permissions & PermissionsContext;
    
    arguments_t arguments{
        .arg[0] = Callback,         /* Callback */
        .arg[1] = CallbackArg,      /* CallbackArg */
        .arg[2] = NULL,             /* Path */
        .arg[3] = Permissions,      /* Permissions */
        .arg[4] = Target,           /* Target */
        .arg[5] = NULL,             /* GP3 */
    };

    ShareDataWithArguments_t Data{
        .Data = RelativePath,
        .Size = (size64_t)strlen(RelativePath) + 1,
        .ParameterPosition = 0x2,
    };
    Status = Sys_ExecThread(Partition->FSServerFunctions.Openfile, &arguments, ExecutionTypeQueu, &Data);
    free(RelativePath);
    
    return Status; 
}


KResult VFSRename(thread_t Callback, uint64_t CallbackArg, ClientVFSContext* Context, permissions_t Permissions, srv_storage_fs_server_rename_t* RenameData){
    partition_t* PartitionOld;
    partition_t* PartitionNew;
    
    char* RelativePathOld;
    char* RelativePathNew;

    char* OldPath = (char*)((uint64_t)RenameData + RenameData->OldPathPosition);
    char* NewPath = (char*)((uint64_t)RenameData + RenameData->NewPathPosition);

    KResult Status = GetVFSAccessData(&RelativePathOld, &PartitionOld, Context, OldPath);

    if(Status != KSUCCESS){
        return Status; 
    }
    
    Status = GetVFSAccessData(&RelativePathNew, &PartitionNew, Context, OldPath);

    if(Status != KSUCCESS){
        return Status; 
    }

    if(PartitionOld != PartitionNew){
        return KFAIL;
    }
    
    size64_t RelativePathOldSize = strlen(RelativePathOld) + 1;
    size64_t RelativePathNewSize = strlen(RelativePathNew) + 1;

    uint64_t RelativeRenameDataSize = sizeof(srv_storage_fs_server_rename_t) + RelativePathOldSize + RelativePathNewSize;
    srv_storage_fs_server_rename_t* RelativeRenameData = (srv_storage_fs_server_rename_t*)malloc(RelativeRenameDataSize);

    RelativeRenameData->OldPathPosition = sizeof(srv_storage_fs_server_rename_t);
    RelativeRenameData->NewPathPosition = sizeof(srv_storage_fs_server_rename_t) + RelativePathOldSize;

    memcpy((uintptr_t)((uint64_t)RelativeRenameData + RelativeRenameData->OldPathPosition), RelativePathOld, RelativePathOldSize);
    memcpy((uintptr_t)((uint64_t)RelativeRenameData + RelativeRenameData->NewPathPosition), RelativePathNew, RelativePathNewSize);
    
    arguments_t arguments{
        .arg[0] = Callback,         /* Callback */
        .arg[1] = CallbackArg,      /* CallbackArg */
        .arg[2] = NULL,             /* Path */
        .arg[3] = Permissions,      /* Permissions */
        .arg[4] = NULL,             /* GP2 */
        .arg[5] = NULL,             /* GP3 */
    };

    ShareDataWithArguments_t Data{
        .Data = &RelativeRenameData,
        .Size = (uint64_t)RelativeRenameDataSize,
        .ParameterPosition = 0x2,
    };
    

    Status = Sys_ExecThread(PartitionOld->FSServerFunctions.Rename, &arguments, ExecutionTypeQueu, &Data);

    free(RelativePathOld);
    free(RelativePathNew);
    free(RelativeRenameData);
    
    return Status; 
}

KResult VFSDirCreate(thread_t Callback, uint64_t CallbackArg, ClientVFSContext* Context, permissions_t Permissions, char* Path, mode_t Mode){
    partition_t* Partition;
    char* RelativePath;

    KResult Status = GetVFSAccessData(&RelativePath, &Partition, Context, Path);

    if(Status != KSUCCESS){
        return Status; 
    }
    
    arguments_t arguments{
        .arg[0] = Callback,         /* Callback */
        .arg[1] = CallbackArg,      /* CallbackArg */
        .arg[2] = NULL,             /* Path */
        .arg[3] = Mode,             /* Mode */
        .arg[4] = Permissions,      /* Permissions */
        .arg[5] = NULL,             /* GP3 */
    };

    ShareDataWithArguments_t Data{
        .Data = RelativePath,
        .Size = (size64_t)strlen(RelativePath) + 1,
        .ParameterPosition = 0x2,
    };

    Status = Sys_ExecThread(Partition->FSServerFunctions.Mkdir, &arguments, ExecutionTypeQueu, &Data);

    free(RelativePath);
    
    return Status; 
}

KResult VFSDirRemove(thread_t Callback, uint64_t CallbackArg, ClientVFSContext* Context, permissions_t Permissions, char* Path){
    partition_t* Partition;
    char* RelativePath;

    KResult Status = GetVFSAccessData(&RelativePath, &Partition, Context, Path);

    if(Status != KSUCCESS){
        return Status; 
    }
    
    arguments_t arguments{
        .arg[0] = Callback,         /* Callback */
        .arg[1] = CallbackArg,      /* CallbackArg */
        .arg[2] = NULL,             /* Path */
        .arg[3] = Permissions,      /* Permissions */
        .arg[4] = NULL,             /* GP2 */
        .arg[5] = NULL,             /* GP3 */
    };

    ShareDataWithArguments_t Data{
        .Data = RelativePath,
        .Size = (size64_t)strlen(RelativePath) + 1,
        .ParameterPosition = 0x2,
    };

    Status = Sys_ExecThread(Partition->FSServerFunctions.Rmdir, &arguments, ExecutionTypeQueu, &Data);

    free(RelativePath);
    
    return Status; 
}

KResult VFSDirOpen(thread_t Callback, uint64_t CallbackArg, ClientVFSContext* Context, permissions_t Permissions, char* Path, process_t Target){
    partition_t* Partition;
    char* RelativePath;

    KResult Status = GetVFSAccessData(&RelativePath, &Partition, Context, Path);

    if(Status != KSUCCESS){
        return Status; 
    }
    
    arguments_t arguments{
        .arg[0] = Callback,         /* Callback */
        .arg[1] = CallbackArg,      /* CallbackArg */
        .arg[2] = NULL,             /* Path */
        .arg[3] = Permissions,      /* Permissions */
        .arg[4] = Target,           /* Target */
        .arg[5] = NULL,             /* GP3 */
    };

    if(RelativePath){
        ShareDataWithArguments_t Data{
            .Data = RelativePath,
            .Size = (size64_t)strlen(RelativePath) + 1,
            .ParameterPosition = 0x2,
        };

        Status = Sys_ExecThread(Partition->FSServerFunctions.Opendir, &arguments, ExecutionTypeQueu, &Data);
        free(RelativePath);
    }else{
        Status = Sys_ExecThread(Partition->FSServerFunctions.Opendir, &arguments, ExecutionTypeQueu, NULL);
    }
    
    return Status; 
}


/* Initrd converter */


static file_dispatch_t FileDispatcher[File_Function_Count] = { 
    [File_Function_Close] = VFSfileCloseInitrd,
    [File_Function_GetSize] = VFSGetfilesizeInitrd,
    [File_Function_Read] = VFSfileReadInitrd,
};

struct InitrdContext{
    process_t Target;
    char* Path;
};

KResult VFSfileDispatcherInitrd(thread_t Callback, uint64_t CallbackArg, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3){
    uint32_t Function = GP0;

    if(Function >= File_Function_Count || Function == File_Function_Write){
        arguments_t arguments{
            .arg[0] = KFAIL,            /* Status */
            .arg[1] = CallbackArg,      /* CallbackArg */
            .arg[2] = NULL,             /* GP0 */
            .arg[3] = NULL,             /* GP1 */
            .arg[4] = NULL,             /* GP2 */
            .arg[5] = NULL,             /* GP3 */
        };

        Sys_ExecThread(Callback, &arguments, ExecutionTypeQueu, NULL);
        Sys_Close(KSUCCESS);
    }

    InitrdContext* Context = (InitrdContext*)Sys_GetExternalDataThread();
    Sys_Close(FileDispatcher[Function](Callback, CallbackArg, Context, GP1, GP2, GP3));
}

KResult VFSfileReadInitrd(thread_t Callback, uint64_t CallbackArg,  InitrdContext* Context, uint64_t GP0, uint64_t GP1, uint64_t GP2){
    size64_t Size = GP1;

    srv_system_callback_t* CallbackSys = Srv_System_ReadFileInitrd(Context->Path, true);

    KResult Status = CallbackSys->Status;

    arguments_t arguments{
        .arg[0] = Status,           /* Status */
        .arg[1] = CallbackArg,      /* CallbackArg */
        .arg[2] = NULL,             /* GP0 */
        .arg[3] = NULL,             /* GP1 */
        .arg[4] = NULL,             /* GP2 */
        .arg[5] = NULL,             /* GP3 */
    };

    uintptr_t Buffer = (uintptr_t)(CallbackSys->Data + GP0);

    ksmem_t MemoryKey;
    Sys_CreateMemoryField(Sys_GetProcess(), Size, &Buffer, &MemoryKey, MemoryFieldTypeSendSpaceRO);

    Sys_Keyhole_CloneModify(MemoryKey, &arguments.arg[2], Context->Target, KeyholeFlagPresent | KeyholeFlagCloneable | KeyholeFlagEditable, PriviledgeApp);
    
    Sys_ExecThread(Callback, &arguments, ExecutionTypeQueuAwait, NULL);
    Sys_CloseMemoryField(Sys_GetProcess(), MemoryKey, Buffer);

    free((uintptr_t)CallbackSys->Data);
    free(CallbackSys);

    return KSUCCESS;
}

KResult VFSGetfilesizeInitrd(thread_t Callback, uint64_t CallbackArg,  InitrdContext* Context, uint64_t GP0, uint64_t GP1, uint64_t GP2){
    srv_system_callback_t* CallbackSys = Srv_System_ReadFileInitrd(Context->Path, true);

    KResult Status = CallbackSys->Status;

    arguments_t arguments{
        .arg[0] = Status,               /* Status */
        .arg[1] = CallbackArg,          /* CallbackArg */
        .arg[2] = CallbackSys->Size,    /* Fielsize */
        .arg[3] = NULL,                 /* GP1 */
        .arg[4] = NULL,                 /* GP2 */
        .arg[5] = NULL,                 /* GP3 */
    };

    Sys_ExecThread(Callback, &arguments, ExecutionTypeQueuAwait, NULL);

    free((uintptr_t)CallbackSys->Data);
    free(CallbackSys);

    return KSUCCESS;
}

KResult VFSfileCloseInitrd(thread_t Callback, uint64_t CallbackArg,  InitrdContext* Context, uint64_t GP0, uint64_t GP1, uint64_t GP2){
    free(Context->Path);
    free(Context);

    arguments_t arguments{
        .arg[0] = KSUCCESS,            /* Status */
        .arg[1] = CallbackArg,      /* CallbackArg */
        .arg[2] = NULL,             /* GP0 */
        .arg[3] = NULL,             /* GP1 */
        .arg[4] = NULL,             /* GP2 */
        .arg[5] = NULL,             /* GP3 */
    };

    Sys_ExecThread(Callback, &arguments, ExecutionTypeQueu, NULL);

    Sys_Exit(KSUCCESS);
}

KResult VFSfileOpenInitrd(thread_t Callback, uint64_t CallbackArg, char* Path, permissions_t Permissions, process_t Target){
    KResult Status = KSUCCESS; // TODO: verify if the file exists

    size64_t FilePathSize = strlen(Path) + 1;
    InitrdContext* Context = (InitrdContext*)malloc(sizeof(InitrdContext));
    Context->Target = Target;
    Context->Path = (char*)malloc(FilePathSize);
    memcpy(Context->Path, Path, FilePathSize);
    
    arguments_t arguments{
        .arg[0] = Status,           /* Status */
        .arg[1] = CallbackArg,      /* CallbackArg */
        .arg[2] = NULL,             /* Data */
        .arg[3] = NULL,             /* GP1 */
        .arg[4] = NULL,             /* GP2 */
        .arg[5] = NULL,             /* GP3 */
    };

    if(Status == KSUCCESS){
        srv_storage_fs_server_open_file_data_t SrvOpenFileData;
        thread_t DispatcherThread;

        Sys_CreateThread(Sys_GetProcess(), (uintptr_t)&VFSfileDispatcherInitrd, PriviledgeDriver, (uint64_t)Context, &DispatcherThread);

        SrvOpenFileData.Dispatcher = MakeShareableThreadToProcess(DispatcherThread, Target);

        SrvOpenFileData.FSDriverProc = VFSProcess;
        
        ShareDataWithArguments_t ShareDataWithArguments{
            .Data = &SrvOpenFileData,
            .Size = sizeof(srv_storage_fs_server_open_file_data_t),
            .ParameterPosition = 0x2,
        };
        Sys_ExecThread(Callback, &arguments, ExecutionTypeQueu, &ShareDataWithArguments);
    }else{
        free(Context->Path);
        free(Context);
        Sys_ExecThread(Callback, &arguments, ExecutionTypeQueu, NULL);
    }
    Sys_Close(KSUCCESS);
}