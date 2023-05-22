#pragma once

#include <core/main.h>

#include <stdio.h>
#include <kot/bits.h>
#include <kot/authorization.h>
#include <kot/uisd/srvs/system.h>
#include <kot/uisd/srvs/storage.h>

#include <kot++/string.h>
#include <kot++/printf.h>

#define VFSValidationFieldsCount 0x2

extern kot_process_t VFSProcess;


struct ClientVFSContext{
    kot_authorization_t Authorization;
    kot_permissions_t Permissions;
    struct partition_t* Partition;
    char* Path;
    size64_t PathLength;
    uint64_t StaticVolumeMountPoint;
    uint64_t DynamicVolumeMountPoint;
};

typedef KResult (*client_vfs_dispatch_t)(kot_thread_t Callback, uint64_t CallbackArg, struct ClientVFSContext* Context, kot_permissions_t Permissions, uint64_t GP0, uint64_t GP1, uint64_t GP2);

KResult InitializeVFS();

KResult VFSMount(kot_thread_t Callback, uint64_t CallbackArg, bool IsMount, struct kot_srv_storage_fs_server_functions_t* StorageFSServerFunctions);

KResult VFSLoginApp(kot_thread_t Callback, uint64_t CallbackArg, kot_process_t Process, kot_authorization_t Authorization, kot_permissions_t Permissions, char* Path);

KResult VFSClientDispatcher(kot_thread_t Callback, uint64_t CallbackArg, uint64_t Function, uint64_t GP0, uint64_t GP1, uint64_t GP2);

KResult GetVFSAccessData(char** RelativePath, partition_t** Partition, ClientVFSContext* Context, char* Path);

KResult VFSFileRemove(kot_thread_t Callback, uint64_t CallbackArg, ClientVFSContext* Context, kot_permissions_t Permissions, char* Path);
KResult VFSFileOpen(kot_thread_t Callback, uint64_t CallbackArg, ClientVFSContext* Context, kot_permissions_t PermissionsContext, kot_permissions_t Permissions, char* Path, kot_process_t Target);
KResult VFSRename(kot_thread_t Callback, uint64_t CallbackArg, ClientVFSContext* Context, kot_permissions_t Permissions, kot_srv_storage_fs_server_rename_t* RenameData);
KResult VFSDirCreate(kot_thread_t Callback, uint64_t CallbackArg, ClientVFSContext* Context, kot_permissions_t Permissions, char* Path, mode_t Mode);
KResult VFSDirRemove(kot_thread_t Callback, uint64_t CallbackArg, ClientVFSContext* Context, kot_permissions_t Permissions, char* Path);
KResult VFSDirOpen(kot_thread_t Callback, uint64_t CallbackArg, ClientVFSContext* Context, kot_permissions_t Permissions, char* Path, kot_process_t Target);


typedef KResult (*file_dispatch_t)(kot_thread_t Callback, uint64_t CallbackArg,  struct InitrdContext* Context, uint64_t GP0, uint64_t GP1, uint64_t GP2);

KResult VFSfileReadInitrd(kot_thread_t Callback, uint64_t CallbackArg,  struct InitrdContext* Context, uint64_t GP0, uint64_t GP1, uint64_t GP2);
KResult VFSGetfilesizeInitrd(kot_thread_t Callback, uint64_t CallbackArg,  struct InitrdContext* Context, uint64_t GP0, uint64_t GP1, uint64_t GP2);
KResult VFSfileCloseInitrd(kot_thread_t Callback, uint64_t CallbackArg,  struct InitrdContext* Context, uint64_t GP0, uint64_t GP1, uint64_t GP2);
KResult VFSfileOpenInitrd(kot_thread_t Callback, uint64_t CallbackArg, char* Path, kot_permissions_t Permissions, kot_process_t Target);