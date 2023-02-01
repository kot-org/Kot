#include <srv/srv.h>

static file_dispatch_t FileDispatcher[File_Function_Count] = { 
    [File_Function_Close] = Closefile,
    [File_Function_GetSize] = Getfilesize,
    [File_Function_Read] = Readfile,
    [File_Function_Write] = Writefile,
};

static dir_dispatch_t DirDispatcher[Dir_Function_Count] = { 
    [Dir_Function_Close] = Closedir,
    [Dir_Function_GetCount] = Getdircount,
    [Dir_Function_Read] = Readdir,
};



KResult MountToVFS(mount_info_t* MountInfo, process_t VFSProcess, thread_t VFSMountThread){
    srv_storage_fs_server_functions_t FSServerFunctions;

    process_t proc = Sys_GetProcess();

    /* ChangeUserData */
    thread_t ChangeUserDataThread = NULL;
    Sys_Createthread(proc, (uintptr_t)&ChangeUserData, PriviledgeDriver, (uint64_t)MountInfo, &ChangeUserDataThread);
    FSServerFunctions.ChangeUserData = MakeShareableThreadToProcess(ChangeUserDataThread, VFSProcess);

    /* Removefile */
    thread_t RemovefileThread = NULL;
    Sys_Createthread(proc, (uintptr_t)&Removefile, PriviledgeDriver, (uint64_t)MountInfo, &RemovefileThread);
    FSServerFunctions.Removefile = MakeShareableThreadToProcess(RemovefileThread, VFSProcess);

    /* Openfile */
    thread_t OpenfileThread = NULL;
    Sys_Createthread(proc, (uintptr_t)&Openfile, PriviledgeDriver, (uint64_t)MountInfo, &OpenfileThread);
    FSServerFunctions.Openfile = MakeShareableThreadToProcess(OpenfileThread, VFSProcess);

    /* Rename */
    thread_t RenameThread = NULL;
    Sys_Createthread(proc, (uintptr_t)&Rename, PriviledgeDriver, (uint64_t)MountInfo, &RenameThread);
    FSServerFunctions.Rename = MakeShareableThreadToProcess(RenameThread, VFSProcess);

    /* Mkdir */
    thread_t MkdirThread = NULL;
    Sys_Createthread(proc, (uintptr_t)&Mkdir, PriviledgeDriver, (uint64_t)MountInfo, &MkdirThread);
    FSServerFunctions.Mkdir = MakeShareableThreadToProcess(MkdirThread, VFSProcess);

    /* Rmdir */
    thread_t RmdirThread = NULL;
    Sys_Createthread(proc, (uintptr_t)&Rmdir, PriviledgeDriver, (uint64_t)MountInfo, &RmdirThread);
    FSServerFunctions.Rmdir = MakeShareableThreadToProcess(RmdirThread, VFSProcess);

    /* Opendir */
    thread_t OpendirThread = NULL;
    Sys_Createthread(proc, (uintptr_t)&Opendir, PriviledgeDriver, (uint64_t)MountInfo, &OpendirThread);
    FSServerFunctions.Opendir = MakeShareableThreadToProcess(OpendirThread, VFSProcess);

    Srv_Storage_MountPartition(VFSMountThread, &FSServerFunctions, true);

    return KSUCCESS;
}


/* VFS access */
KResult ChangeUserData(thread_t Callback, uint64_t CallbackArg, uint64_t UID, uint64_t GID, char* UserName){
    mount_info_t* MountInfo = (mount_info_t*)Sys_GetExternalDataThread();
    
    MountInfo->UID = UID;
    MountInfo->GID = GID;
    memcpy(MountInfo->UserName, UserName, strlen(UserName));

    arguments_t arguments{
        .arg[0] = KSUCCESS,         /* Status */
        .arg[1] = CallbackArg,      /* CallbackArg */
        .arg[2] = NULL,             /* GP0 */
        .arg[3] = NULL,             /* GP1 */
        .arg[4] = NULL,             /* GP2 */
        .arg[5] = NULL,             /* GP3 */
    };

    Sys_Execthread(Callback, &arguments, ExecutionTypeQueu, NULL);
    Sys_Close(KSUCCESS);
}

/* Files */

/* VFS access */
KResult Removefile(thread_t Callback, uint64_t CallbackArg, char* Path, permissions_t Permissions){
    mount_info_t* MountInfo = (mount_info_t*)Sys_GetExternalDataThread();
    KResult Status = MountInfo->RemoveFile(Path, Permissions);
    
    arguments_t arguments{
        .arg[0] = Status,           /* Status */
        .arg[1] = CallbackArg,      /* CallbackArg */
        .arg[2] = NULL,             /* GP0 */
        .arg[3] = NULL,             /* GP1 */
        .arg[4] = NULL,             /* GP2 */
        .arg[5] = NULL,             /* GP3 */
    };

    Sys_Execthread(Callback, &arguments, ExecutionTypeQueu, NULL);
    Sys_Close(KSUCCESS);
}

/* VFS access */
KResult Openfile(thread_t Callback, uint64_t CallbackArg, char* Path, permissions_t Permissions, process_t Target){
    mount_info_t* MountInfo = (mount_info_t*)Sys_GetExternalDataThread();
    KResult Status = KFAIL;

    ext_file_t* File = NULL;

    if(File = MountInfo->OpenFile(Path, Permissions)){
        File->Target = Target;
        Status = KSUCCESS;
    }

    
    arguments_t arguments{
        .arg[0] = Status,           /* Status */
        .arg[1] = CallbackArg,      /* CallbackArg */
        .arg[2] = NULL,             /* Data */
        .arg[3] = NULL,             /* GP1 */
        .arg[4] = NULL,             /* GP2 */
        .arg[5] = NULL,             /* GP3 */
    };

    if(Status == KSUCCESS){
        srv_storage_fs_server_open_file_data_t SrvOpenFileData;
        thread_t DispatcherThread;

        Sys_Createthread(Sys_GetProcess(), (uintptr_t)&FileDispatch, PriviledgeDriver, (uint64_t)File, &DispatcherThread);

        SrvOpenFileData.Dispatcher = MakeShareableThreadToProcess(DispatcherThread, Target);

        SrvOpenFileData.FSDriverProc = ProcessKey;
        
        ShareDataWithArguments_t ShareDataWithArguments{
            .Data = &SrvOpenFileData,
            .Size = sizeof(srv_storage_fs_server_open_file_data_t),
            .ParameterPosition = 0x2,
        };
        Sys_Execthread(Callback, &arguments, ExecutionTypeQueu, &ShareDataWithArguments);
    }else{
        Sys_Execthread(Callback, &arguments, ExecutionTypeQueu, NULL);
    }
    Sys_Close(KSUCCESS);
}

/* Direct access */
KResult FileDispatch(thread_t Callback, uint64_t CallbackArg, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3){
    uint64_t Function = GP0;

    if(Function >= File_Function_Count){
        arguments_t arguments{
            .arg[0] = KFAIL,            /* Status */
            .arg[1] = CallbackArg,      /* CallbackArg */
            .arg[2] = NULL,             /* GP0 */
            .arg[3] = NULL,             /* GP1 */
            .arg[4] = NULL,             /* GP2 */
            .arg[5] = NULL,             /* GP3 */
        };

        Sys_Execthread(Callback, &arguments, ExecutionTypeQueu, NULL);
        Sys_Close(KSUCCESS);
    }

    ext_file_t* File = (ext_file_t*)Sys_GetExternalDataThread();
    Sys_Close(FileDispatcher[Function](Callback, CallbackArg, File, GP1, GP2, GP3)); // It'll call the callback in the function
}

/* Direct access */
KResult Closefile(thread_t Callback, uint64_t CallbackArg, ext_file_t* File, uint64_t GP0, uint64_t GP1, uint64_t GP2){
    KResult Status = File->CloseFile();
    
    arguments_t arguments{
        .arg[0] = Status,           /* Status */
        .arg[1] = CallbackArg,      /* CallbackArg */
        .arg[2] = NULL,             /* GP0 */
        .arg[3] = NULL,             /* GP1 */
        .arg[4] = NULL,             /* GP2 */
        .arg[5] = NULL,             /* GP3 */
    };

    Sys_Execthread(Callback, &arguments, ExecutionTypeQueu, NULL);
    
    if(Status == KSUCCESS){
        Sys_Exit(KSUCCESS);
    }

    return KSUCCESS;
}

/* Direct access */
KResult Getfilesize(thread_t Callback, uint64_t CallbackArg, ext_file_t* File, uint64_t GP0, uint64_t GP1, uint64_t GP2){
    arguments_t arguments{
        .arg[0] = KSUCCESS,             /* Status */
        .arg[1] = CallbackArg,          /* CallbackArg */
        .arg[2] = File->GetSize(),      /* FileSize */
        .arg[3] = NULL,                 /* GP1 */
        .arg[4] = NULL,                 /* GP2 */
        .arg[5] = NULL,                 /* GP3 */
    };
    Sys_Execthread(Callback, &arguments, ExecutionTypeQueu, NULL);
    return KSUCCESS;
}

/* Direct access */
KResult Readfile(thread_t Callback, uint64_t CallbackArg, ext_file_t* File, uint64_t GP0, uint64_t GP1, uint64_t GP2){
    size64_t Size = GP1;
    ksmem_t BufferKey;

    KResult Status = File->ReadFile(&BufferKey, GP0, Size);

    arguments_t arguments{
        .arg[0] = Status,           /* Status */
        .arg[1] = CallbackArg,      /* CallbackArg */
        .arg[2] = NULL,             /* Key to buffer */
        .arg[3] = NULL,             /* GP1 */
        .arg[4] = NULL,             /* GP2 */
        .arg[5] = NULL,             /* GP3 */
    };

    if(Status == KSUCCESS){
        Sys_Keyhole_CloneModify(BufferKey, &arguments.arg[2], File->Target, KeyholeFlagPresent | KeyholeFlagCloneable | KeyholeFlagEditable, PriviledgeApp);
        Sys_Execthread(Callback, &arguments, ExecutionTypeQueu, NULL);
    }else{
        Sys_Execthread(Callback, &arguments, ExecutionTypeQueu, NULL);
    }

    return KSUCCESS;
}

/* Direct access */
KResult Writefile(thread_t Callback, uint64_t CallbackArg, ext_file_t* File, uint64_t GP0, uint64_t GP1, uint64_t GP2){
    KResult Status = KFAIL;
    
    uint64_t TypePointer;
    uint64_t Size;
    if(Sys_GetInfoMemoryField(GP0, &TypePointer, &Size) == KSUCCESS){
        if(TypePointer == MemoryFieldTypeSendSpaceRO){            
            uintptr_t Buffer = malloc(Size);
            assert(Sys_AcceptMemoryField(Sys_GetProcess(), GP0, &Buffer) == KSUCCESS);

            Status = File->WriteFile(Buffer, GP1, Size, GP2);
        }
    }
    
    arguments_t arguments{
        .arg[0] = Status,            /* Status */
        .arg[1] = CallbackArg,      /* CallbackArg */
        .arg[2] = NULL,             /* GP0 */
        .arg[3] = NULL,             /* GP1 */
        .arg[4] = NULL,             /* GP2 */
        .arg[5] = NULL,             /* GP3 */
    };

    Sys_Execthread(Callback, &arguments, ExecutionTypeQueu, NULL);

    return KSUCCESS;
}


/* Files and directories */

/* VFS access */
KResult Rename(thread_t Callback, uint64_t CallbackArg, srv_storage_fs_server_rename_t* RenameData, permissions_t Permissions){
    char* OldPath = (char*)((uint64_t)RenameData + RenameData->OldPathPosition);
    char* NewPath = (char*)((uint64_t)RenameData + RenameData->NewPathPosition);
    
    mount_info_t* MountInfo = (mount_info_t*)Sys_GetExternalDataThread();

    KResult Status = MountInfo->Rename(OldPath, NewPath, Permissions);
    
    arguments_t arguments{
        .arg[0] = Status,            /* Status */
        .arg[1] = CallbackArg,      /* CallbackArg */
        .arg[2] = NULL,             /* GP0 */
        .arg[3] = NULL,             /* GP1 */
        .arg[4] = NULL,             /* GP2 */
        .arg[5] = NULL,             /* GP3 */
    };

    Sys_Execthread(Callback, &arguments, ExecutionTypeQueu, NULL);
    Sys_Close(KSUCCESS);
}


/* Directories */

/* VFS access */
KResult Mkdir(thread_t Callback, uint64_t CallbackArg, char* Path, mode_t Mode, permissions_t Permissions){
    mount_info_t* MountInfo = (mount_info_t*)Sys_GetExternalDataThread();

    KResult Status = MountInfo->CreateDir(Path, Mode, Permissions);
    
    arguments_t arguments{
        .arg[0] = Status,            /* Status */
        .arg[1] = CallbackArg,      /* CallbackArg */
        .arg[2] = NULL,             /* GP0 */
        .arg[3] = NULL,             /* GP1 */
        .arg[4] = NULL,             /* GP2 */
        .arg[5] = NULL,             /* GP3 */
    };

    Sys_Execthread(Callback, &arguments, ExecutionTypeQueu, NULL);
    Sys_Close(KSUCCESS);
}

/* VFS access */
KResult Rmdir(thread_t Callback, uint64_t CallbackArg, char* Path, permissions_t Permissions){
    mount_info_t* MountInfo = (mount_info_t*)Sys_GetExternalDataThread();
    KResult Status = MountInfo->RemoveDir(Path, Permissions);
    
    arguments_t arguments{
        .arg[0] = Status,            /* Status */
        .arg[1] = CallbackArg,      /* CallbackArg */
        .arg[2] = NULL,             /* GP0 */
        .arg[3] = NULL,             /* GP1 */
        .arg[4] = NULL,             /* GP2 */
        .arg[5] = NULL,             /* GP3 */
    };

    Sys_Execthread(Callback, &arguments, ExecutionTypeQueu, NULL);
    Sys_Close(KSUCCESS);
}

/* VFS access */
KResult Opendir(thread_t Callback, uint64_t CallbackArg, char* Path, permissions_t Permissions, process_t Target){
    mount_info_t* MountInfo = (mount_info_t*)Sys_GetExternalDataThread();
    KResult Status = KFAIL;

    ext_directory_t* Directory = NULL;
    
    if(Directory = MountInfo->OpenDir(Path, Permissions)){
        Status = KSUCCESS;
    }
    
    arguments_t arguments{
        .arg[0] = Status,            /* Status */
        .arg[1] = CallbackArg,      /* CallbackArg */
        .arg[2] = NULL,             /* Data */
        .arg[3] = NULL,             /* GP1 */
        .arg[4] = NULL,             /* GP2 */
        .arg[5] = NULL,             /* GP3 */
    };

    if(Status == KSUCCESS){
        srv_storage_fs_server_open_dir_data_t SrvOpenDirData;
        thread_t DispatcherThread;

        Sys_Createthread(Sys_GetProcess(), (uintptr_t)&DirDispatch, PriviledgeDriver, (uint64_t)Directory, &DispatcherThread);

        Sys_Keyhole_CloneModify(DispatcherThread, &SrvOpenDirData.Dispatcher, Target, KeyholeFlagPresent | KeyholeFlagDataTypeThreadIsExecutableWithQueue, PriviledgeApp);

        SrvOpenDirData.FSDriverProc = ProcessKey;
        
        ShareDataWithArguments_t ShareDataWithArguments{
            .Data = &SrvOpenDirData,
            .Size = sizeof(srv_storage_fs_server_open_dir_data_t),
            .ParameterPosition = 0x2,
        };
        Sys_Execthread(Callback, &arguments, ExecutionTypeQueu, &ShareDataWithArguments);
    }else{
        Sys_Execthread(Callback, &arguments, ExecutionTypeQueu, NULL);
    }
    Sys_Close(KSUCCESS);
}

/* Direct access */
KResult DirDispatch(thread_t Callback, uint64_t CallbackArg, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3){
    uint64_t Function = GP0;

    if(Function >= Dir_Function_Count){
        arguments_t arguments{
            .arg[0] = KFAIL,            /* Status */
            .arg[1] = CallbackArg,      /* CallbackArg */
            .arg[2] = NULL,             /* GP0 */
            .arg[3] = NULL,             /* GP1 */
            .arg[4] = NULL,             /* GP2 */
            .arg[5] = NULL,             /* GP3 */
        };

        Sys_Execthread(Callback, &arguments, ExecutionTypeQueu, NULL);
        Sys_Close(KSUCCESS);
    }

    ext_directory_t* Directory = (ext_directory_t*)Sys_GetExternalDataThread();

    Sys_Close(DirDispatcher[Function](Callback, CallbackArg, Directory, GP1, GP2, GP3));
}

/* Direct access */
KResult Readdir(thread_t Callback, uint64_t CallbackArg, ext_directory_t* Directory, uint64_t GP0, uint64_t GP1, uint64_t GP2){
    uint64_t IndexCount = GP1;
    uint64_t IndexStart = GP0;

    read_dir_data** ReadirData = (read_dir_data**)malloc(sizeof(read_dir_data*) * IndexCount);

    uint64_t EntryCount = 0;
    size64_t DataSize = sizeof(directory_entries_t);
    for(uint64_t i = 0; i < IndexCount; i++){
        ReadirData[i] = Directory->ReadDir(IndexStart + i);
        if(ReadirData[i] == NULL) break;
        DataSize += sizeof(directory_entry_t);
        DataSize += ReadirData[i]->NameLength + 1;
        EntryCount++;
    }

    directory_entries_t* Data = (directory_entries_t*)malloc(DataSize);
    Data->EntryCount = EntryCount;

    uint64_t NextEntrYPosition = sizeof(directory_entries_t);
    directory_entry_t* Entry = &Data->FirstEntry;
    for(uint64_t i = 0; i < EntryCount; i++){
        NextEntrYPosition += sizeof(directory_entry_t) + ReadirData[i]->NameLength + 1;
        Entry->NextEntrYPosition = NextEntrYPosition;
        Entry->IsFile = ReadirData[i]->IsFile;
        memcpy(&Entry->Name, ReadirData[i]->Name, ReadirData[i]->NameLength + 1);
        free(ReadirData[i]->Name);
        free(ReadirData[i]);
        Entry = (directory_entry_t*)((uint64_t)&Data->FirstEntry + (uint64_t)NextEntrYPosition);
    }

    Entry->NextEntrYPosition = NULL;

    free(ReadirData);

    
    arguments_t arguments{
        .arg[0] = KSUCCESS,         /* Status */
        .arg[1] = CallbackArg,      /* CallbackArg */
        .arg[2] = NULL,             /* GP0 */
        .arg[3] = DataSize,         /* GP1 */
        .arg[4] = NULL,             /* GP2 */
        .arg[5] = NULL,             /* GP3 */
    };

    ShareDataWithArguments_t ShareDataWithArguments{
        .Data = Data,
        .Size = DataSize,
        .ParameterPosition = 0x2,
    };
    Sys_Execthread(Callback, &arguments, ExecutionTypeQueu, &ShareDataWithArguments);
    free(Data);
    Sys_Close(KSUCCESS);
}

/* Direct access */
KResult Getdircount(thread_t Callback, uint64_t CallbackArg, ext_directory_t* Directory, uint64_t GP0, uint64_t GP1, uint64_t GP2){
    arguments_t arguments{
        .arg[0] = KSUCCESS,                 /* Status */
        .arg[1] = CallbackArg,              /* CallbackArg */
        .arg[2] = Directory->GetDirCount(), /* DirCount */
        .arg[3] = NULL,                     /* GP1 */
        .arg[4] = NULL,                     /* GP2 */
        .arg[5] = NULL,                     /* GP3 */
    };

    Sys_Execthread(Callback, &arguments, ExecutionTypeQueu, NULL);
    return KSUCCESS;
}

/* Direct access */
KResult Closedir(thread_t Callback, uint64_t CallbackArg, ext_directory_t* Directory, uint64_t GP0, uint64_t GP1, uint64_t GP2){
    KResult Status = Directory->CloseDir();
    
    arguments_t arguments{
        .arg[0] = Status,            /* Status */
        .arg[1] = CallbackArg,      /* CallbackArg */
        .arg[2] = NULL,             /* GP0 */
        .arg[3] = NULL,             /* GP1 */
        .arg[4] = NULL,             /* GP2 */
        .arg[5] = NULL,             /* GP3 */
    };

    Sys_Execthread(Callback, &arguments, ExecutionTypeQueu, NULL);
    Sys_Exit(KSUCCESS);
}