#include <stdlib.h>
#include <string.h>
#include <bits/ensure.h>
#include <mlibc/debug.hpp>
#include <abi-bits/fcntl.h>
#include <mlibc/allocator.hpp>
#include <mlibc/all-sysdeps.hpp>
#include <kot/uisd/srvs/storage.h>

extern "C" {

kot_process_t kot_ShareProcessFS = NULL;

kot_file_t* kot_fopen(char* Path, char* Mode){
    if(!kot_ShareProcessFS) kot_ShareProcessFS = kot_ShareProcessKey(kot_Sys_GetProcess());
    if(Mode[0] == 'r'){
        if(Mode[1] == '\0'){
            struct kot_srv_storage_callback_t* CallbackFile = kot_Srv_Storage_Openfile(Path, Storage_Permissions_Read, kot_ShareProcessFS, true);
            if(CallbackFile->Status != KSUCCESS){
                free(CallbackFile);
                return NULL;
            }
            kot_file_t* File = (kot_file_t*)CallbackFile->Data;
            File->Position = NULL;
            File->ExternalData = NULL;
            File->Lock = NULL;
            free(CallbackFile);
            return File;
        }else if(Mode[1] == '+'){
            struct kot_srv_storage_callback_t* CallbackFile = kot_Srv_Storage_Openfile(Path, Storage_Permissions_Read | Storage_Permissions_Write, kot_ShareProcessFS, true);
            if(CallbackFile->Status != KSUCCESS){
                free(CallbackFile);
                return NULL;
            }
            kot_file_t* File = (kot_file_t*)CallbackFile->Data;
            File->Position = NULL;
            File->ExternalData = NULL;
            File->Lock = NULL;
            free(CallbackFile);
            return File;
        }else if(Mode[1] == 'b'){
            if(Mode[2] == '\0'){
                struct kot_srv_storage_callback_t* CallbackFile = kot_Srv_Storage_Openfile(Path, Storage_Permissions_Read, kot_ShareProcessFS, true);
                if(CallbackFile->Status != KSUCCESS){
                    free(CallbackFile);
                    return NULL;
                }
                kot_file_t* File = (kot_file_t*)CallbackFile->Data;
                File->ExternalData = File_Is_Binary;
                File->Position = NULL;
                File->Lock = NULL;
                free(CallbackFile);
                return File;
            }else if(Mode[2] == '+'){
                struct kot_srv_storage_callback_t* CallbackFile = kot_Srv_Storage_Openfile(Path, Storage_Permissions_Read | Storage_Permissions_Write, kot_ShareProcessFS, true);
                if(CallbackFile->Status != KSUCCESS){
                    free(CallbackFile);
                    return NULL;
                }
                kot_file_t* File = (kot_file_t*)CallbackFile->Data;
                File->ExternalData = File_Is_Binary;
                File->Position = NULL;
                File->Lock = NULL;
                free(CallbackFile);
                return File;
            }
        }
    }else if(Mode[0] == 'w'){
        if(Mode[1] == '\0'){
            struct kot_srv_storage_callback_t* CallbackFile = kot_Srv_Storage_Openfile(Path, Storage_Permissions_Write | Storage_Permissions_Create, kot_ShareProcessFS, true);
            if(CallbackFile->Status != KSUCCESS){
                free(CallbackFile);
                return NULL;
            }
            kot_file_t* File = (kot_file_t*)CallbackFile->Data;
            File->Position = NULL;
            File->ExternalData = NULL;
            File->Lock = NULL;
            free(CallbackFile);
            return File;
        }else if(Mode[1] == '+'){
            struct kot_srv_storage_callback_t* CallbackFile = kot_Srv_Storage_Openfile(Path, Storage_Permissions_Read | Storage_Permissions_Write | Storage_Permissions_Create, kot_ShareProcessFS, true);
            if(CallbackFile->Status != KSUCCESS){
                free(CallbackFile);
                return NULL;
            }
            kot_file_t* File = (kot_file_t*)CallbackFile->Data;
            File->Position = NULL;
            File->ExternalData = NULL;
            File->Lock = NULL;
            free(CallbackFile);
            return File;
        }else if(Mode[1] == 'b'){
            if(Mode[2] == '\0'){
                struct kot_srv_storage_callback_t* CallbackFile = kot_Srv_Storage_Openfile(Path, Storage_Permissions_Write | Storage_Permissions_Create, kot_ShareProcessFS, true);
                if(CallbackFile->Status != KSUCCESS){
                    free(CallbackFile);
                    return NULL;
                }
                kot_file_t* File = (kot_file_t*)CallbackFile->Data;
                File->ExternalData = File_Is_Binary;
                File->Position = NULL;
                File->Lock = NULL;
                free(CallbackFile);
                return File;
            }else if(Mode[2] == '+'){
                struct kot_srv_storage_callback_t* CallbackFile = kot_Srv_Storage_Openfile(Path, Storage_Permissions_Read | Storage_Permissions_Write | Storage_Permissions_Create, kot_ShareProcessFS, true);
                if(CallbackFile->Status != KSUCCESS){
                    free(CallbackFile);
                    return NULL;
                }
                kot_file_t* File = (kot_file_t*)CallbackFile->Data;
                File->ExternalData = File_Is_Binary;
                File->Position = NULL;
                File->Lock = NULL;
                free(CallbackFile);
                return File;
            }
        }
    }else if(Mode[0] == 'a'){
        if(Mode[1] == '\0'){
            struct kot_srv_storage_callback_t* CallbackFile = kot_Srv_Storage_Openfile(Path, Storage_Permissions_Write | Storage_Permissions_Create, kot_ShareProcessFS, true);
            if(CallbackFile->Status != KSUCCESS){
                free(CallbackFile);
                return NULL;
            }
            kot_file_t* File = (kot_file_t*)CallbackFile->Data;
            struct kot_srv_storage_callback_t* CallbackFileSize = kot_Srv_Storage_Getfilesize(File, true);
            if(CallbackFileSize->Status != KSUCCESS){
                free(CallbackFileSize);
                free(CallbackFile);
                free(File);
                return NULL;
            }
            size64_t Size = CallbackFileSize->Data;
            File->Position = Size;
            File->ExternalData = NULL;
            File->Lock = NULL;
            free(CallbackFileSize);
            free(CallbackFile);
            return File;
        }else if(Mode[1] == '+'){
            struct kot_srv_storage_callback_t* CallbackFile = kot_Srv_Storage_Openfile(Path, Storage_Permissions_Read | Storage_Permissions_Write | Storage_Permissions_Create, kot_ShareProcessFS, true);
            if(CallbackFile->Status != KSUCCESS){
                free(CallbackFile);
                return NULL;
            }
            kot_file_t* File = (kot_file_t*)CallbackFile->Data;
            struct kot_srv_storage_callback_t* CallbackFileSize = kot_Srv_Storage_Getfilesize(File, true);
            if(CallbackFileSize->Status != KSUCCESS){
                free(CallbackFile);
                free(File);
                return NULL;
            }
            size64_t Size = CallbackFileSize->Data;
            File->Position = Size;
            File->ExternalData = NULL;
            File->Lock = NULL;
            free(CallbackFileSize);
            free(CallbackFile);
            return File;
        }else if(Mode[1] == 'b'){
            if(Mode[2] == '\0'){
                struct kot_srv_storage_callback_t* CallbackFile = kot_Srv_Storage_Openfile(Path, Storage_Permissions_Write | Storage_Permissions_Create, kot_ShareProcessFS, true);
                if(CallbackFile->Status != KSUCCESS){
                    free(CallbackFile);
                    return NULL;
                }
                kot_file_t* File = (kot_file_t*)CallbackFile->Data;
                struct kot_srv_storage_callback_t* CallbackFileSize = kot_Srv_Storage_Getfilesize(File, true);
                if(CallbackFileSize->Status != KSUCCESS){
                    free(CallbackFile);
                    free(File);
                    return NULL;
                }
                size64_t Size = CallbackFileSize->Data;
                File->ExternalData = File_Is_Binary;
                File->Position = Size;
                File->Lock = NULL;
                free(CallbackFile);
                return File;
            }else if(Mode[2] == '+'){
                struct kot_srv_storage_callback_t* CallbackFile = kot_Srv_Storage_Openfile(Path, Storage_Permissions_Read | Storage_Permissions_Write | Storage_Permissions_Create, kot_ShareProcessFS, true);
                if(CallbackFile->Status != KSUCCESS){
                    free(CallbackFile);
                    return NULL;
                }
                kot_file_t* File = (kot_file_t*)CallbackFile->Data;
                struct kot_srv_storage_callback_t* CallbackFileSize = kot_Srv_Storage_Getfilesize(File, true);
                if(CallbackFileSize->Status != KSUCCESS){
                    free(CallbackFile);
                    free(File);
                    return NULL;
                }
                size64_t Size = CallbackFileSize->Data;
                File->ExternalData = File_Is_Binary;
                File->Position = Size;
                File->Lock = NULL;
                free(CallbackFile);
                return File;
            }
        }
    }
    return NULL;
}

kot_file_t* kot_fopenmf(char* Path, int Flags, mode_t Mode){
    if(!kot_ShareProcessFS) kot_ShareProcessFS = kot_ShareProcessKey(kot_Sys_GetProcess());

    kot_permissions_t Permissions = NULL;

    // Main flags
    if(Flags & O_RDWR){
        Permissions = Storage_Permissions_Read | Storage_Permissions_Write;
    }else if(Flags & O_RDONLY){
        Permissions = Storage_Permissions_Read;
    }else if(Flags & O_WRONLY){
        Permissions = Storage_Permissions_Write;
    }

    // Other flags
    if(Flags & O_CREAT){
        Permissions |= Storage_Permissions_Create;
    }

    if(Flags & O_TRUNC){
        // TODO
        __ensure(!"O_TRUNC : Not implemented");
    }

    if(Flags & O_CLOEXEC){
        // TODO
        __ensure(!"O_CLOEXEC : Not implemented");
    }
    struct kot_srv_storage_callback_t* CallbackFile = kot_Srv_Storage_Openfile(Path, Permissions, kot_ShareProcessFS, true);
    if(CallbackFile->Status != KSUCCESS){
        free(CallbackFile);
        return NULL;
    }
    kot_file_t* File = (kot_file_t*)CallbackFile->Data;
    if(Flags & O_APPEND){
        struct kot_srv_storage_callback_t* CallbackFileSize = kot_Srv_Storage_Getfilesize(File, true);
        if(CallbackFileSize->Status != KSUCCESS){
            free(CallbackFileSize);
            free(CallbackFile);
            free(File);
            return NULL;
        }
        size64_t Size = CallbackFileSize->Data;
        free(CallbackFileSize);
        File->Position = Size;
    }else{
        File->Position = NULL;
    }
    File->Lock = NULL;
    free(CallbackFile);
    return File;
}

KResult kot_fclose(kot_file_t* File){
    struct kot_srv_storage_callback_t* CallbackCloseFile = kot_Srv_Storage_Closefile(File, true);
    KResult Status = CallbackCloseFile->Status;
    free(CallbackCloseFile);
    free(File);
    return Status;
}

KResult kot_fread(void* Buffer, size_t BlockSize, size_t BlockCount, kot_file_t* File){
    atomicAcquire(&File->Lock, 0);
    struct kot_srv_storage_callback_t* CallbackReadFile = kot_Srv_Storage_Readfile(File, Buffer, File->Position, BlockSize * BlockCount, true);
    KResult Status = CallbackReadFile->Status;
    if(Status == KSUCCESS){
        File->Position += CallbackReadFile->Size;
    }
    atomicUnlock(&File->Lock, 0);
    free(CallbackReadFile);
    return Status;
}

KResult kot_fwrite(void* Buffer, size_t BlockSize, size_t BlockCount, kot_file_t* File){
    atomicAcquire(&File->Lock, 0);
    struct kot_srv_storage_callback_t* CallbackWriteFile = kot_Srv_Storage_Writefile(File, Buffer, File->Position, BlockSize * BlockCount, File->IsDataEnd, true);
    KResult Status = CallbackWriteFile->Status;
    if(Status == KSUCCESS){
        File->Position += BlockSize * BlockCount;
    }
    atomicUnlock(&File->Lock, 0);
    free(CallbackWriteFile);
    return Status;
}

KResult kot_fputs(char* String, kot_file_t* File){
    return kot_fwrite((void*)String, strlen(String) + 1, 1, File);
}

KResult kot_fseek(kot_file_t* File, uint64_t Offset, int Whence){
    atomicAcquire(&File->Lock, 0);
    switch (Whence){
    case SEEK_SET:{
        File->Position = Offset;
        atomicUnlock(&File->Lock, 0);
        return KSUCCESS;
    }
    case SEEK_CUR:{
        File->Position += Offset;
        atomicUnlock(&File->Lock, 0);
        return KSUCCESS;
    }
    case SEEK_END:{
        struct kot_srv_storage_callback_t* CallbackFileSize = kot_Srv_Storage_Getfilesize(File, true);
        if(CallbackFileSize->Status != KSUCCESS){
            free(CallbackFileSize);
            return KFAIL;
        }
        size64_t Size = CallbackFileSize->Data;
        File->Position = Size + Offset;
        free(CallbackFileSize);
        atomicUnlock(&File->Lock, 0);
        return KSUCCESS;
    }
    default:
        atomicUnlock(&File->Lock, 0);
        return KFAIL;
    }
}

uint64_t kot_ftell(kot_file_t* File){
    atomicAcquire(&File->Lock, 0);
    uint64_t Position = File->Position;
    atomicUnlock(&File->Lock, 0);
    return Position;
}

kot_directory_t* kot_opendir(char* Path){
    if(!kot_ShareProcessFS) kot_ShareProcessFS = kot_ShareProcessKey(kot_Sys_GetProcess());
    struct kot_srv_storage_callback_t* CallbackDir = kot_Srv_Storage_DirOpen(Path, kot_ShareProcessFS, true);
    if(CallbackDir->Status != KSUCCESS){
        free(CallbackDir);
        return NULL;
    }
    kot_directory_t* Dir = (kot_directory_t*)CallbackDir->Data;
    free(CallbackDir);
    Dir->Position = NULL;
    return Dir;
}

KResult kot_closedir(kot_directory_t* Dir){
    struct kot_srv_storage_callback_t* CallbackDir = kot_Srv_Storage_Closedir(Dir, true);
    KResult Status = CallbackDir->Status;
    free(CallbackDir);
    free(Dir);
    return Status;
}

KResult kot_rewinddir(kot_directory_t* Dir){
    Dir->Position = NULL;
    return KSUCCESS;
}

KResult kot_filecount(kot_directory_t* Dir, uint64_t* Count){
    struct kot_srv_storage_callback_t* CallbackDir = kot_Srv_Storage_Getdircount(Dir, true);
    KResult Status = CallbackDir->Status;
    *Count = CallbackDir->Data;
    free(CallbackDir);
    return Status;
}

kot_directory_entries_t* kot_mreaddir(kot_directory_t* Dir, uint64_t Start, uint64_t Count){
    struct kot_srv_storage_callback_t* CallbackDir = kot_Srv_Storage_Readdir(Dir, Start, Count, true);
    kot_directory_entries_t* Entries = (kot_directory_entries_t*)CallbackDir->Data;
    free(CallbackDir);
    return Entries;
}

kot_directory_entry_t* kot_readdir(kot_directory_t* Dir){
    atomicAcquire(&Dir->Lock, 0);
    kot_directory_entries_t* Entries;
    if((Entries = kot_mreaddir(Dir, Dir->Position, 1)) == NULL){
        atomicUnlock(&Dir->Lock, 0);
        return NULL;
    }
    Dir->Position++;
    atomicUnlock(&Dir->Lock, 0);
    size64_t DirEntrySize = sizeof(kot_directory_entry_t) + strlen(Entries->FirstEntry.Name) + 1;
    kot_directory_entry_t* Entry = (kot_directory_entry_t*)malloc(DirEntrySize);
    memcpy(Entry, &Entries->FirstEntry, DirEntrySize);
    free(Entries);
    return Entry;
}

KResult kot_removefile(char* Path){
    struct kot_srv_storage_callback_t* CallbackDir = kot_Srv_Storage_DirRemove(Path, true);
    KResult Status = CallbackDir->Status;
    free(CallbackDir);
    return Status;
}

KResult kot_rename(char* OldName, char* NewName){
    struct kot_srv_storage_callback_t* CallbackDir = kot_Srv_Storage_Rename(OldName, NewName, true);
    KResult Status = CallbackDir->Status;
    free(CallbackDir);
    return Status;
}


KResult kot_mkdir(char* Path, mode_t Mode){
    struct kot_srv_storage_callback_t* CallbackDir = kot_Srv_Storage_DirCreate(Path, Mode, true);
    KResult Status = CallbackDir->Status;
    free(CallbackDir);
    return Status;
}

KResult kot_rmdir(char* Path){
    struct kot_srv_storage_callback_t* CallbackDir = kot_Srv_Storage_DirRemove(Path, true);
    KResult Status = CallbackDir->Status;
    free(CallbackDir);
    return Status;
}

char* kot_dirname(char* path){
    int len = strlen(path);
    char *slash = path + len - 1;

    while (slash > path && *slash == '/') {
        slash--;
    }
    while (slash > path && *slash != '/') {
        slash--;
    }
    if (slash == path && *slash != '/') {
        return ".";
    } else if (slash == path) {
        return "/";
    } else {
        *slash = '\0';
        return path;
    }
}

}