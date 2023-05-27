#include <core/main.h>

kot_uisd_graphics_t* SrvData;

KResult InitialiseServer(orbc* Orb){
    kot_process_t proc = kot_Sys_GetProcess();

    void* address = kot_GetFreeAlignedSpace(sizeof(kot_uisd_graphics_t));
    kot_key_mem_t key = NULL;
    kot_Sys_CreateMemoryField(proc, sizeof(kot_uisd_graphics_t), &address, &key, MemoryFieldTypeShareSpaceRO);

    SrvData = (kot_uisd_graphics_t*)address;
    memset(SrvData, 0, sizeof(kot_uisd_graphics_t)); // Clear data

    SrvData->ControllerHeader.IsReadWrite = false;
    SrvData->ControllerHeader.Version = ORB_Srv_Version;
    SrvData->ControllerHeader.VendorID = Kot_VendorID;
    SrvData->ControllerHeader.Type = ControllerTypeEnum_Graphics;
    SrvData->ControllerHeader.Process = kot_ShareProcessKey(proc);

    /* CreateWindow */
    kot_thread_t CreateWindowThread = NULL;
    kot_Sys_CreateThread(proc, (void*)&CreateWindowSrv, PriviledgeApp, (uint64_t)Orb, &CreateWindowThread);
    SrvData->CreateWindow = kot_MakeShareableThread(CreateWindowThread, PriviledgeApp);

    kot_uisd_callbackInfo_t* Callback = kot_CreateControllerUISD(ControllerTypeEnum_Graphics, key, true);
    KResult Status = Callback->Status;
    free(Callback);
    
    return Status;
}

KResult CreateWindowSrv(kot_thread_t Callback, uint64_t CallbackArg, kot_process_t Target, kot_event_t Event, uint64_t WindowType){
    orbc* Orb = (orbc*)kot_Sys_GetExternalDataThread();

    windowc* Window = NULL;
    
    if((Window = new windowc(Orb, WindowType, Event)) != NULL){
        kot_ShareDataWithArguments_t Data{
            .ParameterPosition = 0x3,
            .Data = Window->GetFramebuffer(),
            .Size = sizeof(kot_framebuffer_t),
        };

        Window->Target = Target;

        kot_thread_t GraphicsHandlerThread = NULL;
        kot_Sys_CreateThread(kot_Sys_GetProcess(), (void*)&WindowGraphicsHandler, PriviledgeApp, (uint64_t)Window, &GraphicsHandlerThread);
        kot_thread_t ShareableGraphicsHandlerThread = kot_MakeShareableThreadToProcess(GraphicsHandlerThread, Window->Target);
        
        kot_arguments_t Arguments{
            .arg[0] = KSUCCESS,                         /* Status */
            .arg[1] = CallbackArg,                      /* CallbackArg */
            .arg[2] = Window->GetFramebufferKey(),      /* FramebufferKey */
            .arg[3] = NULL,                             /* Framebuffer */
            .arg[4] = ShareableGraphicsHandlerThread,   /* ShareableGraphicsHandlerThread */
            .arg[5] = NULL,                             /* GP3 */
        };

        kot_Sys_ExecThread(Callback, &Arguments, ExecutionTypeQueu, &Data);
        kot_Sys_Close(KSUCCESS);
    }
    
    kot_arguments_t Arguments{
        .arg[0] = KFAIL,            /* Status */
        .arg[1] = CallbackArg,      /* CallbackArg */
        .arg[2] = NULL,             /* GP0 */
        .arg[3] = NULL,             /* GP1 */
        .arg[4] = NULL,             /* GP2 */
        .arg[5] = NULL,             /* GP3 */
    };

    kot_Sys_ExecThread(Callback, &Arguments, ExecutionTypeQueu, NULL);
    kot_Sys_Close(KSUCCESS);
}

static window_dispatch_t WindowDispatcher[Window_Function_Count] = { 
    [Window_Function_Close] = WindowClose,
    [Window_Function_Resize] = WindowResize,
    [Window_Function_ChangePostion] = WindowChangePostion,    
    [Window_Function_ChangeVisibility] = WindowChangeVisibility,    
};

KResult WindowGraphicsHandler(kot_thread_t Callback, uint64_t CallbackArg, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3){
    uint64_t Function = GP0;

    if(Function >= Window_Function_Count){
        kot_arguments_t Arguments{
            .arg[0] = KFAIL,            /* Status */
            .arg[1] = CallbackArg,      /* CallbackArg */
            .arg[2] = NULL,             /* GP0 */
            .arg[3] = NULL,             /* GP1 */
            .arg[4] = NULL,             /* GP2 */
            .arg[5] = NULL,             /* GP3 */
        };

        kot_Sys_ExecThread(Callback, &Arguments, ExecutionTypeQueu, NULL);
        kot_Sys_Close(KSUCCESS);
    }

    windowc* Window = (windowc*)kot_Sys_GetExternalDataThread();
    kot_Sys_Close(WindowDispatcher[Function](Callback, CallbackArg, Window, GP1, GP2, GP3));
}

KResult WindowClose(kot_thread_t Callback, uint64_t CallbackArg, windowc* Window, uint64_t GP0, uint64_t GP1, uint64_t GP2){
    KResult Status = Window->Close();
    kot_arguments_t Arguments{
        .arg[0] = Status,               /* Status */
        .arg[1] = CallbackArg,          /* CallbackArg */
        .arg[2] = NULL,                 /* GP0 */
        .arg[3] = NULL,                 /* GP1 */
        .arg[4] = NULL,                 /* GP2 */
        .arg[5] = NULL,                 /* GP3 */
    };

    kot_Sys_ExecThread(Callback, &Arguments, ExecutionTypeQueu, NULL);
    return KSUCCESS;    
}

KResult WindowResize(kot_thread_t Callback, uint64_t CallbackArg, windowc* Window, uint64_t GP0, uint64_t GP1, uint64_t GP2){
    KResult Status = Window->Resize(GP0, GP1);
    if(Status == KSUCCESS){
        kot_ShareDataWithArguments_t Data{
            .ParameterPosition = 0x3,
            .Data = Window->GetFramebuffer(),
            .Size = sizeof(kot_framebuffer_t),
        };

        kot_thread_t GraphicsHandlerThread = NULL;
        kot_Sys_CreateThread(kot_Sys_GetProcess(), (void*)&WindowGraphicsHandler, PriviledgeApp, (uint64_t)Window, &GraphicsHandlerThread);
        kot_thread_t ShareableGraphicsHandlerThread = kot_MakeShareableThreadToProcess(GraphicsHandlerThread, Window->Target);
        
        kot_arguments_t Arguments{
            .arg[0] = Status,                           /* Status */
            .arg[1] = CallbackArg,                      /* CallbackArg */
            .arg[2] = Window->GetFramebufferKey(),      /* FramebufferKey */
            .arg[3] = NULL,                             /* Framebuffer */
            .arg[4] = ShareableGraphicsHandlerThread,   /* ShareableGraphicsHandlerThread */
            .arg[5] = NULL,                             /* GP3 */
        };

        kot_Sys_ExecThread(Callback, &Arguments, ExecutionTypeQueu, &Data);
        return KSUCCESS;
    }

    kot_arguments_t Arguments{
        .arg[0] = Status,               /* Status */
        .arg[1] = CallbackArg,          /* CallbackArg */
        .arg[2] = NULL,                 /* GP0 */
        .arg[3] = NULL,                 /* GP1 */
        .arg[4] = NULL,                 /* GP2 */
        .arg[5] = NULL,                 /* GP3 */
    };

    kot_Sys_ExecThread(Callback, &Arguments, ExecutionTypeQueu, NULL);
    return KSUCCESS;    
}

KResult WindowChangePostion(kot_thread_t Callback, uint64_t CallbackArg, windowc* Window, uint64_t GP0, uint64_t GP1, uint64_t GP2){
    KResult Status = Window->Move(GP0, GP1);
    kot_arguments_t Arguments{
        .arg[0] = Status,               /* Status */
        .arg[1] = CallbackArg,          /* CallbackArg */
        .arg[2] = Window->XPosition,    /* XPosition */
        .arg[3] = Window->YPosition,    /* YPosition */
        .arg[4] = NULL,                 /* GP2 */
        .arg[5] = NULL,                 /* GP3 */
    };

    kot_Sys_ExecThread(Callback, &Arguments, ExecutionTypeQueu, NULL);
    return KSUCCESS;    
}

KResult WindowChangeVisibility(kot_thread_t Callback, uint64_t CallbackArg, windowc* Window, uint64_t GP0, uint64_t GP1, uint64_t GP2){
    bool IsVisible = Window->SetVisible(GP0);
    KResult Status = (IsVisible == GP0) ? KSUCCESS : KFAIL;

    kot_arguments_t Arguments{
        .arg[0] = Status,               /* Status */
        .arg[1] = CallbackArg,          /* CallbackArg */
        .arg[2] = IsVisible,            /* IsVisible */
        .arg[3] = NULL,                 /* GP1 */
        .arg[4] = NULL,                 /* GP2 */
        .arg[5] = NULL,                 /* GP3 */
    };

    kot_Sys_ExecThread(Callback, &Arguments, ExecutionTypeQueu, NULL);
    return KSUCCESS;    
}