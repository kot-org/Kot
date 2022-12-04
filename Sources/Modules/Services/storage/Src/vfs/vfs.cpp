#include <vfs/vfs.h>

process_t VFSProcess;

KResult InitializeVFS(){
    VFSProcess = ShareProcessKey(Sys_GetProcess());

    return KSUCCESS;
}

KResult VFSMount(thread_t Callback, uint64_t CallbackArg, bool IsMount, srv_storage_fs_server_functions_t* StorageFSServerFunctions){
    KResult Statu = KFAIL;

    partition_t* Partition = (partition_t*)Sys_GetExternalDataThread();
    if(IsMount){
        memcpy(&Partition->FSServerFunctions, StorageFSServerFunctions, sizeof(srv_storage_fs_server_functions_t));
        std::printf("%x", Partition->Index);
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