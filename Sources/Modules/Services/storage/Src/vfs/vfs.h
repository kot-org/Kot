#pragma once

#include <core/main.h>

#include <kot/bits.h>
#include <kot/stdio.h>
#include <kot/authorization.h>
#include <kot/uisd/srvs/system.h>
#include <kot/uisd/srvs/storage.h>

#include <kot++/string.h>
#include <kot++/printf.h>

#define VFSValidationFieldsCount 0x2

extern process_t VFSProcess;


struct ClientVFSContext{
    authorization_t Authorization;
    permissions_t Permissions;
    struct partition_t* Partition;
    char* Path;
    size64_t PathLength;
    uint64_t StaticVolumeMountPoint;
    uint64_t DynamicVolumeMountPoint;
};

typedef KResult (*client_vfs_dispatch_t)(thread_t Callback, uint64_t CallbackArg, struct ClientVFSContext* Context, permissions_t Permissions, uint64_t GP0, uint64_t GP1, uint64_t GP2);

KResult InitializeVFS();

KResult VFSMount(thread_t Callback, uint64_t CallbackArg, bool IsMount, struct srv_storage_fs_server_functions_t* StorageFSServerFunctions);

KResult VFSLoginApp(thread_t Callback, uint64_t CallbackArg, process_t Process, authorization_t Authorization, permissions_t Permissions, char* Path);

KResult VFSClientDispatcher(thread_t Callback, uint64_t CallbackArg, uint64_t Function, uint64_t GP0, uint64_t GP1, uint64_t GP2);

KResult GetVFSAccessData(char** RelativePath, partition_t** Partition, ClientVFSContext* Context, char* Path);

KResult VFSFileRemove(thread_t Callback, uint64_t CallbackArg, ClientVFSContext* Context, permissions_t Permissions, char* Path);
KResult VFSFileOpen(thread_t Callback, uint64_t CallbackArg, ClientVFSContext* Context, permissions_t PermissionsContext, permissions_t Permissions, char* Path, process_t Target);
KResult VFSRename(thread_t Callback, uint64_t CallbackArg, ClientVFSContext* Context, permissions_t Permissions, srv_storage_fs_server_rename_t* RenameData);
KResult VFSDirCreate(thread_t Callback, uint64_t CallbackArg, ClientVFSContext* Context, permissions_t Permissions, char* Path);
KResult VFSDirRemove(thread_t Callback, uint64_t CallbackArg, ClientVFSContext* Context, permissions_t Permissions, char* Path);
KResult VFSDirOpen(thread_t Callback, uint64_t CallbackArg, ClientVFSContext* Context, permissions_t Permissions, char* Path, process_t Target);


typedef KResult (*file_dispatch_t)(thread_t Callback, uint64_t CallbackArg, char* FilePath, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint32_t GP3);

KResult VFSfileReadInitrd(thread_t Callback, uint64_t CallbackArg, char* FilePath, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint32_t GP3);
KResult VFSfileCloseInitrd(thread_t Callback, uint64_t CallbackArg, char* FilePath, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint32_t GP3);