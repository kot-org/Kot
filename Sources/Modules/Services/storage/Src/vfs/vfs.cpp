#include <vfs/vfs.h>

process_t VFSProcess;

static client_vfs_dispatch_t VFSClientDispatcherFunctions[Client_VFS_Function_Count] = {
    [Client_VFS_File_Remove] = (client_vfs_dispatch_t)VFSFileRemove,
    [Client_VFS_File_OPEN] = (client_vfs_dispatch_t)VFSFileOpen,
    [Client_VFS_Rename] = (client_vfs_dispatch_t)VFSRename,
    [Client_VFS_Dir_Create] = (client_vfs_dispatch_t)VFSDirCreate,
    [Client_VFS_Dir_Remove] = (client_vfs_dispatch_t)VFSDirRemove,
    [Client_VFS_Dir_Open] = (client_vfs_dispatch_t)VFSDirOpen,
};

KResult InitializeVFS(){
    VFSProcess = ShareProcessKey(Sys_GetProcess());
    char* test0;
    partition_t* test1;
    ClientVFSContext test2{
        .Partition = test1,
        .Path = "hereitis/",
    };
    GetVFSAccessData(&test0, &test1, &test2, "folder0/folder1");
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
        char VolumeBuffer[33];
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
        .PID = Context->PID,
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

        free(AccessTypeBuffer);
        free(VolumeBuffer);

        if(*AccessTypeBuffer == 's'){
            // TODO
        }else if(*AccessTypeBuffer == 'd'){
            if(strncmp(*RelativePath, Context->Path, Context->PathLength)){
                PartitionContext = Context->Partition;
            }else{
                authorization_t AuthorizationNeed = (Volume == Context->StaticVolumeMountPoint) ? FS_AUTHORIZATION_MEDIUM : FS_AUTHORIZATION_HIGH;
                if(AuthorizationNeed > Context->Authorization){
                    if(Volume > PartitionsList->length) return KNOTALLOW;

                    if(VFSAskForAuthorization(Context, AuthorizationNeed) != KSUCCESS){
                        free(Sb);
                        free(*RelativePath);
                        free(VolumeBuffer);
                        return KNOTALLOW;
                    }
                }
                PartitionContext = (partition_t*)vector_get(PartitionsList, Volume);
            }

        }else{
            free(Sb);
            free(*RelativePath);
            free(VolumeBuffer);
            return KFAIL;
        }

        free(VolumeBuffer);
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

    Sys_Execthread(Callback, &arguments, ExecutionTypeQueu, NULL);
    Sys_Close(KSUCCESS);
}

KResult VFSLoginApp(thread_t Callback, uint64_t CallbackArg){
    // TODO read autorization into file
    KResult Status = KFAIL;
    
    arguments_t arguments{
        .arg[0] = Status,            /* Status */
        .arg[1] = CallbackArg,      /* CallbackArg */
        .arg[2] = NULL,             /* GP0 */
        .arg[3] = NULL,             /* GP1 */
        .arg[4] = NULL,             /* GP2 */
        .arg[5] = NULL,             /* GP3 */
    };

    Sys_Execthread(Callback, &arguments, ExecutionTypeQueu, NULL);
    Sys_Close(KSUCCESS);    
}


// TODO : ChangeUserData

KResult VFSClientDispatcher(thread_t Callback, uint64_t CallbackArg, uint64_t Function, uint64_t GP0, uint64_t GP1, uint64_t GP2){
    if(Function >= Client_VFS_Function_Count){
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

        Sys_Execthread(Callback, &arguments, ExecutionTypeQueu, NULL);
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

    Permissions |= (1 << File_Permissions_Write);
    
    arguments_t arguments{
        .arg[0] = Callback,         /* Callback */
        .arg[1] = CallbackArg,      /* CallbackArg */
        .arg[2] = NULL,             /* Path */
        .arg[3] = Permissions,      /* Permissions */
        .arg[4] = NULL,             /* GP2 */
        .arg[5] = NULL,             /* GP3 */
    };

    ShareDataWithArguments_t Data{
        .Data = &RelativePath,
        .Size = (uint64_t)strlen(RelativePath),
        .ParameterPosition = 0x2,
    };

    Sys_Execthread(Partition->FSServerFunctions.Removefile, &arguments, ExecutionTypeQueu, &Data);

    free(RelativePath);
    
    return KSUCCESS; 
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
    
    arguments_t arguments{
        .arg[0] = Callback,         /* Callback */
        .arg[1] = CallbackArg,      /* CallbackArg */
        .arg[2] = NULL,             /* Path */
        .arg[3] = Permissions,      /* Permissions */
        .arg[4] = Target,           /* Target */
        .arg[5] = NULL,             /* GP3 */
    };

    ShareDataWithArguments_t Data{
        .Data = &RelativePath,
        .Size = (uint64_t)strlen(RelativePath),
        .ParameterPosition = 0x2,
    };

    Sys_Execthread(Partition->FSServerFunctions.Openfile, &arguments, ExecutionTypeQueu, &Data);

    free(RelativePath);
    
    return KSUCCESS; 
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
    
    uint64_t RelativeRenameDataSize = sizeof(srv_storage_fs_server_rename_t) + strlen(RelativePathOld) + strlen(RelativePathNew);
    srv_storage_fs_server_rename_t* RelativeRenameData = (srv_storage_fs_server_rename_t*)malloc(RelativeRenameDataSize);

    RelativeRenameData->OldPathPosition = sizeof(srv_storage_fs_server_rename_t);
    RelativeRenameData->NewPathPosition = sizeof(srv_storage_fs_server_rename_t) + strlen(RelativePathOld);

    memcpy((uintptr_t)((uint64_t)RelativeRenameData + RelativeRenameData->OldPathPosition), RelativePathOld, strlen(RelativePathOld));
    memcpy((uintptr_t)((uint64_t)RelativeRenameData + RelativeRenameData->NewPathPosition), RelativePathNew, strlen(RelativePathNew));

    Permissions |= (1 << File_Permissions_Write);
    
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

    Sys_Execthread(PartitionOld->FSServerFunctions.Rename, &arguments, ExecutionTypeQueu, &Data);

    free(RelativePathOld);
    free(RelativePathNew);
    free(RelativeRenameData);
    
    return KSUCCESS; 
}

KResult VFSDirCreate(thread_t Callback, uint64_t CallbackArg, ClientVFSContext* Context, permissions_t Permissions, char* Path){
    partition_t* Partition;
    char* RelativePath;

    KResult Status = GetVFSAccessData(&RelativePath, &Partition, Context, Path);

    if(Status != KSUCCESS){
        return Status; 
    }

    Permissions |= (1 << File_Permissions_Write);
    
    arguments_t arguments{
        .arg[0] = Callback,         /* Callback */
        .arg[1] = CallbackArg,      /* CallbackArg */
        .arg[2] = NULL,             /* Path */
        .arg[3] = Permissions,      /* Permissions */
        .arg[4] = NULL,             /* GP2 */
        .arg[5] = NULL,             /* GP3 */
    };

    ShareDataWithArguments_t Data{
        .Data = &RelativePath,
        .Size = (uint64_t)strlen(RelativePath),
        .ParameterPosition = 0x2,
    };

    Sys_Execthread(Partition->FSServerFunctions.Mkdir, &arguments, ExecutionTypeQueu, &Data);

    free(RelativePath);
    
    return KSUCCESS; 
}

KResult VFSDirRemove(thread_t Callback, uint64_t CallbackArg, ClientVFSContext* Context, permissions_t Permissions, char* Path){
    partition_t* Partition;
    char* RelativePath;

    KResult Status = GetVFSAccessData(&RelativePath, &Partition, Context, Path);

    if(Status != KSUCCESS){
        return Status; 
    }

    Permissions |= (1 << File_Permissions_Write);
    
    arguments_t arguments{
        .arg[0] = Callback,         /* Callback */
        .arg[1] = CallbackArg,      /* CallbackArg */
        .arg[2] = NULL,             /* Path */
        .arg[3] = Permissions,      /* Permissions */
        .arg[4] = NULL,             /* GP2 */
        .arg[5] = NULL,             /* GP3 */
    };

    ShareDataWithArguments_t Data{
        .Data = &RelativePath,
        .Size = (uint64_t)strlen(RelativePath),
        .ParameterPosition = 0x2,
    };

    Sys_Execthread(Partition->FSServerFunctions.Rmdir, &arguments, ExecutionTypeQueu, &Data);

    free(RelativePath);
    
    return KSUCCESS; 
}

KResult VFSDirOpen(thread_t Callback, uint64_t CallbackArg, ClientVFSContext* Context, permissions_t Permissions, char* Path, process_t Target){
    partition_t* Partition;
    char* RelativePath;

    KResult Status = GetVFSAccessData(&RelativePath, &Partition, Context, Path);

    if(Status != KSUCCESS){
        return Status; 
    }

    Permissions |= (1 << File_Permissions_Write);
    
    arguments_t arguments{
        .arg[0] = Callback,         /* Callback */
        .arg[1] = CallbackArg,      /* CallbackArg */
        .arg[2] = NULL,             /* Path */
        .arg[3] = Permissions,      /* Permissions */
        .arg[4] = Target,           /* Target */
        .arg[5] = NULL,             /* GP3 */
    };

    ShareDataWithArguments_t Data{
        .Data = &RelativePath,
        .Size = (uint64_t)strlen(RelativePath),
        .ParameterPosition = 0x2,
    };

    Sys_Execthread(Partition->FSServerFunctions.Rmdir, &arguments, ExecutionTypeQueu, &Data);

    free(RelativePath);
    
    return KSUCCESS; 
}