#ifndef KOT_SRV_STORAGE_H
#define KOT_SRV_STORAGE_H 1

#include <kot/sys.h>
#include <kot/uisd.h>
#include <kot/types.h>
#include <kot/atomic.h>
#include <kot/memory.h>
#include <abi-bits/mode_t.h>

#if defined(__cplusplus)
extern "C" {
#endif

#define Serial_Number_Size              0x14
#define Drive_Model_Number_Size         0x28

#define Storage_Permissions_Admin          (1 << 0)
#define Storage_Permissions_Read           (1 << 1)
#define Storage_Permissions_Write          (1 << 2)
#define Storage_Permissions_Create         (1 << 3)


#define Client_VFS_Function_Count       0x8

#define Client_VFS_File_Remove          0x0
#define Client_VFS_File_Open            0x1
#define Client_VFS_Rename               0x2
#define Client_VFS_Dir_Create           0x3
#define Client_VFS_Dir_Remove           0x4
#define Client_VFS_Dir_Open             0x5
#define Client_VFS_Get_CWD              0x6
#define Client_VFS_Set_CWD              0x7

#define File_Function_Count             0x6

#define File_Function_Close             0x0
#define File_Function_GetSize           0x1
#define File_Function_Read              0x2
#define File_Function_Write             0x3
#define File_Function_Ioctl             0x4
#define File_Function_Stat              0x5

// External data file
#define File_Is_Binary                  (1 << 0)


#define Dir_Function_Count              0x3

#define Dir_Function_Close              0x0
#define Dir_Function_GetCount           0x1
#define Dir_Function_Read               0x2


typedef KResult (*kot_StorageCallbackHandler)(KResult Status, struct kot_srv_storage_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3);

struct kot_srv_storage_space_info_t{
    kot_thread_t CreateProtectedDeviceSpaceThread;
    kot_thread_t RequestToDeviceThread;
    kot_key_mem_t BufferRWKey;
    uint64_t BufferRWAlignement;
    uint64_t BufferRWUsableSize;
    kot_process_t DriverProc;
    uint64_t SpaceSize;
};

struct kot_srv_storage_device_info_t{
    struct kot_srv_storage_space_info_t MainSpace;
    uint64_t DeviceSize;
    uint8_t SerialNumber[Serial_Number_Size];
    uint8_t DriveModelNumber[Drive_Model_Number_Size];
};

struct kot_srv_storage_fs_server_functions_t{
    kot_thread_t ChangeUserData;

    kot_thread_t Removefile;

    kot_thread_t Openfile;

    kot_thread_t Rename;

    kot_thread_t Mkdir;
    kot_thread_t Rmdir;

    kot_thread_t Opendir;
};

struct kot_srv_storage_fs_server_open_file_data_t{
    kot_thread_t Dispatcher;
    kot_process_t FSDriverProc;
};

struct kot_srv_storage_fs_server_open_dir_data_t{
    kot_thread_t Dispatcher;
    kot_process_t FSDriverProc;    
};

struct kot_srv_storage_fs_server_rename_t{
    uint64_t OldPathPosition;
    uint64_t NewPathPosition;
};

struct kot_srv_storage_callback_t{
    kot_thread_t Self;
    uintptr_t Data;
    size64_t Size;
    bool IsAwait;
    KResult Status;
    kot_StorageCallbackHandler Handler;
};


struct kot_srv_storage_fs_server_read_dir_t{
    uint64_t OldPathPosition;
    uint64_t NewPathPosition;
};

typedef struct {
    uint64_t Lock;

    kot_process_t FileProcessHandler;
    kot_thread_t FileThreadHandler;

    uint64_t ExternalData;

    uint64_t Position;

    bool IsDataEnd;
} kot_file_t;

typedef struct {
    uint64_t Lock;
    
    kot_process_t DirProcessHandler;
    kot_thread_t DirThreadHandler;

    uint64_t Position;
} kot_directory_t;

typedef struct {
    uint64_t NextEntryPosition;
    bool IsFile;
    char Name[];
} kot_directory_entry_t;

typedef struct {
    uint64_t EntryCount;
    kot_directory_entry_t FirstEntry;
} kot_directory_entries_t;

typedef uint64_t kot_permissions_t;

extern kot_process_t kot_ShareProcessFS;

void kot_Srv_Storage_Initialize();

void kot_Srv_Storage_Callback(KResult Status, struct kot_srv_storage_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3);

struct kot_srv_storage_callback_t* kot_Srv_Storage_AddDevice(struct kot_srv_storage_device_info_t* Info, bool IsAwait);
struct kot_srv_storage_callback_t* kot_Srv_Storage_RemoveDevice(uint64_t Index, bool IsAwait);
struct kot_srv_storage_callback_t* kot_Srv_Storage_NotifyOnNewPartitionByGUIDType(struct kot_GUID_t* PartitionTypeGUID, kot_thread_t ThreadToNotify, kot_process_t ProcessToNotify, bool IsAwait);
struct kot_srv_storage_callback_t* kot_Srv_Storage_MountPartition(kot_thread_t VFSMountThread, struct kot_srv_storage_fs_server_functions_t* FSServerFunctions, bool IsAwait);
struct kot_srv_storage_callback_t* kot_Srv_Storage_UnmountPartition(kot_thread_t VFSMountThread, bool IsAwait);

struct kot_srv_storage_callback_t* kot_Srv_Storage_VFSLoginApp(kot_process_t Process, kot_authorization_t Authorization, kot_permissions_t Permissions, char* Path, bool IsAwait);

struct kot_srv_storage_callback_t* kot_Srv_Storage_Removefile(char* Path, bool IsAwait);
struct kot_srv_storage_callback_t* kot_Srv_Storage_Openfile(char* Path, kot_permissions_t Permissions, kot_process_t Target, bool IsAwait);
struct kot_srv_storage_callback_t* kot_Srv_Storage_Rename(char* OldPath, char* NewPath, bool IsAwait);

struct kot_srv_storage_callback_t* kot_Srv_Storage_DirCreate(char* Path, mode_t Mode, bool IsAwait);
struct kot_srv_storage_callback_t* kot_Srv_Storage_DirRemove(char* Path, bool IsAwait);
struct kot_srv_storage_callback_t* kot_Srv_Storage_DirOpen(char* Path, kot_process_t Target, bool IsAwait);

struct kot_srv_storage_callback_t* kot_Srv_Storage_GetCWD(bool IsAwait);
struct kot_srv_storage_callback_t* kot_Srv_Storage_SetCWD(char* Path, bool IsAwait);

struct kot_srv_storage_callback_t* kot_Srv_Storage_Closefile(kot_file_t* File, bool IsAwait);
struct kot_srv_storage_callback_t* kot_Srv_Storage_Getfilesize(kot_file_t* File, bool IsAwait);
struct kot_srv_storage_callback_t* kot_Srv_Storage_Readfile(kot_file_t* File, void* Buffer, uint64_t Start, size64_t Size, bool IsAwait);
struct kot_srv_storage_callback_t* kot_Srv_Storage_Writefile(kot_file_t* File, void* Buffer, uint64_t Start, size64_t Size, bool IsDataEnd, bool IsAwait);
struct kot_srv_storage_callback_t* kot_Srv_Storage_Ioctl(kot_file_t* File, unsigned long Request, void* Arg, bool IsAwait);

struct kot_srv_storage_callback_t* kot_Srv_Storage_Closedir(kot_directory_t* Dir, bool IsAwait);
struct kot_srv_storage_callback_t* kot_Srv_Storage_Getdircount(kot_directory_t* Dir, bool IsAwait);
struct kot_srv_storage_callback_t* kot_Srv_Storage_Readdir(kot_directory_t* Dir, uint64_t IndexStart, size64_t IndexNumber, bool IsAwait);

struct kot_srv_storage_callback_t* kot_Srv_Storage_NewDev(char* Name, struct kot_srv_storage_fs_server_functions_t* FSServerFunctions, bool IsAwait);

kot_process_t kot_ShareProcessKeyFS(kot_process_t Process);
kot_file_t* kot_fopen(char* Path, char* Mode);
kot_file_t* kot_fopenmf(char* Path, int Flags, mode_t Mode);
KResult kot_fclose(kot_file_t* File);
KResult kot_fread(void* Buffer, size_t BlockSize, size_t BlockCount, kot_file_t* File);
KResult kot_fwrite(void* Buffer, size_t BlockSize, size_t BlockCount, kot_file_t* File);
KResult kot_fputs(char* String, kot_file_t* File);
KResult kot_fseek(kot_file_t* File, uint64_t Offset, int Whence);
uint64_t kot_ftell(kot_file_t* File);

kot_directory_t* kot_opendir(char* Path);
KResult kot_rewinddir(kot_directory_t* Dir);
KResult kot_filecount(kot_directory_t* Dir, uint64_t* Count);
kot_directory_entries_t* kot_mreaddir(kot_directory_t* Dir, uint64_t Start, uint64_t Count);
kot_directory_entry_t* kot_readdir(kot_directory_t* Dir);
KResult kot_closedir(kot_directory_t* Dir);
KResult kot_removefile(char* Path);
KResult kot_rename(char* OldName, char* NewName);
KResult kot_mkdir(char* Path, mode_t Mode);
KResult kot_rmdir(char* Path);

char* kot_dirname(char* path);

#if defined(__cplusplus)
} 
#endif

#endif