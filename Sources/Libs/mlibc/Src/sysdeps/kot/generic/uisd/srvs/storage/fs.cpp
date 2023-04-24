#include <kot/uisd/srvs/storage.h>
#include <stdlib.h>
#include <string.h>

namespace Kot{
    kot_process_t ShareProcessFS = NULL;

    file_t* fopen(char* Path, char* Mode){
        if(!ShareProcessFS) ShareProcessFS = ShareProcessKey(Sys_GetProcess());
        if(Mode[0] == 'r'){
            if(Mode[1] == '\0'){
                struct srv_storage_callback_t* CallbackFile = Srv_Storage_Openfile(Path, Storage_Permissions_Read, ShareProcessFS, true);
                if(CallbackFile->Status != KSUCCESS){
                    free(CallbackFile);
                    return NULL;
                }
                file_t* File = (file_t*)CallbackFile->Data;
                File->Position = NULL;
                File->Lock = NULL;
                free(CallbackFile);
                return File;
            }else if(Mode[1] == '+'){
                struct srv_storage_callback_t* CallbackFile = Srv_Storage_Openfile(Path, Storage_Permissions_Read | Storage_Permissions_Write, ShareProcessFS, true);
                if(CallbackFile->Status != KSUCCESS){
                    free(CallbackFile);
                    return NULL;
                }
                file_t* File = (file_t*)CallbackFile->Data;
                File->Position = NULL;
                File->Lock = NULL;
                free(CallbackFile);
                return File;
            }else if(Mode[1] == 'b'){
                if(Mode[2] == '\0'){
                    struct srv_storage_callback_t* CallbackFile = Srv_Storage_Openfile(Path, Storage_Permissions_Read, ShareProcessFS, true);
                    if(CallbackFile->Status != KSUCCESS){
                        free(CallbackFile);
                        return NULL;
                    }
                    file_t* File = (file_t*)CallbackFile->Data;
                    File->IsBinary = true;
                    File->Position = NULL;
                    File->Lock = NULL;
                    free(CallbackFile);
                    return File;
                }else if(Mode[2] == '+'){
                    struct srv_storage_callback_t* CallbackFile = Srv_Storage_Openfile(Path, Storage_Permissions_Read | Storage_Permissions_Write, ShareProcessFS, true);
                    if(CallbackFile->Status != KSUCCESS){
                        free(CallbackFile);
                        return NULL;
                    }
                    file_t* File = (file_t*)CallbackFile->Data;
                    File->IsBinary = true;
                    File->Position = NULL;
                    File->Lock = NULL;
                    free(CallbackFile);
                    return File;
                }
            }
        }else if(Mode[0] == 'w'){
            if(Mode[1] == '\0'){
                struct srv_storage_callback_t* CallbackFile = Srv_Storage_Openfile(Path, Storage_Permissions_Write | Storage_Permissions_Create, ShareProcessFS, true);
                if(CallbackFile->Status != KSUCCESS){
                    free(CallbackFile);
                    return NULL;
                }
                file_t* File = (file_t*)CallbackFile->Data;
                File->Position = NULL;
                File->Lock = NULL;
                free(CallbackFile);
                return File;
            }else if(Mode[1] == '+'){
                struct srv_storage_callback_t* CallbackFile = Srv_Storage_Openfile(Path, Storage_Permissions_Read | Storage_Permissions_Write | Storage_Permissions_Create, ShareProcessFS, true);
                if(CallbackFile->Status != KSUCCESS){
                    free(CallbackFile);
                    return NULL;
                }
                file_t* File = (file_t*)CallbackFile->Data;
                File->Position = NULL;
                File->Lock = NULL;
                free(CallbackFile);
                return File;
            }else if(Mode[1] == 'b'){
                if(Mode[2] == '\0'){
                    struct srv_storage_callback_t* CallbackFile = Srv_Storage_Openfile(Path, Storage_Permissions_Write | Storage_Permissions_Create, ShareProcessFS, true);
                    if(CallbackFile->Status != KSUCCESS){
                        free(CallbackFile);
                        return NULL;
                    }
                    file_t* File = (file_t*)CallbackFile->Data;
                    File->IsBinary = true;
                    File->Position = NULL;
                    File->Lock = NULL;
                    free(CallbackFile);
                    return File;
                }else if(Mode[2] == '+'){
                    struct srv_storage_callback_t* CallbackFile = Srv_Storage_Openfile(Path, Storage_Permissions_Read | Storage_Permissions_Write | Storage_Permissions_Create, ShareProcessFS, true);
                    if(CallbackFile->Status != KSUCCESS){
                        free(CallbackFile);
                        return NULL;
                    }
                    file_t* File = (file_t*)CallbackFile->Data;
                    File->IsBinary = true;
                    File->Position = NULL;
                    File->Lock = NULL;
                    free(CallbackFile);
                    return File;
                }
            }
        }else if(Mode[0] == 'a'){
            if(Mode[1] == '\0'){
                struct srv_storage_callback_t* CallbackFile = Srv_Storage_Openfile(Path, Storage_Permissions_Write | Storage_Permissions_Create, ShareProcessFS, true);
                if(CallbackFile->Status != KSUCCESS){
                    free(CallbackFile);
                    return NULL;
                }
                file_t* File = (file_t*)CallbackFile->Data;
                struct srv_storage_callback_t* CallbackFileSize = Srv_Storage_Getfilesize(File, true);
                if(CallbackFileSize->Status != KSUCCESS){
                    free(CallbackFile);
                    free(File);
                    return NULL;
                }
                size64_t Size = CallbackFileSize->Data;
                File->Position = Size;
                File->Lock = NULL;
                free(CallbackFileSize);
                free(CallbackFile);
                return File;
            }else if(Mode[1] == '+'){
                struct srv_storage_callback_t* CallbackFile = Srv_Storage_Openfile(Path, Storage_Permissions_Read | Storage_Permissions_Write | Storage_Permissions_Create, ShareProcessFS, true);
                if(CallbackFile->Status != KSUCCESS){
                    free(CallbackFile);
                    return NULL;
                }
                file_t* File = (file_t*)CallbackFile->Data;
                struct srv_storage_callback_t* CallbackFileSize = Srv_Storage_Getfilesize(File, true);
                if(CallbackFileSize->Status != KSUCCESS){
                    free(CallbackFile);
                    free(File);
                    return NULL;
                }
                size64_t Size = CallbackFileSize->Data;
                File->Position = Size;
                File->Lock = NULL;
                free(CallbackFileSize);
                free(CallbackFile);
                return File;
            }else if(Mode[1] == 'b'){
                if(Mode[2] == '\0'){
                    struct srv_storage_callback_t* CallbackFile = Srv_Storage_Openfile(Path, Storage_Permissions_Write | Storage_Permissions_Create, ShareProcessFS, true);
                    if(CallbackFile->Status != KSUCCESS){
                        free(CallbackFile);
                        return NULL;
                    }
                    file_t* File = (file_t*)CallbackFile->Data;
                    struct srv_storage_callback_t* CallbackFileSize = Srv_Storage_Getfilesize(File, true);
                    if(CallbackFileSize->Status != KSUCCESS){
                        free(CallbackFile);
                        free(File);
                        return NULL;
                    }
                    size64_t Size = CallbackFileSize->Data;
                    File->IsBinary = true;
                    File->Position = Size;
                    File->Lock = NULL;
                    free(CallbackFile);
                    return File;
                }else if(Mode[2] == '+'){
                    struct srv_storage_callback_t* CallbackFile = Srv_Storage_Openfile(Path, Storage_Permissions_Read | Storage_Permissions_Write | Storage_Permissions_Create, ShareProcessFS, true);
                    if(CallbackFile->Status != KSUCCESS){
                        free(CallbackFile);
                        return NULL;
                    }
                    file_t* File = (file_t*)CallbackFile->Data;
                    struct srv_storage_callback_t* CallbackFileSize = Srv_Storage_Getfilesize(File, true);
                    if(CallbackFileSize->Status != KSUCCESS){
                        free(CallbackFile);
                        free(File);
                        return NULL;
                    }
                    size64_t Size = CallbackFileSize->Data;
                    File->IsBinary = true;
                    File->Position = Size;
                    File->Lock = NULL;
                    free(CallbackFile);
                    return File;
                }
            }
        }
        return NULL;
    }

    KResult fclose(file_t* File){
        struct srv_storage_callback_t* CallbackCloseFile = Srv_Storage_Closefile(File, true);
        KResult Status = CallbackCloseFile->Status;
        free(CallbackCloseFile);
        free(File);
        return Status;
    }

    KResult fread(uintptr_t Buffer, size_t BlockSize, size_t BlockCount, file_t* File){
        atomicAcquire(&File->Lock, 0);
        struct srv_storage_callback_t* CallbackReadFile = Srv_Storage_Readfile(File, Buffer, File->Position, BlockSize * BlockCount, true);
        KResult Status = CallbackReadFile->Status;
        if(Status == KSUCCESS){
            File->Position += BlockSize * BlockCount;
        }
        atomicUnlock(&File->Lock, 0);
        free(CallbackReadFile);
        return Status;
    }

    KResult fwrite(uintptr_t Buffer, size_t BlockSize, size_t BlockCount, file_t* File){
        atomicAcquire(&File->Lock, 0);
        struct srv_storage_callback_t* CallbackReadFile = Srv_Storage_Writefile(File, Buffer, File->Position, BlockSize * BlockCount, File->IsDataEnd, true);
        KResult Status = CallbackReadFile->Status;
        if(Status == KSUCCESS){
            File->Position += BlockSize * BlockCount;
        }
        atomicUnlock(&File->Lock, 0);
        free(CallbackReadFile);
        return Status;
    }

    KResult fputs(char* String, file_t* File){
        return fwrite((uintptr_t)String, strlen(String) + 1, 1, File);
    }

    KResult fseek(file_t* File, uint64_t Offset, int Whence){
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
            struct srv_storage_callback_t* CallbackFileSize = Srv_Storage_Getfilesize(File, true);
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

    uint64_t ftell(file_t* File){
        atomicAcquire(&File->Lock, 0);
        uint64_t Position = File->Position;
        atomicUnlock(&File->Lock, 0);
        return Position;
    }

    directory_t* opendir(char* Path){
        if(!ShareProcessFS) ShareProcessFS = ShareProcessKey(Sys_GetProcess());
        struct srv_storage_callback_t* CallbackDir = Srv_Storage_DirOpen(Path, ShareProcessFS, true);
        if(CallbackDir->Status != KSUCCESS){
            free(CallbackDir);
            return NULL;
        }
        directory_t* Dir = (directory_t*)CallbackDir->Data;
        free(CallbackDir);
        Dir->Position = NULL;
        return Dir;
    }

    KResult closedir(directory_t* Dir){
        struct srv_storage_callback_t* CallbackDir = Srv_Storage_Closedir(Dir, true);
        KResult Status = CallbackDir->Status;
        free(CallbackDir);
        free(Dir);
        return Status;
    }

    KResult rewinddir(directory_t* Dir){
        Dir->Position = NULL;
        return KSUCCESS;
    }

    KResult filecount(directory_t* Dir, uint64_t* Count){
        struct srv_storage_callback_t* CallbackDir = Srv_Storage_Getdircount(Dir, true);
        KResult Status = CallbackDir->Status;
        *Count = CallbackDir->Data;
        free(CallbackDir);
        return Status;
    }

    directory_entries_t* mreaddir(directory_t* Dir, uint64_t Start, uint64_t Count){
        struct srv_storage_callback_t* CallbackDir = Srv_Storage_Readdir(Dir, Start, Count, true);
        directory_entries_t* Entries = (directory_entries_t*)CallbackDir->Data;
        free(CallbackDir);
        return Entries;
    }

    directory_entry_t* readdir(directory_t* Dir){
        atomicAcquire(&Dir->Lock, 0);
        directory_entries_t* Entries;
        if((Entries = mreaddir(Dir, Dir->Position, 1)) == NULL){
            atomicUnlock(&Dir->Lock, 0);
            return NULL;
        }
        Dir->Position++;
        atomicUnlock(&Dir->Lock, 0);
        size64_t DirEntrySize = sizeof(directory_entry_t) + strlen(Entries->FirstEntry.Name) + 1;
        directory_entry_t* Entry = (directory_entry_t*)malloc(DirEntrySize);
        memcpy(Entry, &Entries->FirstEntry, DirEntrySize);
        free(Entries);
        return Entry;
    }

    KResult removefile(char* Path){
        struct srv_storage_callback_t* CallbackDir = Srv_Storage_DirRemove(Path, true);
        KResult Status = CallbackDir->Status;
        free(CallbackDir);
        return Status;
    }

    KResult rename(char* OldName, char* NewName){
        struct srv_storage_callback_t* CallbackDir = Srv_Storage_Rename(OldName, NewName, true);
        KResult Status = CallbackDir->Status;
        free(CallbackDir);
        return Status;
    }


    KResult mkdir(char* Path, mode_t Mode){
        struct srv_storage_callback_t* CallbackDir = Srv_Storage_DirCreate(Path, Mode, true);
        KResult Status = CallbackDir->Status;
        free(CallbackDir);
        return Status;
    }

    KResult rmdir(char* Path){
        struct srv_storage_callback_t* CallbackDir = Srv_Storage_DirRemove(Path, true);
        KResult Status = CallbackDir->Status;
        free(CallbackDir);
        return Status;
    }

    char* dirname(char* path){
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