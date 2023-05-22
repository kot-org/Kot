#pragma once

#include <core/core.h>
#include <stdio.h>

typedef KResult (*file_dispatch_t)(kot_thread_t Callback, uint64_t CallbackArg, struct ext_file_t* File, uint64_t GP0, uint64_t GP1, uint64_t GP2);
typedef KResult (*dir_dispatch_t)(kot_thread_t Callback, uint64_t CallbackArg, struct ext_directory_t* Directory, uint64_t GP0, uint64_t GP1, uint64_t GP2);


KResult MountToVFS(struct mount_info_t* MountInfo, kot_process_t VFSProcess, kot_thread_t VFSMountThread);

KResult ChangeUserData(kot_thread_t Callback, uint64_t CallbackArg, uint64_t UID, uint64_t GID, char* UserName);

KResult Removefile(kot_thread_t Callback, uint64_t CallbackArg, char* Path, kot_permissions_t Permissions);

KResult Openfile(kot_thread_t Callback, uint64_t CallbackArg, char* Path, kot_permissions_t Permissions, kot_process_t Target);

KResult FileDispatch(kot_thread_t Callback, uint64_t CallbackArg, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3);
KResult Closefile(kot_thread_t Callback, uint64_t CallbackArg, ext_file_t* File, uint64_t GP0, uint64_t GP1, uint64_t GP2);
KResult Getfilesize(kot_thread_t Callback, uint64_t CallbackArg, ext_file_t* File, uint64_t GP0, uint64_t GP1, uint64_t GP2);
KResult Readfile(kot_thread_t Callback, uint64_t CallbackArg, ext_file_t* File, uint64_t GP0, uint64_t GP1, uint64_t GP2);
KResult Writefile(kot_thread_t Callback, uint64_t CallbackArg, ext_file_t* File, uint64_t GP0, uint64_t GP1, uint64_t GP2);

KResult Rename(kot_thread_t Callback, uint64_t CallbackArg, struct srv_storage_fs_server_rename_t* RenameData, kot_permissions_t Permissions);

KResult Mkdir(kot_thread_t Callback, uint64_t CallbackArg, char* Path, mode_t Mode, kot_permissions_t Permissions);

KResult Rmdir(kot_thread_t Callback, uint64_t CallbackArg, char* Path, kot_permissions_t Permissions);

KResult Opendir(kot_thread_t Callback, uint64_t CallbackArg, char* Path, kot_permissions_t Permissions, kot_process_t Target);

KResult DirDispatch(kot_thread_t Callback, uint64_t CallbackArg, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3);
KResult Readdir(kot_thread_t Callback, uint64_t CallbackArg, ext_directory_t* Directory, uint64_t GP0, uint64_t GP1, uint64_t GP2);
KResult Getdircount(kot_thread_t Callback, uint64_t CallbackArg, ext_directory_t* Directory, uint64_t GP0, uint64_t GP1, uint64_t GP2);
KResult Closedir(kot_thread_t Callback, uint64_t CallbackArg, ext_directory_t* Directory, uint64_t GP0, uint64_t GP1, uint64_t GP2);