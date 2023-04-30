#include <dev/dev.h>

vector_t* DevList;
uint64_t DevListLock;

struct dev_t{
    char* Name;
    size64_t NameLen;
    struct partition_t VirtualPartition;
};

KResult InitializeDev(){
    DevList = vector_create();
    return KSUCCESS;
}

KResult NewDev(thread_t Callback, uint64_t CallbackArg, uintptr_t Opaque){
    struct srv_storage_fs_server_functions_t* FSServerFunctions = (srv_storage_fs_server_functions_t*)Opaque;
    char* Name = (char*)((uint64_t)Opaque + sizeof(struct srv_storage_fs_server_functions_t));

    dev_t* Dev = (dev_t*)malloc(sizeof(dev_t));

    Dev->VirtualPartition.Index = 0;
    Dev->VirtualPartition.IsMount = true;
    Dev->VirtualPartition.Start = 0;
    Dev->VirtualPartition.Size = 0;
    Dev->VirtualPartition.PartitionTypeGUID = {.Data0 = 0, .Data1 = 0, .Data2 = 0, .Data3 = 0, .Data4 = 0};
    Dev->VirtualPartition.Device = 0;
    Dev->VirtualPartition.SpaceList = 0;
    Dev->VirtualPartition.StaticVolumeMountPoint = 0;
    Dev->VirtualPartition.DynamicVolumeMountPoint = 0;

    Dev->NameLen = strlen(Name);
    Dev->Name = (char*)malloc(Dev->NameLen * sizeof(char));
    strcpy(Dev->Name, Name);

    memcpy(&Dev->VirtualPartition.FSServerFunctions, FSServerFunctions, sizeof(srv_storage_fs_server_functions_t));

    atomicAcquire(&DevListLock, 0);
    vector_push(DevList, Dev);
    atomicUnlock(&DevListLock, 0);
    
    arguments_t arguments{
        .arg[0] = KSUCCESS,         /* Status */
        .arg[1] = CallbackArg,      /* CallbackArg */
        .arg[2] = NULL,             /* GP0 */
        .arg[3] = NULL,             /* GP1 */
        .arg[4] = NULL,             /* GP2 */
        .arg[5] = NULL,             /* GP3 */
    };

    Sys_ExecThread(Callback, &arguments, ExecutionTypeQueu, NULL);
    Sys_Close(KSUCCESS);
}

KResult GetDevAccessData(char** RelativePath, partition_t** Partition, ClientVFSContext* Context, char* Path){
    char* DevNameTarget = Path + DEV_PATH_LEN;
    atomicAcquire(&DevListLock, 0);
    for(uint64_t i = 0; i < DevList->length; i++){
        dev_t* Dev = (dev_t*)vector_get(DevList, i);
        if(strncmp(DevNameTarget, Dev->Name, Dev->NameLen)){
            *Partition = &Dev->VirtualPartition;
            *RelativePath = Dev->Name;
            return KSUCCESS;
        }
    }
    atomicUnlock(&DevListLock, 0);
    return KFAIL;   
}