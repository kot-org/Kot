#pragma once

#include <core/main.h>
#include <kot/uisd/srvs/storage.h>

#include <kot++/string.h>
#include <kot++/printf.h>

extern process_t VFSProcess;


struct ClientVFSContext{
    permissions_t Permissions;
    struct partition_t* Partition;
    char* Path;
};

typedef KResult (*client_vfs_dispatch_t)(thread_t Callback, uint64_t CallbackArg, struct ClientVFSContext* Context, permissions_t Permissions, uint64_t GP0, uint64_t GP1, uint64_t GP2);

KResult InitializeVFS();

KResult VFSMount(thread_t Callback, uint64_t CallbackArg, bool IsMount, struct srv_storage_fs_server_functions_t* StorageFSServerFunctions);

KResult GetVFSAccessData(char** RelativePath, partition_t** Partition, ClientVFSContext* Context, char* Path);

KResult VFSFileRemove(thread_t Callback, uint64_t CallbackArg, ClientVFSContext* Context, permissions_t Permissions, char* Path);
