#include <vfs/vfs.h>

process_t VFSProcess;

static client_vfs_dispatch_t VFSClientDispatcherFunctions[Client_VFS_Function_Count] = {
    [Client_VFS_File_Remove] = (client_vfs_dispatch_t)VFSFileRemove,
    // [Client_VFS_File_OPEN] = (client_vfs_dispatch_t)VFSFileOpen,
    // [Client_VFS_Rename] = (client_vfs_dispatch_t)VFSRename,
    // [Client_VFS_Dir_Create] = (client_vfs_dispatch_t)VFSDirCreate,
    // [Client_VFS_Dir_Remove] = (client_vfs_dispatch_t)VFSDirRemove,
    // [Client_VFS_Dir_Open] = (client_vfs_dispatch_t)VFSDirOpen,
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


KResult GetVFSAccessData(char** RelativePath, partition_t** Partition, ClientVFSContext* Context, char* Path){
    std::StringBuilder* Sb = new std::StringBuilder(Path);
    int64_t RelativePathStart = Sb->indexOf(":");

    partition_t* PartitionContext;

    if(RelativePathStart != -1){
        char* Volume = Sb->substr(0, RelativePathStart);
        *RelativePath = Sb->substr(RelativePathStart + 1, Sb->length());
        free(Volume);
    }else{
        PartitionContext = Context->Partition;
        Sb->append(Context->Path, 0);
        std::printf(Sb->toString());
    }

    *RelativePath = Sb->toString();
    *Partition = PartitionContext;
    free(Sb);
    return KSUCCESS;
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
            .arg[0] = Status,            /* Status */
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
    
    thread_t FSServer = Partition->FSServerFunctions.Removefile;

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