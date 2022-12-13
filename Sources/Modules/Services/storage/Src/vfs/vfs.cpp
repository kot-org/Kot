#include <vfs/vfs.h>

process_t VFSProcess;

static client_vfs_dispatch_t VFSClientDispatcher[Client_VFS_Function_Count] = {
    // [Client_VFS_File_Remove] = VFSFileRemove,
    // [Client_VFS_File_OPEN] = VFSFileOpen,
    // [Client_VFS_Rename] = VFSRename,
    // [Client_VFS_Dir_Create] = VFSDirCreate,
    // [Client_VFS_Dir_Remove] = VFSDirRemove,
    // [Client_VFS_Dir_Open] = VFSDirOpen,
};

KResult InitializeVFS(){
    VFSProcess = ShareProcessKey(Sys_GetProcess());

    return KSUCCESS;
}

KResult VFSMount(thread_t Callback, uint64_t CallbackArg, bool IsMount, srv_storage_fs_server_functions_t* StorageFSServerFunctions){
    KResult Statu = KFAIL;

    partition_t* Partition = (partition_t*)Sys_GetExternalDataThread();
    if(IsMount){
        memcpy(&Partition->FSServerFunctions, StorageFSServerFunctions, sizeof(srv_storage_fs_server_functions_t));
        Statu = MountPartition(Partition->Index);
    }else{
        Statu = UnmountPartition(Partition->Index);
    }
    
    
    arguments_t arguments{
        .arg[0] = Statu,            /* Status */
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
    KResult Statu = KFAIL;
    
    arguments_t arguments{
        .arg[0] = Statu,            /* Status */
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

KResult VFSClientDispatecher(thread_t Callback, uint64_t CallbackArg, uint64_t Function, uint64_t GP0, uint64_t GP1, uint64_t GP2){
    if(Function >= Client_VFS_Function_Count){
        Sys_Close(KSUCCESS); 
    }
    
    ClientVFSContext* Context = (ClientVFSContext*)Sys_GetExternalDataThread();

    VFSClientDispatcher[Function](Callback, CallbackArg, Context, Context->Permissions, GP0, GP1, GP2);
    Sys_Close(KSUCCESS);
}

KResult VFSFileRemove(thread_t Callback, uint64_t CallbackArg, ClientVFSContext* Context, permissions_t Permissions, char* Path){
    KResult Statu = KFAIL;

    std::StringBuilder* Sb = new std::StringBuilder(Path);
    int64_t RelativePathStart = Sb->indexOf(":");

    char* RelativePath;

    partition_t* Partition;

    if(RelativePathStart){
        char* Volume = Sb->substr(0, RelativePathStart);
        RelativePath = Sb->substr(RelativePathStart + 1, Sb->length());
        free(Volume);
    }else{
        RelativePath = Path;
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

    free(Sb);
    free(RelativePath);
    
    return KSUCCESS; 
}