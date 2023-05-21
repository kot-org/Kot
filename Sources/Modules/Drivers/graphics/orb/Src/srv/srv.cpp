#include <core/main.h>

uisd_graphics_t* SrvData;

KResult InitialiseServer(orbc* Orb){
    process_t proc = Sys_GetProcess();

    uintptr_t address = GetFreeAlignedSpace(sizeof(uisd_graphics_t));
    ksmem_t key = NULL;
    Sys_CreateMemoryField(proc, sizeof(uisd_graphics_t), &address, &key, MemoryFieldTypeShareSpaceRO);

    SrvData = (uisd_graphics_t*)address;
    memset(SrvData, 0, sizeof(uisd_graphics_t)); // Clear data

    SrvData->ControllerHeader.IsReadWrite = false;
    SrvData->ControllerHeader.Version = ORB_Srv_Version;
    SrvData->ControllerHeader.VendorID = Kot_VendorID;
    SrvData->ControllerHeader.Type = ControllerTypeEnum_Graphics;
    SrvData->ControllerHeader.Process = ShareProcessKey(proc);

    /* CreateWindow */
    thread_t CreateWindowThread = NULL;
    Sys_CreateThread(proc, (uintptr_t)&CreateWindowSrv, PriviledgeApp, (uint64_t)Orb, &CreateWindowThread);
    SrvData->CreateWindow = MakeShareableThread(CreateWindowThread, PriviledgeApp);

    uisd_callbackInfo_t* Callback = CreateControllerUISD(ControllerTypeEnum_Graphics, key, true);
    KResult Status = Callback->Status;
    free(Callback);
    
    return Status;
}

KResult CreateWindowSrv(thread_t Callback, uint64_t CallbackArg, process_t Target, kot_event_t Event, uint64_t WindowType){
    orbc* Orb = (orbc*)Sys_GetExternalDataThread();

    windowc* Window = NULL;
    
    if((Window = new windowc(Orb, WindowType, Event)) != NULL){
        ShareDataWithArguments_t Data{
            .ParameterPosition = 0x3,
            .Data = Window->GetFramebuffer(),
            .Size = sizeof(kot_framebuffer_t),
        };

        Window->Target = Target;

        thread_t GraphicsHandlerThread = NULL;
        Sys_CreateThread(Sys_GetProcess(), (uintptr_t)&WindowGraphicsHandler, PriviledgeApp, (uint64_t)Window, &GraphicsHandlerThread);
        thread_t ShareableGraphicsHandlerThread = MakeShareableThreadToProcess(GraphicsHandlerThread, Window->Target);
        
        arguments_t Arguments{
            .arg[0] = KSUCCESS,                         /* Status */
            .arg[1] = CallbackArg,                      /* CallbackArg */
            .arg[2] = Window->GetFramebufferKey(),      /* FramebufferKey */
            .arg[3] = NULL,                             /* Framebuffer */
            .arg[4] = ShareableGraphicsHandlerThread,   /* ShareableGraphicsHandlerThread */
            .arg[5] = NULL,                             /* GP3 */
        };

        Sys_ExecThread(Callback, &Arguments, ExecutionTypeQueu, &Data);
        Sys_Close(KSUCCESS);
    }
    
    arguments_t Arguments{
        .arg[0] = KFAIL,            /* Status */
        .arg[1] = CallbackArg,      /* CallbackArg */
        .arg[2] = NULL,             /* GP0 */
        .arg[3] = NULL,             /* GP1 */
        .arg[4] = NULL,             /* GP2 */
        .arg[5] = NULL,             /* GP3 */
    };

    Sys_ExecThread(Callback, &Arguments, ExecutionTypeQueu, NULL);
    Sys_Close(KSUCCESS);
}

static window_dispatch_t WindowDispatcher[Window_Function_Count] = { 
    [Window_Function_Close] = WindowClose,
    [Window_Function_Resize] = WindowResize,
    [Window_Function_ChangePostion] = WindowChangePostion,    
    [Window_Function_ChangeVisibility] = WindowChangeVisibility,    
};

KResult WindowGraphicsHandler(thread_t Callback, uint64_t CallbackArg, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3){
    uint64_t Function = GP0;

    if(Function >= Window_Function_Count){
        arguments_t Arguments{
            .arg[0] = KFAIL,            /* Status */
            .arg[1] = CallbackArg,      /* CallbackArg */
            .arg[2] = NULL,             /* GP0 */
            .arg[3] = NULL,             /* GP1 */
            .arg[4] = NULL,             /* GP2 */
            .arg[5] = NULL,             /* GP3 */
        };

        Sys_ExecThread(Callback, &Arguments, ExecutionTypeQueu, NULL);
        Sys_Close(KSUCCESS);
    }

    windowc* Window = (windowc*)Sys_GetExternalDataThread();
    Sys_Close(WindowDispatcher[Function](Callback, CallbackArg, Window, GP1, GP2, GP3));
}

KResult WindowClose(thread_t Callback, uint64_t CallbackArg, windowc* Window, uint64_t GP0, uint64_t GP1, uint64_t GP2){
    KResult Status = Window->Close();
    arguments_t Arguments{
        .arg[0] = Status,               /* Status */
        .arg[1] = CallbackArg,          /* CallbackArg */
        .arg[2] = NULL,                 /* GP0 */
        .arg[3] = NULL,                 /* GP1 */
        .arg[4] = NULL,                 /* GP2 */
        .arg[5] = NULL,                 /* GP3 */
    };

    Sys_ExecThread(Callback, &Arguments, ExecutionTypeQueu, NULL);
    return KSUCCESS;    
}

KResult WindowResize(thread_t Callback, uint64_t CallbackArg, windowc* Window, uint64_t GP0, uint64_t GP1, uint64_t GP2){
    KResult Status = Window->Resize(GP0, GP1);
    if(Status == KSUCCESS){
        ShareDataWithArguments_t Data{
            .ParameterPosition = 0x3,
            .Data = Window->GetFramebuffer(),
            .Size = sizeof(kot_framebuffer_t),
        };

        thread_t GraphicsHandlerThread = NULL;
        Sys_CreateThread(Sys_GetProcess(), (uintptr_t)&WindowGraphicsHandler, PriviledgeApp, (uint64_t)Window, &GraphicsHandlerThread);
        thread_t ShareableGraphicsHandlerThread = MakeShareableThreadToProcess(GraphicsHandlerThread, Window->Target);
        
        arguments_t Arguments{
            .arg[0] = Status,                           /* Status */
            .arg[1] = CallbackArg,                      /* CallbackArg */
            .arg[2] = Window->GetFramebufferKey(),      /* FramebufferKey */
            .arg[3] = NULL,                             /* Framebuffer */
            .arg[4] = ShareableGraphicsHandlerThread,   /* ShareableGraphicsHandlerThread */
            .arg[5] = NULL,                             /* GP3 */
        };

        Sys_ExecThread(Callback, &Arguments, ExecutionTypeQueu, &Data);
        return KSUCCESS;
    }

    arguments_t Arguments{
        .arg[0] = Status,               /* Status */
        .arg[1] = CallbackArg,          /* CallbackArg */
        .arg[2] = NULL,                 /* GP0 */
        .arg[3] = NULL,                 /* GP1 */
        .arg[4] = NULL,                 /* GP2 */
        .arg[5] = NULL,                 /* GP3 */
    };

    Sys_ExecThread(Callback, &Arguments, ExecutionTypeQueu, NULL);
    return KSUCCESS;    
}

KResult WindowChangePostion(thread_t Callback, uint64_t CallbackArg, windowc* Window, uint64_t GP0, uint64_t GP1, uint64_t GP2){
    KResult Status = Window->Move(GP0, GP1);
    arguments_t Arguments{
        .arg[0] = Status,               /* Status */
        .arg[1] = CallbackArg,          /* CallbackArg */
        .arg[2] = Window->XPosition,    /* XPosition */
        .arg[3] = Window->YPosition,    /* YPosition */
        .arg[4] = NULL,                 /* GP2 */
        .arg[5] = NULL,                 /* GP3 */
    };

    Sys_ExecThread(Callback, &Arguments, ExecutionTypeQueu, NULL);
    return KSUCCESS;    
}

KResult WindowChangeVisibility(thread_t Callback, uint64_t CallbackArg, windowc* Window, uint64_t GP0, uint64_t GP1, uint64_t GP2){
    bool IsVisible = Window->SetVisible(GP0);
    KResult Status = (IsVisible == GP0) ? KSUCCESS : KFAIL;

    arguments_t Arguments{
        .arg[0] = Status,               /* Status */
        .arg[1] = CallbackArg,          /* CallbackArg */
        .arg[2] = IsVisible,            /* IsVisible */
        .arg[3] = NULL,                 /* GP1 */
        .arg[4] = NULL,                 /* GP2 */
        .arg[5] = NULL,                 /* GP3 */
    };

    Sys_ExecThread(Callback, &Arguments, ExecutionTypeQueu, NULL);
    return KSUCCESS;    
}