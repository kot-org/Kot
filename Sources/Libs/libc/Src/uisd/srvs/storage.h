#ifndef _SRV_STORAGE_H
#define _SRV_STORAGE_H 1

#include <kot/sys.h>
#include <kot/uisd.h>
#include <kot/types.h>
#include <kot/authorization.h>

#if defined(__cplusplus)
extern "C" {
#endif

#define Serial_Number_Size              0x14
#define Drive_Model_Number_Size         0x28

#define Storage_Permissions_Admin          (1 << 0)
#define Storage_Permissions_Read           (1 << 1)
#define Storage_Permissions_Write          (1 << 2)
#define Storage_Permissions_Create         (1 << 3)


#define Client_VFS_Function_Count       0x6

#define Client_VFS_File_Remove          0x0
#define Client_VFS_File_Open            0x1
#define Client_VFS_Rename               0x2
#define Client_VFS_Dir_Create           0x3
#define Client_VFS_Dir_Remove           0x4
#define Client_VFS_Dir_Open             0x5

#define File_Function_Count             0x4

#define File_Function_Close             0x0
#define File_Function_GetSize           0x1
#define File_Function_Read              0x2
#define File_Function_Write             0x3


#define Dir_Function_Count              0x3

#define Dir_Function_Close              0x0
#define Dir_Function_GetCount           0x1
#define Dir_Function_Read               0x2

#define SEEK_SET                        0x0
#define SEEK_CUR                        0x1
#define SEEK_END                        0x2


typedef KResult (*StorageCallbackHandler)(KResult Status, struct srv_storage_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3);

struct srv_storage_space_info_t{
    thread_t CreateProtectedDeviceSpaceThread;
    thread_t RequestToDeviceThread;
    ksmem_t BufferRWKey;
    uint64_t BufferRWAlignement;
    uint64_t BufferRWUsableSize;
    process_t DriverProc;
    uint64_t SpaceSize;
};

struct srv_storage_device_info_t{
    struct srv_storage_space_info_t MainSpace;
    uint64_t DeviceSize;
    uint8_t SerialNumber[Serial_Number_Size];
    uint8_t DriveModelNumber[Drive_Model_Number_Size];
};

struct srv_storage_fs_server_functions_t{
    thread_t ChangeUserData;

    thread_t Removefile;

    thread_t Openfile;

    thread_t Rename;

    thread_t Mkdir;
    thread_t Rmdir;

    thread_t Opendir;
};

struct srv_storage_fs_server_open_file_data_t{
    thread_t Dispatcher;
    process_t FSDriverProc;
};

struct srv_storage_fs_server_open_dir_data_t{
    thread_t Dispatcher;
    process_t FSDriverProc;    
};

struct srv_storage_fs_server_rename_t{
    uint64_t OldPathPosition;
    uint64_t NewPathPosition;
};

struct srv_storage_callback_t{
    thread_t Self;
    uint64_t Data;
    size64_t Size;
    bool IsAwait;
    KResult Status;
    StorageCallbackHandler Handler;
};


struct srv_storage_fs_server_read_dir_t{
    uint64_t OldPathPosition;
    uint64_t NewPathPosition;
};

typedef struct {
    uint64_t Lock;

    process_t FileProcessHandler;
    thread_t FileThreadHandler;

    bool IsBinary;

    uint64_t Position;

    bool IsDataEnd;
} file_t;

typedef struct {
    uint64_t Lock;
    
    process_t DirProcessHandler;
    thread_t DirThreadHandler;

    uint64_t Position;
} directory_t;

typedef struct {
    uint64_t NextEntryPosition;
    bool IsFile;
    char Name[];
} directory_entry_t;

typedef struct {
    uint64_t EntryCount;
    directory_entry_t FirstEntry;
} directory_entries_t;

typedef uint64_t mode_t;
typedef uint64_t permissions_t;

void Srv_Storage_Initialize();

void Srv_Storage_Callback(KResult Status, struct srv_storage_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3);

struct srv_storage_callback_t* Srv_Storage_AddDevice(struct srv_storage_device_info_t* Info, bool IsAwait);
struct srv_storage_callback_t* Srv_Storage_RemoveDevice(uint64_t Index, bool IsAwait);
struct srv_storage_callback_t* Srv_Storage_NotifyOnNewPartitionByGUIDType(GUID_t* PartitionTypeGUID, thread_t ThreadToNotify, process_t ProcessToNotify, bool IsAwait);
struct srv_storage_callback_t* Srv_Storage_MountPartition(thread_t VFSMountThread, struct srv_storage_fs_server_functions_t* FSServerFunctions, bool IsAwait);
struct srv_storage_callback_t* Srv_Storage_UnmountPartition(thread_t VFSMountThread, bool IsAwait);

struct srv_storage_callback_t* Srv_Storage_VFSLoginApp(process_t Process, authorization_t Authorization, permissions_t Permissions, char* Path, bool IsAwait);

struct srv_storage_callback_t* Srv_Storage_Removefile(char* Path, bool IsAwait);
struct srv_storage_callback_t* Srv_Storage_Openfile(char* Path, permissions_t Permissions, process_t Target, bool IsAwait);
struct srv_storage_callback_t* Srv_Storage_Rename(char* OldPath, char* NewPath, bool IsAwait);

struct srv_storage_callback_t* Srv_Storage_DirCreate(char* Path, mode_t Mode, bool IsAwait);
struct srv_storage_callback_t* Srv_Storage_DirRemove(char* Path, bool IsAwait);
struct srv_storage_callback_t* Srv_Storage_DirOpen(char* Path, process_t Target, bool IsAwait);

struct srv_storage_callback_t* Srv_Storage_Closefile(file_t* File, bool IsAwait);
struct srv_storage_callback_t* Srv_Storage_Getfilesize(file_t* File, bool IsAwait);
struct srv_storage_callback_t* Srv_Storage_Readfile(file_t* File, uintptr_t Buffer, uint64_t Start, size64_t Size, bool IsAwait);
struct srv_storage_callback_t* Srv_Storage_Writefile(file_t* File, uintptr_t Buffer, uint64_t Start, size64_t Size, bool IsDataEnd, bool IsAwait);

struct srv_storage_callback_t* Srv_Storage_Closedir(directory_t* Dir, bool IsAwait);
struct srv_storage_callback_t* Srv_Storage_Getdircount(directory_t* Dir, bool IsAwait);
struct srv_storage_callback_t* Srv_Storage_Readdir(directory_t* Dir, uint64_t IndexStart, size64_t IndexNumber, bool IsAwait);

file_t* fopen(char* Path, char* Mode);
KResult fclose(file_t* File);
KResult fread(uintptr_t Buffer, size_t BlockSize, size_t BlockCount, file_t* File);
KResult fwrite(uintptr_t Buffer, size_t BlockSize, size_t BlockCount, file_t* File);
KResult fputs(char* String, file_t* File);
KResult fseek(file_t* File, uint64_t Offset, int Whence);
uint64_t ftell(file_t* File);

directory_t* opendir(char* Path);
KResult rewinddir(directory_t* Dir);
directory_entries_t* mreaddir(directory_t* Dir, uint64_t Start, uint64_t Count);
directory_entry_t* readdir(directory_t* Dir);
KResult closedir(directory_t* Dir);
KResult removefile(char* Path);
KResult rename(char* OldName, char* NewName);
KResult mkdir(char* Path, mode_t Mode);
KResult rmdir(char* Path);

#if defined(__cplusplus)
}
#endif

#endif