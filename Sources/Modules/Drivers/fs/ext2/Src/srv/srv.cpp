#include <srv/srv.h>

static file_dispatch_t FileDispatcher[File_Function_Count] = { 
    [File_Function_Close] = Closefile,
    [File_Function_Read] = Readfile,
    [File_Function_Write] = Writefile,
};

static dir_dispatch_t DirDispatcher[Dir_Function_Count] = { 
    [Dir_Function_Close] = Closedir,
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
    uint32_t Function = GP0 & 0xffffffff;
    uint32_t GP4 =  GP0 >> 32;

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
    Sys_Close(FileDispatcher[Function](Callback, CallbackArg, File, GP1, GP2, GP3, GP4)); // It'll call the callback in the function
}

/* Direct access */
KResult Closefile(thread_t Callback, uint64_t CallbackArg, ext_file_t* File, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint32_t GP3){
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
KResult Readfile(thread_t Callback, uint64_t CallbackArg, ext_file_t* File, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint32_t GP3){
    size64_t Size = GP2;

    uintptr_t Buffer = malloc(Size);

    KResult Status = File->ReadFile(Buffer, GP1, Size);

    arguments_t arguments{
        .arg[0] = Status,           /* Status */
        .arg[1] = CallbackArg,      /* CallbackArg */
        .arg[2] = NULL,             /* Key to buffer */
        .arg[3] = NULL,             /* GP1 */
        .arg[4] = NULL,             /* GP2 */
        .arg[5] = NULL,             /* GP3 */
    };

    if(Status == KSUCCESS){
        ksmem_t MemoryKey;
        Sys_CreateMemoryField(Sys_GetProcess(), Size, &Buffer, &MemoryKey, MemoryFieldTypeSendSpaceRO);

        uint64_t KeyFlag = NULL;
        Keyhole_SetFlag(&KeyFlag, KeyholeFlagPresent, true);
        Keyhole_SetFlag(&KeyFlag, KeyholeFlagCloneable, true);
        Keyhole_SetFlag(&KeyFlag, KeyholeFlagEditable, true);
        Sys_Keyhole_CloneModify(MemoryKey, &arguments.arg[2], GP0, KeyFlag, PriviledgeApp);
        
        Sys_Execthread(Callback, &arguments, ExecutionTypeQueuAwait, NULL);
        Sys_CloseMemoryField(Sys_GetProcess(), MemoryKey, Buffer);
    }else{
        Sys_Execthread(Callback, &arguments, ExecutionTypeQueu, NULL);
    }

    free(Buffer);

    return KSUCCESS;
}

/* Direct access */
KResult Writefile(thread_t Callback, uint64_t CallbackArg, ext_file_t* File, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint32_t GP3){
    KResult Status = KFAIL;
    
    uint64_t TypePointer;
    uint64_t SizePointer;
    if(Sys_GetInfoMemoryField(GP0, &TypePointer, &SizePointer) == KSUCCESS){
        if(TypePointer == MemoryFieldTypeSendSpaceRO){
            size64_t Size = GP1;
            
            uintptr_t Buffer = malloc(Size);
            Sys_AcceptMemoryField(Sys_GetProcess(), GP0, &Buffer);

            Status = File->WriteFile(Buffer, GP1, GP2, GP3);
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
KResult Mkdir(thread_t Callback, uint64_t CallbackArg, char* Path, permissions_t Permissions){
    mount_info_t* MountInfo = (mount_info_t*)Sys_GetExternalDataThread();

    KResult Status = MountInfo->CreateDir(Path, Permissions);
    
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
        uint64_t ThreadFlags = NULL;

        Sys_Createthread(Sys_GetProcess(), (uintptr_t)&DirDispatch, PriviledgeDriver, (uint64_t)Directory, &DispatcherThread);

        Keyhole_SetFlag(&ThreadFlags, KeyholeFlagPresent, true);
        Keyhole_SetFlag(&ThreadFlags, KeyholeFlagDataTypeThreadIsExecutableWithQueue, true);

        Sys_Keyhole_CloneModify(DispatcherThread, &SrvOpenDirData.Dispatcher, Target, ThreadFlags, PriviledgeApp);

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
    uint32_t Function = GP0 & 0xffffffff;
    uint32_t GP4 =  GP0 >> 32;

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

    Sys_Close(DirDispatcher[Function](Callback, CallbackArg, Directory, GP1, GP2, GP3, GP4));
}

/* Direct access */
KResult Readdir(thread_t Callback, uint64_t CallbackArg, ext_directory_t* Directory, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint32_t GP3){
    KResult Status = KFAIL;
    
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

/* Direct access */
KResult Closedir(thread_t Callback, uint64_t CallbackArg, ext_directory_t* Directory, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint32_t GP3){
    KResult Status = KFAIL;
    
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