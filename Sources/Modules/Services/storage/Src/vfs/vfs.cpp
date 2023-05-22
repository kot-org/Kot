#include <vfs/vfs.h>

kot_process_t VFSProcess;

static client_vfs_dispatch_t VFSClientDispatcherFunctions[Client_VFS_Function_Count] = {
    [Client_VFS_File_Remove] = (client_vfs_dispatch_t)VFSFileRemove,
    [Client_VFS_File_Open] = (client_vfs_dispatch_t)VFSFileOpen,
    [Client_VFS_Rename] = (client_vfs_dispatch_t)VFSRename,
    [Client_VFS_Dir_Create] = (client_vfs_dispatch_t)VFSDirCreate,
    [Client_VFS_Dir_Remove] = (client_vfs_dispatch_t)VFSDirRemove,
    [Client_VFS_Dir_Open] = (client_vfs_dispatch_t)VFSDirOpen,
};

KResult InitializeVFS(){
    VFSProcess = kot_ShareProcessKey(kot_Sys_GetProcess());

    // RootPartition is initrd
    partition_t* RootPartition = (partition_t*)malloc(sizeof(partition_t));
    RootPartition->IsMount = true;
    RootPartition->StaticVolumeMountPoint = 0;
    RootPartition->DynamicVolumeMountPoint = 0;
    RootPartition->Index = 0;
    kot_Sys_CreateThread(kot_Sys_GetProcess(), (void*)&VFSfileOpenInitrd, PriviledgeService, NULL, &RootPartition->FSServerFunctions.Openfile);
    
    kot_vector_push(PartitionsList, RootPartition);

    return KSUCCESS;
}

KResult ReadContextFile(ClientVFSContext* Context){
    return KSUCCESS;
}

KResult WriteContextFile(ClientVFSContext* Context){
    return KSUCCESS;
}

KResult VFSAskForAuthorization(ClientVFSContext* Context, kot_authorization_t authorization){
    char Message[1024];

    if(authorization == FS_AUTHORIZATION_MEDIUM){
        sprintf(Message, " wants to access to file system with medium access : the software will be abble to read and write ALL FILES in the volume : s%x and d%x", Context->StaticVolumeMountPoint, Context->DynamicVolumeMountPoint);
    }else{
        sprintf(Message, " wants to access to file system with high access : the software will be abble to read and write ALL FILES in your PC");
    }

    kot_validation_field_t ValidationsFields[] = {
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

    kot_autorization_field_t AuthorizationField{
        .PID = kot_Sys_GetPIDThreadLauncher(),
        .Title = "File system access",
        .Message = Message,
        .ValidationFieldsCount = VFSValidationFieldsCount,
        .ValidationFields = (kot_validation_field_t*)malloc(sizeof(kot_validation_field_t) * VFSValidationFieldsCount)
    };
    memcpy(AuthorizationField.ValidationFields, ValidationsFields, sizeof(kot_validation_field_t) * VFSValidationFieldsCount);

    if(kot_GetAuthorization(&AuthorizationField, true) == KSUCCESS){
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
    }else if(RelativePathStart == 0){
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
            PartitionContext = (partition_t*)kot_vector_get(PartitionsList, Volume);
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
    if(Path[0] == '/'){
        if(strncmp(Path, DEV_PATH, DEV_PATH_LEN)){
            // OS Services
            return GetDevAccessData(RelativePath, Partition, Context, Path);
        }else{
            // Otherwise valid path can't begin with /
            return KFAIL;
        }
    }

    std::StringBuilder* Sb = new std::StringBuilder(Path);
    int64_t RelativePathStart = Sb->indexOf(":");

    partition_t* PartitionContext;
    if(RelativePathStart == -1){
        PartitionContext = Context->Partition;
        Sb->append(Context->Path, 0);
        *RelativePath = Sb->toString();
    }else if(RelativePathStart == 0){
        free(Sb);
        return KFAIL;
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
                kot_authorization_t AuthorizationNeed = (Volume == Context->StaticVolumeMountPoint) ? FS_AUTHORIZATION_MEDIUM : FS_AUTHORIZATION_HIGH;
                if(AuthorizationNeed > Context->Authorization){
                    if(Volume > PartitionsList->length) return KNOTALLOW;

                    if(VFSAskForAuthorization(Context, AuthorizationNeed) != KSUCCESS){
                        free(Sb);
                        free(*RelativePath);
                        free(AccessTypeBuffer);
                        return KNOTALLOW;
                    }
                }
                PartitionContext = (partition_t*)kot_vector_get(PartitionsList, Volume);
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

KResult VFSMount(kot_thread_t Callback, uint64_t CallbackArg, bool IsMount, kot_srv_storage_fs_server_functions_t* StorageFSServerFunctions){
    KResult Status = KFAIL;

    partition_t* Partition = (partition_t*)kot_Sys_GetExternalDataThread();
    if(IsMount){
        memcpy(&Partition->FSServerFunctions, StorageFSServerFunctions, sizeof(kot_srv_storage_fs_server_functions_t));
        Status = MountPartition(Partition->Index);
    }else{
        Status = UnmountPartition(Partition->Index);
    }
    
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

KResult VFSLoginApp(kot_thread_t Callback, uint64_t CallbackArg, kot_process_t Process, kot_authorization_t Authorization, kot_permissions_t Permissions, char* Path){
    KResult Status = KFAIL;
    ClientVFSContext* Context = (ClientVFSContext*)malloc(sizeof(ClientVFSContext));
    Context->Authorization = Authorization;
    Context->Permissions = Permissions;
    Context->PathLength = NULL;
    kot_thread_t VFSClientShareableDispatcherThread = NULL;
    if(GetVFSAbsolutePath(&Context->Path, &Context->Partition, Path) == KSUCCESS){
        if(Context->Path){
            Context->PathLength = strlen(Context->Path);
            Context->StaticVolumeMountPoint = Context->Partition->StaticVolumeMountPoint;
            Context->DynamicVolumeMountPoint = Context->Partition->DynamicVolumeMountPoint;

            /* VFSClientDispatcher */
            kot_thread_t VFSClientDispatcherThread = NULL;
            kot_Sys_CreateThread(kot_Sys_GetProcess(), (void*)&VFSClientDispatcher, PriviledgeApp, (uint64_t)Context, &VFSClientDispatcherThread);
            VFSClientShareableDispatcherThread = kot_MakeShareableThreadToProcess(VFSClientDispatcherThread, Process);
        }
    }
    
    kot_arguments_t arguments{
        .arg[0] = Status,                               /* Status */
        .arg[1] = CallbackArg,                          /* CallbackArg */
        .arg[2] = VFSClientShareableDispatcherThread,   /* VFSClientShareableDispatcherThread */
        .arg[3] = NULL,                                 /* GP1 */
        .arg[4] = NULL,                                 /* GP2 */
        .arg[5] = NULL,                                 /* GP3 */
    };

    kot_Sys_ExecThread(Callback, &arguments, ExecutionTypeQueu, NULL);
    kot_Sys_Close(KSUCCESS);    
}


// TODO : ChangeUserData

KResult VFSClientDispatcher(kot_thread_t Callback, uint64_t CallbackArg, uint64_t Function, uint64_t GP0, uint64_t GP1, uint64_t GP2){
    if(Function >= Client_VFS_Function_Count){
        kot_arguments_t arguments{
            .arg[0] = KFAIL,            /* Status */
            .arg[1] = CallbackArg,      /* CallbackArg */
            .arg[2] = NULL,             /* GP0 */
            .arg[3] = NULL,             /* GP1 */
            .arg[4] = NULL,             /* GP2 */
            .arg[5] = NULL,             /* GP3 */
        };

        kot_Sys_ExecThread(Callback, &arguments, ExecutionTypeQueu, NULL);

        kot_Sys_Close(KSUCCESS); 
    }
    
    ClientVFSContext* Context = (ClientVFSContext*)kot_Sys_GetExternalDataThread();
    
    KResult Status = VFSClientDispatcherFunctions[Function](Callback, CallbackArg, Context, Context->Permissions, GP0, GP1, GP2);

    if(Status != KSUCCESS){
        kot_arguments_t arguments{
            .arg[0] = Status,           /* Status */
            .arg[1] = CallbackArg,      /* CallbackArg */
            .arg[2] = NULL,             /* GP0 */
            .arg[3] = NULL,             /* GP1 */
            .arg[4] = NULL,             /* GP2 */
            .arg[5] = NULL,             /* GP3 */
        };

        kot_Sys_ExecThread(Callback, &arguments, ExecutionTypeQueu, NULL);
    }
    kot_Sys_Close(KSUCCESS);
}

KResult VFSFileRemove(kot_thread_t Callback, uint64_t CallbackArg, ClientVFSContext* Context, kot_permissions_t Permissions, char* Path){
    partition_t* Partition;
    char* RelativePath;

    KResult Status = GetVFSAccessData(&RelativePath, &Partition, Context, Path);

    if(Status != KSUCCESS){
        return Status; 
    }
    
    kot_arguments_t arguments{
        .arg[0] = Callback,         /* Callback */
        .arg[1] = CallbackArg,      /* CallbackArg */
        .arg[2] = NULL,             /* Path */
        .arg[3] = Permissions,      /* Permissions */
        .arg[4] = NULL,             /* GP2 */
        .arg[5] = NULL,             /* GP3 */
    };

    kot_ShareDataWithArguments_t Data{
        .Data = RelativePath,
        .Size = (size64_t)strlen(RelativePath) + 1,
        .ParameterPosition = 0x2,
    };

    Status = kot_Sys_ExecThread(Partition->FSServerFunctions.Removefile, &arguments, ExecutionTypeQueu, &Data);

    free(RelativePath);
    
    return Status; 
}

KResult VFSFileOpen(kot_thread_t Callback, uint64_t CallbackArg, ClientVFSContext* Context, kot_permissions_t PermissionsContext, kot_permissions_t Permissions, char* Path, kot_process_t Target){
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
    
    kot_arguments_t arguments{
        .arg[0] = Callback,         /* Callback */
        .arg[1] = CallbackArg,      /* CallbackArg */
        .arg[2] = NULL,             /* Path */
        .arg[3] = Permissions,      /* Permissions */
        .arg[4] = Target,           /* Target */
        .arg[5] = NULL,             /* GP3 */
    };

    kot_ShareDataWithArguments_t Data{
        .Data = RelativePath,
        .Size = (size64_t)strlen(RelativePath) + 1,
        .ParameterPosition = 0x2,
    };
    Status = kot_Sys_ExecThread(Partition->FSServerFunctions.Openfile, &arguments, ExecutionTypeQueu, &Data);
    free(RelativePath);
    
    return Status; 
}


KResult VFSRename(kot_thread_t Callback, uint64_t CallbackArg, ClientVFSContext* Context, kot_permissions_t Permissions, kot_srv_storage_fs_server_rename_t* RenameData){
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

    uint64_t RelativeRenameDataSize = sizeof(kot_srv_storage_fs_server_rename_t) + RelativePathOldSize + RelativePathNewSize;
    kot_srv_storage_fs_server_rename_t* RelativeRenameData = (kot_srv_storage_fs_server_rename_t*)malloc(RelativeRenameDataSize);

    RelativeRenameData->OldPathPosition = sizeof(kot_srv_storage_fs_server_rename_t);
    RelativeRenameData->NewPathPosition = sizeof(kot_srv_storage_fs_server_rename_t) + RelativePathOldSize;

    memcpy((void*)((uint64_t)RelativeRenameData + RelativeRenameData->OldPathPosition), RelativePathOld, RelativePathOldSize);
    memcpy((void*)((uint64_t)RelativeRenameData + RelativeRenameData->NewPathPosition), RelativePathNew, RelativePathNewSize);
    
    kot_arguments_t arguments{
        .arg[0] = Callback,         /* Callback */
        .arg[1] = CallbackArg,      /* CallbackArg */
        .arg[2] = NULL,             /* Path */
        .arg[3] = Permissions,      /* Permissions */
        .arg[4] = NULL,             /* GP2 */
        .arg[5] = NULL,             /* GP3 */
    };

    kot_ShareDataWithArguments_t Data{
        .Data = &RelativeRenameData,
        .Size = (uint64_t)RelativeRenameDataSize,
        .ParameterPosition = 0x2,
    };
    

    Status = kot_Sys_ExecThread(PartitionOld->FSServerFunctions.Rename, &arguments, ExecutionTypeQueu, &Data);

    free(RelativePathOld);
    free(RelativePathNew);
    free(RelativeRenameData);
    
    return Status; 
}

KResult VFSDirCreate(kot_thread_t Callback, uint64_t CallbackArg, ClientVFSContext* Context, kot_permissions_t Permissions, char* Path, mode_t Mode){
    partition_t* Partition;
    char* RelativePath;

    KResult Status = GetVFSAccessData(&RelativePath, &Partition, Context, Path);

    if(Status != KSUCCESS){
        return Status; 
    }
    
    kot_arguments_t arguments{
        .arg[0] = Callback,         /* Callback */
        .arg[1] = CallbackArg,      /* CallbackArg */
        .arg[2] = NULL,             /* Path */
        .arg[3] = Mode,             /* Mode */
        .arg[4] = Permissions,      /* Permissions */
        .arg[5] = NULL,             /* GP3 */
    };

    kot_ShareDataWithArguments_t Data{
        .Data = RelativePath,
        .Size = (size64_t)strlen(RelativePath) + 1,
        .ParameterPosition = 0x2,
    };

    Status = kot_Sys_ExecThread(Partition->FSServerFunctions.Mkdir, &arguments, ExecutionTypeQueu, &Data);

    free(RelativePath);
    
    return Status; 
}

KResult VFSDirRemove(kot_thread_t Callback, uint64_t CallbackArg, ClientVFSContext* Context, kot_permissions_t Permissions, char* Path){
    partition_t* Partition;
    char* RelativePath;

    KResult Status = GetVFSAccessData(&RelativePath, &Partition, Context, Path);

    if(Status != KSUCCESS){
        return Status; 
    }
    
    kot_arguments_t arguments{
        .arg[0] = Callback,         /* Callback */
        .arg[1] = CallbackArg,      /* CallbackArg */
        .arg[2] = NULL,             /* Path */
        .arg[3] = Permissions,      /* Permissions */
        .arg[4] = NULL,             /* GP2 */
        .arg[5] = NULL,             /* GP3 */
    };

    kot_ShareDataWithArguments_t Data{
        .Data = RelativePath,
        .Size = (size64_t)strlen(RelativePath) + 1,
        .ParameterPosition = 0x2,
    };

    Status = kot_Sys_ExecThread(Partition->FSServerFunctions.Rmdir, &arguments, ExecutionTypeQueu, &Data);

    free(RelativePath);
    
    return Status; 
}

KResult VFSDirOpen(kot_thread_t Callback, uint64_t CallbackArg, ClientVFSContext* Context, kot_permissions_t Permissions, char* Path, kot_process_t Target){
    partition_t* Partition;
    char* RelativePath;

    KResult Status = GetVFSAccessData(&RelativePath, &Partition, Context, Path);

    if(Status != KSUCCESS){
        return Status; 
    }
    
    kot_arguments_t arguments{
        .arg[0] = Callback,         /* Callback */
        .arg[1] = CallbackArg,      /* CallbackArg */
        .arg[2] = NULL,             /* Path */
        .arg[3] = Permissions,      /* Permissions */
        .arg[4] = Target,           /* Target */
        .arg[5] = NULL,             /* GP3 */
    };

    if(RelativePath){
        kot_ShareDataWithArguments_t Data{
            .Data = RelativePath,
            .Size = (size64_t)strlen(RelativePath) + 1,
            .ParameterPosition = 0x2,
        };

        Status = kot_Sys_ExecThread(Partition->FSServerFunctions.Opendir, &arguments, ExecutionTypeQueu, &Data);
        free(RelativePath);
    }else{
        Status = kot_Sys_ExecThread(Partition->FSServerFunctions.Opendir, &arguments, ExecutionTypeQueu, NULL);
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
    kot_process_t Target;
    char* Path;
};

KResult VFSfileDispatcherInitrd(kot_thread_t Callback, uint64_t CallbackArg, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3){
    uint32_t Function = GP0;

    if(Function >= File_Function_Count || Function == File_Function_Write){
        kot_arguments_t arguments{
            .arg[0] = KFAIL,            /* Status */
            .arg[1] = CallbackArg,      /* CallbackArg */
            .arg[2] = NULL,             /* GP0 */
            .arg[3] = NULL,             /* GP1 */
            .arg[4] = NULL,             /* GP2 */
            .arg[5] = NULL,             /* GP3 */
        };

        kot_Sys_ExecThread(Callback, &arguments, ExecutionTypeQueu, NULL);
        kot_Sys_Close(KSUCCESS);
    }

    InitrdContext* Context = (InitrdContext*)kot_Sys_GetExternalDataThread();
    kot_Sys_Close(FileDispatcher[Function](Callback, CallbackArg, Context, GP1, GP2, GP3));
}

KResult VFSfileReadInitrd(kot_thread_t Callback, uint64_t CallbackArg,  InitrdContext* Context, uint64_t GP0, uint64_t GP1, uint64_t GP2){
    size64_t Size = GP1;

    kot_srv_system_callback_t* CallbackSys = kot_Srv_System_ReadFileInitrd(Context->Path, true);

    KResult Status = CallbackSys->Status;

    kot_arguments_t arguments{
        .arg[0] = Status,           /* Status */
        .arg[1] = CallbackArg,      /* CallbackArg */
        .arg[2] = NULL,             /* GP0 */
        .arg[3] = NULL,             /* GP1 */
        .arg[4] = NULL,             /* GP2 */
        .arg[5] = NULL,             /* GP3 */
    };

    void* Buffer = (void*)(CallbackSys->Data + GP0);

    kot_key_mem_t MemoryKey;
    kot_Sys_CreateMemoryField(kot_Sys_GetProcess(), Size, &Buffer, &MemoryKey, MemoryFieldTypeSendSpaceRO);

    kot_Sys_Keyhole_CloneModify(MemoryKey, &arguments.arg[2], Context->Target, KeyholeFlagPresent | KeyholeFlagCloneable | KeyholeFlagEditable, PriviledgeApp);
    
    kot_Sys_ExecThread(Callback, &arguments, ExecutionTypeQueuAwait, NULL);
    kot_Sys_CloseMemoryField(kot_Sys_GetProcess(), MemoryKey, Buffer);

    free((void*)CallbackSys->Data);
    free(CallbackSys);

    return KSUCCESS;
}

KResult VFSGetfilesizeInitrd(kot_thread_t Callback, uint64_t CallbackArg,  InitrdContext* Context, uint64_t GP0, uint64_t GP1, uint64_t GP2){
    kot_srv_system_callback_t* CallbackSys = kot_Srv_System_ReadFileInitrd(Context->Path, true);

    KResult Status = CallbackSys->Status;

    kot_arguments_t arguments{
        .arg[0] = Status,               /* Status */
        .arg[1] = CallbackArg,          /* CallbackArg */
        .arg[2] = CallbackSys->Size,    /* Fielsize */
        .arg[3] = NULL,                 /* GP1 */
        .arg[4] = NULL,                 /* GP2 */
        .arg[5] = NULL,                 /* GP3 */
    };

    kot_Sys_ExecThread(Callback, &arguments, ExecutionTypeQueuAwait, NULL);

    free((void*)CallbackSys->Data);
    free(CallbackSys);

    return KSUCCESS;
}

KResult VFSfileCloseInitrd(kot_thread_t Callback, uint64_t CallbackArg,  InitrdContext* Context, uint64_t GP0, uint64_t GP1, uint64_t GP2){
    free(Context->Path);
    free(Context);

    kot_arguments_t arguments{
        .arg[0] = KSUCCESS,            /* Status */
        .arg[1] = CallbackArg,      /* CallbackArg */
        .arg[2] = NULL,             /* GP0 */
        .arg[3] = NULL,             /* GP1 */
        .arg[4] = NULL,             /* GP2 */
        .arg[5] = NULL,             /* GP3 */
    };

    kot_Sys_ExecThread(Callback, &arguments, ExecutionTypeQueu, NULL);

    kot_Sys_Exit(KSUCCESS);
}

KResult VFSfileOpenInitrd(kot_thread_t Callback, uint64_t CallbackArg, char* Path, kot_permissions_t Permissions, kot_process_t Target){
    kot_srv_system_callback_t* CallbackSys = kot_Srv_System_ReadFileInitrd(Path, true);
    KResult Status = CallbackSys->Status;
    free((void*)CallbackSys->Data);
    free(CallbackSys);

    
    kot_arguments_t arguments{
        .arg[0] = Status,           /* Status */
        .arg[1] = CallbackArg,      /* CallbackArg */
        .arg[2] = NULL,             /* Data */
        .arg[3] = NULL,             /* GP1 */
        .arg[4] = NULL,             /* GP2 */
        .arg[5] = NULL,             /* GP3 */
    };

    if(Status == KSUCCESS){
        size64_t FilePathSize = strlen(Path) + 1;
        InitrdContext* Context = (InitrdContext*)malloc(sizeof(InitrdContext));
        Context->Target = Target;
        Context->Path = (char*)malloc(FilePathSize);
        memcpy(Context->Path, Path, FilePathSize);

        kot_srv_storage_fs_server_open_file_data_t SrvOpenFileData;
        kot_thread_t DispatcherThread;

        kot_Sys_CreateThread(kot_Sys_GetProcess(), (void*)&VFSfileDispatcherInitrd, PriviledgeDriver, (uint64_t)Context, &DispatcherThread);

        SrvOpenFileData.Dispatcher = kot_MakeShareableThreadToProcess(DispatcherThread, Target);

        SrvOpenFileData.FSDriverProc = VFSProcess;
        
        kot_ShareDataWithArguments_t ShareDataWithArguments{
            .Data = &SrvOpenFileData,
            .Size = sizeof(kot_srv_storage_fs_server_open_file_data_t),
            .ParameterPosition = 0x2,
        };
        kot_Sys_ExecThread(Callback, &arguments, ExecutionTypeQueu, &ShareDataWithArguments);
    }else{
        kot_Sys_ExecThread(Callback, &arguments, ExecutionTypeQueu, NULL);
    }
    kot_Sys_Close(KSUCCESS);
}