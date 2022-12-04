#pragma once

#include <core/core.h>

#define File_Function_Count 0x3

#define File_Function_Close 0x0
#define File_Function_Read  0x1
#define File_Function_Write 0x2


#define Dir_Function_Count 0x2

#define Dir_Function_Close 0x0
#define Dir_Function_Read  0x1

typedef KResult (*file_dispatch_t)(thread_t Callback, uint64_t CallbackArg, struct file_t* file, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint32_t GP3);
typedef KResult (*dir_dispatch_t)(thread_t Callback, uint64_t CallbackArg, struct directory_t* directory, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint32_t GP3);


KResult MountToVFS(struct mount_info_t* MountInfo, process_t VFSProcess, thread_t VFSMountThread);

KResult ChangeUserData(thread_t Callback, uint64_t CallbackArg, uint64_t UID, uint64_t GID, char* UserName);

KResult Removefile(thread_t Callback, uint64_t CallbackArg, char* Path, uint64_t Permissions);

KResult Openfile(thread_t Callback, uint64_t CallbackArg, char* Path, uint64_t Permissions, process_t Target);

KResult FileDispatch(thread_t Callback, uint64_t CallbackArg, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3);
KResult Closefile(thread_t Callback, uint64_t CallbackArg, file_t* File, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint32_t GP3);
KResult Readfile(thread_t Callback, uint64_t CallbackArg, file_t* File, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint32_t GP3);
KResult Writefile(thread_t Callback, uint64_t CallbackArg, file_t* File, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint32_t GP3);

KResult Rename(thread_t Callback, uint64_t CallbackArg, char* OldPath, char* NewPath, uint64_t Permissions);

KResult Mkdir(thread_t Callback, uint64_t CallbackArg, char* Path, char* Name, uint64_t Permissions);

KResult Rmdir(thread_t Callback, uint64_t CallbackArg, char* Path, uint64_t Permissions);

KResult Opendir(thread_t Callback, uint64_t CallbackArg, char* Path, uint64_t Permissions, process_t Target);

KResult DirDispatch(thread_t Callback, uint64_t CallbackArg, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3);
KResult Readdir(thread_t Callback, uint64_t CallbackArg, directory_t* Directory, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint32_t GP3);
KResult Closedir(thread_t Callback, uint64_t CallbackArg, directory_t* Directory, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint32_t GP3);