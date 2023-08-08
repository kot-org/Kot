#include <srv/srv.h>

#include <kot/ioctlsrv.h>

static shell_dispatch_t ShellDispatcher[File_Function_Count] = { 
    [File_Function_Close] = Closeshell,
    [File_Function_GetSize] = Getshellsize,
    [File_Function_Read] = Readshell,
    [File_Function_Write] = Writeshell,
    [File_Function_Ioctl] = Ioctlshell,
};

kot_process_t ProcessKey;

KResult SrvInitalize(){
    ProcessKey = kot_ShareProcessKey(kot_Sys_GetProcess());

    /* Enable shell as file */
    kot_srv_storage_fs_server_functions_t FSServerFunctions;

    kot_thread_t ThreadOpenfile;
    kot_Sys_CreateThread(kot_Sys_GetProcess(), (void*)&OpenShell, PriviledgeApp, NULL, &ThreadOpenfile);
    FSServerFunctions.Openfile = kot_MakeShareableSpreadThreadToProcess(ThreadOpenfile, ((kot_uisd_storage_t*)kot_FindControllerUISD(ControllerTypeEnum_Storage))->ControllerHeader.Process);

    kot_srv_storage_callback_t* StorageCallback = kot_Srv_Storage_NewDev("tty", &FSServerFunctions, true);


    /* Load uisd controller */
    void* address = kot_GetFreeAlignedSpace(sizeof(kot_uisd_shell_t));
    kot_key_mem_t key = NULL;
    kot_Sys_CreateMemoryField(kot_Sys_GetProcess(), sizeof(kot_uisd_shell_t), &address, &key, MemoryFieldTypeShareSpaceRO);

    kot_uisd_shell_t* ShellSrv = (kot_uisd_shell_t*)address;
    ShellSrv->ControllerHeader.IsReadWrite = false;
    ShellSrv->ControllerHeader.Version = Shell_Srv_Version;
    ShellSrv->ControllerHeader.VendorID = Kot_VendorID;
    ShellSrv->ControllerHeader.Type = ControllerTypeEnum_Shell;

    ShellSrv->IsAvailableAsFile = (StorageCallback->Status == KSUCCESS);

    kot_uisd_callbackInfo_t* info = kot_CreateControllerUISD(ControllerTypeEnum_Shell, key, true);
    free(info);

    return KSUCCESS;
}

KResult OpenShell(kot_thread_t Callback, uint64_t CallbackArg, char* Path, kot_permissions_t Permissions, kot_process_t Target){
    shell_t* Shell = NewShell(Target);
    
    kot_srv_storage_fs_server_open_file_data_t SrvOpenFileData;
    kot_thread_t DispatcherThread;

    kot_Sys_CreateThread(kot_Sys_GetProcess(), (void*)&ShellDispatch, PriviledgeDriver, (uint64_t)Shell, &DispatcherThread);

    SrvOpenFileData.Dispatcher = kot_MakeShareableThreadToProcess(DispatcherThread, Target);

    SrvOpenFileData.FSDriverProc = ProcessKey;

    kot_arguments_t arguments{
        .arg[0] = KSUCCESS,         /* Status */
        .arg[1] = CallbackArg,      /* CallbackArg */
        .arg[2] = NULL,             /* Data */
        .arg[3] = NULL,             /* GP1 */
        .arg[4] = NULL,             /* GP2 */
        .arg[5] = NULL,             /* GP3 */
    };
    
    kot_ShareDataWithArguments_t ShareDataWithArguments{
        .Data = (void*)&SrvOpenFileData,
        .Size = sizeof(kot_srv_storage_fs_server_open_file_data_t),
        .ParameterPosition = 0x2,
    };

    kot_Sys_ExecThread(Callback, &arguments, ExecutionTypeQueu, &ShareDataWithArguments);
    kot_Sys_Close(KSUCCESS);
}

KResult ShellDispatch(kot_thread_t Callback, uint64_t CallbackArg, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3){
    uint64_t Function = GP0;

    if(Function >= File_Function_Count){
        kot_arguments_t arguments{
            .arg[0] = KFAIL,            /* Status */
            .arg[1] = CallbackArg,      /* CallbackArg */
            .arg[2] = NULL,             /* GP0 */
            .arg[3] = NULL,             /* GP1 */
            .arg[4] = NULL,             /* GP2 */
            .arg[5] = NULL,             /* GP3 */
        };

        kot_Sys_ExecThread(Callback, &arguments, ExecutionTypeQueu, NULL);
        kot_Sys_Close(KSUCCESS);
    }

    shell_t* Shell = (shell_t*)kot_Sys_GetExternalDataThread();
    kot_Sys_Close(ShellDispatcher[Function](Callback, CallbackArg, Shell, GP1, GP2, GP3)); // It'll call the callback in the function
}

/* Direct access */
KResult Closeshell(kot_thread_t Callback, uint64_t CallbackArg, shell_t* Shell, uint64_t GP0, uint64_t GP1, uint64_t GP2){
    KResult Status = KSUCCESS;
    
    kot_arguments_t arguments{
        .arg[0] = Status,           /* Status */
        .arg[1] = CallbackArg,      /* CallbackArg */
        .arg[2] = NULL,             /* GP0 */
        .arg[3] = NULL,             /* GP1 */
        .arg[4] = NULL,             /* GP2 */
        .arg[5] = NULL,             /* GP3 */
    };

    kot_Sys_ExecThread(Callback, &arguments, ExecutionTypeQueu, NULL);
    
    if(Status == KSUCCESS){
        kot_Sys_Exit(KSUCCESS);
    }

    return KSUCCESS;
}

/* Direct access */
KResult Getshellsize(kot_thread_t Callback, uint64_t CallbackArg, shell_t* Shell, uint64_t GP0, uint64_t GP1, uint64_t GP2){
    kot_arguments_t arguments{
        .arg[0] = KSUCCESS,             /* Status */
        .arg[1] = CallbackArg,          /* CallbackArg */
        .arg[2] = 0,                    /* FileSize */
        .arg[3] = NULL,                 /* GP1 */
        .arg[4] = NULL,                 /* GP2 */
        .arg[5] = NULL,                 /* GP3 */
    };
    kot_Sys_ExecThread(Callback, &arguments, ExecutionTypeQueu, NULL);
    return KSUCCESS;
}

/* Direct access */
KResult Readshell(kot_thread_t Callback, uint64_t CallbackArg, shell_t* Shell, uint64_t GP0, uint64_t GP1, uint64_t GP2){
    // Ignore GP0 : start
    kot_process_t TargetDataProc = static_cast<kot_process_t>(GP2);
    return ShellCreateRequest(Shell, Callback, CallbackArg, GP1, TargetDataProc);
}

/* Direct access */
KResult Writeshell(kot_thread_t Callback, uint64_t CallbackArg, shell_t* Shell, uint64_t GP0, uint64_t GP1, uint64_t GP2){
    KResult Status = KFAIL;
    
    uint64_t TypePointer;
    uint64_t Size;
    if(kot_Sys_GetInfoMemoryField(GP0, &TypePointer, &Size) == KSUCCESS){
        if(TypePointer == MemoryFieldTypeSendSpaceRO){            
            void* Buffer = malloc(Size);
            assert(kot_Sys_AcceptMemoryField(kot_Sys_GetProcess(), GP0, (void**)&Buffer) == KSUCCESS);
            ShellPrint(Shell, (void*)Buffer, Size);

            Status = KSUCCESS;
        }
    }
    
    kot_arguments_t arguments{
        .arg[0] = Status,            /* Status */
        .arg[1] = CallbackArg,      /* CallbackArg */
        .arg[2] = NULL,             /* GP0 */
        .arg[3] = NULL,             /* GP1 */
        .arg[4] = NULL,             /* GP2 */
        .arg[5] = NULL,             /* GP3 */
    };

    kot_Sys_ExecThread(Callback, &arguments, ExecutionTypeQueu, NULL);

    return KSUCCESS;
}

/* Direct access */
KResult Ioctlshell(kot_thread_t Callback, uint64_t CallbackArg, shell_t* Shell, uint64_t GP0, uint64_t GP1, uint64_t GP2){
    /* Args */
    unsigned long Request = static_cast<unsigned long>(GP0);
    void* Arg = reinterpret_cast<void*>(GP1);
    kot_process_t TargetDataProc = static_cast<kot_process_t>(GP2);
    
    int Result = 0;

    
    switch (Request){
        case TCGETS:
            SetArgData(Shell->Terminos, sizeof(struct termios), Arg, TargetDataProc);
            break;
        case TCSETS:
            GetArgData(Shell->Terminos, sizeof(struct termios), Arg, TargetDataProc);
            break;
        case TIOCGWINSZ:
            SetArgData(Shell->Winsize, sizeof(struct winsize), Arg, TargetDataProc);
            break;
        case TIOCSWINSZ:
            GetArgData(Shell->Winsize, sizeof(struct winsize), Arg, TargetDataProc);
            break;
        case TIOCGPTN:
            SetArgData(&Shell->TerminalID, sizeof(int), Arg, TargetDataProc);
            break;
        default:
            Result = -EINVAL;
            break;
    }
    
    kot_arguments_t arguments{
        .arg[0] = KSUCCESS,         /* Status */
        .arg[1] = CallbackArg,      /* CallbackArg */
        .arg[2] = (uint64_t)Result, /* Result */
        .arg[3] = NULL,             /* GP1 */
        .arg[4] = NULL,             /* GP2 */
        .arg[5] = NULL,             /* GP3 */
    };

    kot_Sys_ExecThread(Callback, &arguments, ExecutionTypeQueu, NULL);

    return KSUCCESS;
}