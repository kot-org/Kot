#include <dev/dev.h>

kot_vector_t* DevList;
uint64_t DevListLock;

struct dev_t{
    char* Name;
    size64_t NameLen;
    struct partition_t VirtualPartition;
};

KResult InitializeDev(){
    DevList = kot_vector_create();
    return KSUCCESS;
}

KResult NewDev(kot_thread_t Callback, uint64_t CallbackArg, void* Opaque){
    struct kot_srv_storage_fs_server_functions_t* FSServerFunctions = (kot_srv_storage_fs_server_functions_t*)Opaque;
    char* Name = (char*)((uint64_t)Opaque + sizeof(struct kot_srv_storage_fs_server_functions_t));

    dev_t* Dev = (dev_t*)malloc(sizeof(dev_t));

    Dev->VirtualPartition.Index = 0;
    Dev->VirtualPartition.IsMount = true;
    Dev->VirtualPartition.Start = 0;
    Dev->VirtualPartition.Size = 0;
    memset(&Dev->VirtualPartition.PartitionTypeGUID, 0, sizeof(kot_GUID_t));
    Dev->VirtualPartition.Device = 0;
    Dev->VirtualPartition.SpaceList = 0;
    Dev->VirtualPartition.StaticVolumeMountPoint = 0;
    Dev->VirtualPartition.DynamicVolumeMountPoint = 0;

    Dev->NameLen = strlen(Name);
    Dev->Name = (char*)malloc(Dev->NameLen * sizeof(char));
    strncpy(Dev->Name, Name, Dev->NameLen);

    memcpy(&Dev->VirtualPartition.FSServerFunctions, FSServerFunctions, sizeof(kot_srv_storage_fs_server_functions_t));

    atomicAcquire(&DevListLock, 0);
    kot_vector_push(DevList, (void*)Dev);
    atomicUnlock(&DevListLock, 0);
    
    kot_arguments_t arguments{
        .arg[0] = KSUCCESS,         /* Status */
        .arg[1] = CallbackArg,      /* CallbackArg */
        .arg[2] = NULL,             /* GP0 */
        .arg[3] = NULL,             /* GP1 */
        .arg[4] = NULL,             /* GP2 */
        .arg[5] = NULL,             /* GP3 */
    };

    kot_Sys_ExecThread(Callback, &arguments, ExecutionTypeQueu, NULL);
    kot_Sys_Close(KSUCCESS);
}

KResult GetDevAccessData(char** RelativePath, partition_t** Partition, ClientVFSContext* Context, char* Path){
    char* DevNameTarget = Path + DEV_PATH_LEN;
    atomicAcquire(&DevListLock, 0);
    for(uint64_t i = 0; i < DevList->length; i++){
        dev_t* Dev = (dev_t*)kot_vector_get(DevList, i);
        if(!strncmp(DevNameTarget, Dev->Name, Dev->NameLen)){
            *Partition = &Dev->VirtualPartition;
            *RelativePath = (char*)malloc(Dev->NameLen + 1);
            memcpy(*RelativePath, Dev->Name, Dev->NameLen);
            (*RelativePath)[Dev->NameLen] = NULL;
            atomicUnlock(&DevListLock, 0);
            return KSUCCESS;
        }
    }
    atomicUnlock(&DevListLock, 0);
    return KFAIL;   
}