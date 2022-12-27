#include <kot/uisd/srvs/storage.h>

file_t* fopen(char* Path, char* Mode){
    if(Mode[0] == 'r'){
        if(Mode[1] == '\0'){
            struct srv_storage_callback_t* CallbackFile = Srv_Storage_Openfile(Path, Storage_Permissions_Read, ShareProcessKey(Sys_GetProcess()), true);
            if(CallbackFile->Status != KSUCCESS){
                free(CallbackFile);
                return NULL;
            }
            file_t* File = (file_t*)CallbackFile->Data;
            File->PositionRead = NULL;
            File->PositionWrite = NULL;
            free(CallbackFile);
            return CallbackFile->Data;
        }else if(Mode[1] == '+'){
            struct srv_storage_callback_t* CallbackFile = Srv_Storage_Openfile(Path, Storage_Permissions_Read | Storage_Permissions_Write, ShareProcessKey(Sys_GetProcess()), true);
            if(CallbackFile->Status != KSUCCESS){
                free(CallbackFile);
                return NULL;
            }
            file_t* File = (file_t*)CallbackFile->Data;
            File->PositionRead = NULL;
            File->PositionWrite = NULL;
            free(CallbackFile);
            return CallbackFile->Data;
        }else if(Mode[1] == 'b'){
            if(Mode[1] == '\0'){
                struct srv_storage_callback_t* CallbackFile = Srv_Storage_Openfile(Path, Storage_Permissions_Read, ShareProcessKey(Sys_GetProcess()), true);
                if(CallbackFile->Status != KSUCCESS){
                    free(CallbackFile);
                    return NULL;
                }
                file_t* File = (file_t*)CallbackFile->Data;
                File->IsBinary = true;
                File->PositionRead = NULL;
                File->PositionWrite = NULL;
                free(CallbackFile);
                return CallbackFile->Data;
            }else if(Mode[1] == '+'){
                struct srv_storage_callback_t* CallbackFile = Srv_Storage_Openfile(Path, Storage_Permissions_Read | Storage_Permissions_Write, ShareProcessKey(Sys_GetProcess()), true);
                if(CallbackFile->Status != KSUCCESS){
                    free(CallbackFile);
                    return NULL;
                }
                file_t* File = (file_t*)CallbackFile->Data;
                File->IsBinary = true;
                File->PositionRead = NULL;
                File->PositionWrite = NULL;
                free(CallbackFile);
                return CallbackFile->Data;
            }
        }
    }else if(Mode[0] == 'w'){
        if(Mode[1] == '\0'){
            struct srv_storage_callback_t* CallbackFile = Srv_Storage_Openfile(Path, Storage_Permissions_Write |  Storage_Permissions_Create, ShareProcessKey(Sys_GetProcess()), true);
            if(CallbackFile->Status != KSUCCESS){
                free(CallbackFile);
                return NULL;
            }
            file_t* File = (file_t*)CallbackFile->Data;
            File->PositionRead = NULL;
            File->PositionWrite = NULL;
            free(CallbackFile);
            return CallbackFile->Data;
        }else if(Mode[1] == '+'){
            struct srv_storage_callback_t* CallbackFile = Srv_Storage_Openfile(Path, Storage_Permissions_Read | Storage_Permissions_Write |  Storage_Permissions_Create, ShareProcessKey(Sys_GetProcess()), true);
            if(CallbackFile->Status != KSUCCESS){
                free(CallbackFile);
                return NULL;
            }
            file_t* File = (file_t*)CallbackFile->Data;
            File->PositionRead = NULL;
            File->PositionWrite = NULL;
            free(CallbackFile);
            return CallbackFile->Data;
        }else if(Mode[1] == 'b'){
            if(Mode[1] == '\0'){
                struct srv_storage_callback_t* CallbackFile = Srv_Storage_Openfile(Path, Storage_Permissions_Write |  Storage_Permissions_Create, ShareProcessKey(Sys_GetProcess()), true);
                if(CallbackFile->Status != KSUCCESS){
                    free(CallbackFile);
                    return NULL;
                }
                file_t* File = (file_t*)CallbackFile->Data;
                File->IsBinary = true;
                File->PositionRead = NULL;
                File->PositionWrite = NULL;
                free(CallbackFile);
                return CallbackFile->Data;
            }else if(Mode[1] == '+'){
                struct srv_storage_callback_t* CallbackFile = Srv_Storage_Openfile(Path, Storage_Permissions_Read | Storage_Permissions_Write |  Storage_Permissions_Create, ShareProcessKey(Sys_GetProcess()), true);
                if(CallbackFile->Status != KSUCCESS){
                    free(CallbackFile);
                    return NULL;
                }
                file_t* File = (file_t*)CallbackFile->Data;
                File->IsBinary = true;
                File->PositionRead = NULL;
                File->PositionWrite = NULL;
                free(CallbackFile);
                return CallbackFile->Data;
            }
        }
    }else if(Mode[0] == 'a'){
        if(Mode[1] == '\0'){
            struct srv_storage_callback_t* CallbackFile = Srv_Storage_Openfile(Path, Storage_Permissions_Read, ShareProcessKey(Sys_GetProcess()), true);
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
            File->PositionRead = Size;
            File->PositionWrite = Size;
            free(CallbackFileSize);
            free(CallbackFile);
            return CallbackFile->Data;
        }else if(Mode[1] == '+'){
            struct srv_storage_callback_t* CallbackFile = Srv_Storage_Openfile(Path, Storage_Permissions_Read | Storage_Permissions_Write, ShareProcessKey(Sys_GetProcess()), true);
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
            File->PositionRead = NULL;
            File->PositionWrite = Size;
            free(CallbackFileSize);
            free(CallbackFile);
            return CallbackFile->Data;
        }else if(Mode[1] == 'b'){
            if(Mode[1] == '\0'){
                struct srv_storage_callback_t* CallbackFile = Srv_Storage_Openfile(Path, Storage_Permissions_Read, ShareProcessKey(Sys_GetProcess()), true);
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
                File->PositionRead = Size;
                File->PositionWrite = Size;
                free(CallbackFile);
                return CallbackFile->Data;
            }else if(Mode[1] == '+'){
                struct srv_storage_callback_t* CallbackFile = Srv_Storage_Openfile(Path, Storage_Permissions_Read | Storage_Permissions_Write, ShareProcessKey(Sys_GetProcess()), true);
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
                File->PositionRead = NULL;
                File->PositionWrite = Size;
                free(CallbackFile);
                return CallbackFile->Data;
            }
        }
    }
    return NULL;
}

KResult fclose(file_t* File){
    struct srv_storage_callback_t* CallbackCloseFile = Srv_Storage_Closefile(File, true);
    KResult Status = CallbackCloseFile->Status;
    free(CallbackCloseFile);
    return Status;
}

KResult fread(uintptr_t Buffer, size_t BlockSize, size_t BlockCount, file_t* File){
    struct srv_storage_callback_t* CallbackReadFile = Srv_Storage_Readfile(File, Buffer, File->PositionRead, BlockSize * BlockCount, true);
    KResult Status = CallbackReadFile->Status;
    free(CallbackReadFile);
    return Status;
}

KResult fwrite(uintptr_t Buffer, size_t BlockSize, size_t BlockCount, file_t* File){
    struct srv_storage_callback_t* CallbackReadFile = Srv_Storage_Writefile(File, Buffer, File->PositionWrite, BlockSize * BlockCount, File->IsDataEnd, true);
    KResult Status = CallbackReadFile->Status;
    free(CallbackReadFile);
    return Status;
}

KResult fputs(char* String, file_t* File){
    return fwrite(String, strlen(String), 1, File);
}

KResult fseek(file_t* File, uint64_t Offset, int Whence){
    switch (Whence){
    case SEEK_SET:{
        File->PositionRead = Offset;
        File->PositionWrite = Offset;
        return KSUCCESS;
    }
    case SEEK_CUR:{
        File->PositionRead += Offset;
        File->PositionWrite += Offset;
        return KSUCCESS;
    }
    case SEEK_END:{
        struct srv_storage_callback_t* CallbackFileSize = Srv_Storage_Getfilesize(File, true);
        if(CallbackFileSize->Status != KSUCCESS){
            return KFAIL;
        }
        size64_t Size = CallbackFileSize->Data;
        File->PositionRead = Size + Offset;
        File->PositionWrite = Size + Offset;
        free(CallbackFileSize);
        return KSUCCESS;
    }
    default:
        return KFAIL;
    }
}

uint64_t ftell(file_t* File){
    return File->PositionRead;
}

KResult opendir(char* Path){

}

KResult readdir(directory_t* Directory){

}

KResult closedir(directory_t* Directory){

}

KResult removefile(char* Path){

}

KResult rename(char* OldName, char* NewName){

}


KResult mkdir(char* Path, mode_t Mode){

}

KResult rmdir(char* Path){

}