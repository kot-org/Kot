#include <srv/srv.h>

static shell_dispatch_t ShellDispatcher[File_Function_Count] = { 
    [File_Function_Close] = Closeshell,
    [File_Function_GetSize] = Getshellsize,
    [File_Function_Read] = Readshell,
    [File_Function_Write] = Writeshell,
};

process_t ProcessKey;

KResult SrvInitalize(){
    ProcessKey = ShareProcessKey(Sys_GetProcess());

    /* Enable shell as file */
    srv_storage_fs_server_functions_t FSServerFunctions;

    thread_t ThreadOpenfile;
    Sys_CreateThread(Sys_GetProcess(), (uintptr_t)&OpenShell, PriviledgeApp, NULL, &ThreadOpenfile);
    FSServerFunctions.Openfile = MakeShareableSpreadThreadToProcess(ThreadOpenfile, ((uisd_storage_t*)FindControllerUISD(ControllerTypeEnum_Storage))->ControllerHeader.Process);

    srv_storage_callback_t* StorageCallback = Srv_Storage_NewDev("tty", &FSServerFunctions, true);


    /* Load uisd controller */
    uintptr_t address = GetFreeAlignedSpace(sizeof(uisd_shell_t));
    ksmem_t key = NULL;
    Sys_CreateMemoryField(Sys_GetProcess(), sizeof(uisd_shell_t), &address, &key, MemoryFieldTypeShareSpaceRO);

    uisd_shell_t* ShellSrv = (uisd_shell_t*)address;
    ShellSrv->ControllerHeader.IsReadWrite = false;
    ShellSrv->ControllerHeader.Version = Shell_Srv_Version;
    ShellSrv->ControllerHeader.VendorID = Kot_VendorID;
    ShellSrv->ControllerHeader.Type = ControllerTypeEnum_Shell;

    ShellSrv->IsAvailableAsFile = (StorageCallback->Status == KSUCCESS);

    uisd_callbackInfo_t* info = CreateControllerUISD(ControllerTypeEnum_Shell, key, true);
    free(info);

    return KSUCCESS;
}

KResult OpenShell(thread_t Callback, uint64_t CallbackArg, char* Path, permissions_t Permissions, process_t Target){
    shell_t* Shell = NewShell(Target);
    
    srv_storage_fs_server_open_file_data_t SrvOpenFileData;
    thread_t DispatcherThread;

    Sys_CreateThread(Sys_GetProcess(), (uintptr_t)&ShellDispatch, PriviledgeDriver, (uint64_t)Shell, &DispatcherThread);

    SrvOpenFileData.Dispatcher = MakeShareableThreadToProcess(DispatcherThread, Target);

    SrvOpenFileData.FSDriverProc = ProcessKey;

    arguments_t arguments{
        .arg[0] = KSUCCESS,         /* Status */
        .arg[1] = CallbackArg,      /* CallbackArg */
        .arg[2] = NULL,             /* Data */
        .arg[3] = NULL,             /* GP1 */
        .arg[4] = NULL,             /* GP2 */
        .arg[5] = NULL,             /* GP3 */
    };
    
    ShareDataWithArguments_t ShareDataWithArguments{
        .Data = &SrvOpenFileData,
        .Size = sizeof(srv_storage_fs_server_open_file_data_t),
        .ParameterPosition = 0x2,
    };

    Sys_ExecThread(Callback, &arguments, ExecutionTypeQueu, &ShareDataWithArguments);
    Sys_Close(KSUCCESS);
}

KResult ShellDispatch(thread_t Callback, uint64_t CallbackArg, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3){
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

        Sys_ExecThread(Callback, &arguments, ExecutionTypeQueu, NULL);
        Sys_Close(KSUCCESS);
    }

    shell_t* Shell = (shell_t*)Sys_GetExternalDataThread();
    Sys_Close(ShellDispatcher[Function](Callback, CallbackArg, Shell, GP1, GP2, GP3)); // It'll call the callback in the function
}

/* Direct access */
KResult Closeshell(thread_t Callback, uint64_t CallbackArg, shell_t* Shell, uint64_t GP0, uint64_t GP1, uint64_t GP2){
    KResult Status = KSUCCESS;
    
    arguments_t arguments{
        .arg[0] = Status,           /* Status */
        .arg[1] = CallbackArg,      /* CallbackArg */
        .arg[2] = NULL,             /* GP0 */
        .arg[3] = NULL,             /* GP1 */
        .arg[4] = NULL,             /* GP2 */
        .arg[5] = NULL,             /* GP3 */
    };

    Sys_ExecThread(Callback, &arguments, ExecutionTypeQueu, NULL);
    
    if(Status == KSUCCESS){
        Sys_Exit(KSUCCESS);
    }

    return KSUCCESS;
}

/* Direct access */
KResult Getshellsize(thread_t Callback, uint64_t CallbackArg, shell_t* Shell, uint64_t GP0, uint64_t GP1, uint64_t GP2){
    arguments_t arguments{
        .arg[0] = KSUCCESS,             /* Status */
        .arg[1] = CallbackArg,          /* CallbackArg */
        .arg[2] = 0,                    /* FileSize */
        .arg[3] = NULL,                 /* GP1 */
        .arg[4] = NULL,                 /* GP2 */
        .arg[5] = NULL,                 /* GP3 */
    };
    Sys_ExecThread(Callback, &arguments, ExecutionTypeQueu, NULL);
    return KSUCCESS;
}

/* Direct access */
KResult Readshell(thread_t Callback, uint64_t CallbackArg, shell_t* Shell, uint64_t GP0, uint64_t GP1, uint64_t GP2){
    size64_t Size = GP1;
    ksmem_t BufferKey;

    KResult Status = KSUCCESS;

    arguments_t arguments{
        .arg[0] = Status,           /* Status */
        .arg[1] = CallbackArg,      /* CallbackArg */
        .arg[2] = NULL,             /* Key to buffer */
        .arg[3] = NULL,             /* GP1 */
        .arg[4] = NULL,             /* GP2 */
        .arg[5] = NULL,             /* GP3 */
    };

    if(Status == KSUCCESS){
        Sys_Keyhole_CloneModify(BufferKey, &arguments.arg[2], Shell->Target, KeyholeFlagPresent | KeyholeFlagCloneable | KeyholeFlagEditable, PriviledgeApp);
        Sys_ExecThread(Callback, &arguments, ExecutionTypeQueu, NULL);
    }else{
        Sys_ExecThread(Callback, &arguments, ExecutionTypeQueu, NULL);
    }

    return KSUCCESS;
}

/* Direct access */
KResult Writeshell(thread_t Callback, uint64_t CallbackArg, shell_t* Shell, uint64_t GP0, uint64_t GP1, uint64_t GP2){
    KResult Status = KFAIL;
    
    uint64_t TypePointer;
    uint64_t Size;
    if(Sys_GetInfoMemoryField(GP0, &TypePointer, &Size) == KSUCCESS){
        if(TypePointer == MemoryFieldTypeSendSpaceRO){            
            uintptr_t Buffer = malloc(Size);
            assert(Sys_AcceptMemoryField(Sys_GetProcess(), GP0, &Buffer) == KSUCCESS);
            ShellPrint(Shell, Buffer, Size);

            Status = KSUCCESS;
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

    Sys_ExecThread(Callback, &arguments, ExecutionTypeQueu, NULL);

    return KSUCCESS;
}