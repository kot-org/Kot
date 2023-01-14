#include <kot/uisd/srvs/graphics.h>

thread_t srv_graphics_callback_thread = NULL;
uisd_graphics_t* GraphicsData = NULL;
process_t ShareProcessGraphics = NULL;

void Srv_Graphics_Initialize(){
    GraphicsData = (uisd_graphics_t*)FindControllerUISD(ControllerTypeEnum_Graphics);
    process_t Process = Sys_GetProcess();
    ShareProcessGraphics = ShareProcessKey(Process);

    thread_t GraphicsThreadKeyCallback = NULL;
    Sys_Createthread(Process, &Srv_Graphics_Callback, PriviledgeDriver, NULL, &GraphicsThreadKeyCallback);
    srv_graphics_callback_thread = MakeShareableThreadToProcess(GraphicsThreadKeyCallback, GraphicsData->ControllerHeader.Process);
}

void Srv_Graphics_Callback(KResult Status, struct srv_graphics_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3){
    Callback->Status = Callback->Handler(Status, Callback, GP0, GP1, GP2, GP3);

    if(Callback->IsAwait){
        Sys_Unpause(Callback->Self);
    }
        
    Sys_Close(KSUCCESS);
}

/* CreateWindow */
KResult Srv_Graphics_CreateWindow_Callback(KResult Status, struct srv_graphics_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3){
    if(Status == KSUCCESS){
        window_t* Window = (window_t*)Callback->Data;
        Window->GraphicsHandler = GP2;
        memcpy(&Window->Framebuffer, GP1, sizeof(framebuffer_t));
        Window->BufferKey = GP0;
        Sys_AcceptMemoryField(Sys_GetProcess(), Window->BufferKey, &Window->Framebuffer.Buffer);
    }
    return Status;
}

struct srv_graphics_callback_t* Srv_Graphics_CreateWindow(uintptr_t EventHandler, uint64_t WindowType, bool IsAwait){
    if(!srv_graphics_callback_thread) Srv_Graphics_Initialize();

    window_t* Window = (window_t*)malloc(sizeof(window_t));

    struct srv_graphics_callback_t* callback = (struct srv_graphics_callback_t*)malloc(sizeof(struct srv_graphics_callback_t));
    callback->Self = Sys_Getthread();
    callback->Data = Window;
    callback->Size = sizeof(window_t);
    callback->IsAwait = IsAwait;
    callback->Status = KBUSY;
    callback->Handler = &Srv_Graphics_CreateWindow_Callback;

    Sys_Createthread(Sys_GetProcess(), EventHandler, PriviledgeApp, Window, &Window->EventHandler);

    struct arguments_t parameters;
    parameters.arg[0] = srv_graphics_callback_thread;
    parameters.arg[1] = callback;
    parameters.arg[2] = ShareProcessGraphics;
    parameters.arg[3] = MakeShareableThreadToProcess(Window->EventHandler, GraphicsData->ControllerHeader.Process);
    parameters.arg[4] = WindowType;
    
    KResult Status = Sys_Execthread(GraphicsData->CreateWindow, &parameters, ExecutionTypeQueu, NULL);
    if(Status == KSUCCESS && IsAwait){
        Sys_Pause(false);
    }
    return callback;
}

/* CloseWindow */
KResult Srv_Graphics_CloseWindow_Callback(KResult Status, struct srv_graphics_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3){
    if(Status == KSUCCESS){
        window_t* Window = (window_t*)Callback->Data;
        Sys_CloseMemoryField(Sys_GetProcess(), Window->BufferKey, Window->Framebuffer.Buffer);
        free(Window);
    }
    return Status;
}

struct srv_graphics_callback_t* Srv_Graphics_CloseWindow(window_t* Window, bool IsAwait){
    if(!srv_graphics_callback_thread) Srv_Graphics_Initialize();

    struct srv_graphics_callback_t* callback = (struct srv_graphics_callback_t*)malloc(sizeof(struct srv_graphics_callback_t));
    callback->Self = Sys_Getthread();
    callback->Data = Window;
    callback->Size = NULL;
    callback->IsAwait = IsAwait;
    callback->Status = KBUSY;
    callback->Handler = &Srv_Graphics_CloseWindow_Callback; 

    struct arguments_t parameters;
    parameters.arg[0] = srv_graphics_callback_thread;
    parameters.arg[1] = callback;
    parameters.arg[2] = Window_Function_Close;
    

    KResult Status = Sys_Execthread(Window->GraphicsHandler, &parameters, ExecutionTypeQueu, NULL);
    if(Status == KSUCCESS && IsAwait){
        Sys_Pause(false);
    }
    return callback;
}

/* ResizeWindow */
KResult Srv_Graphics_ResizeWindow_Callback(KResult Status, struct srv_graphics_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3){
    if(Status == KSUCCESS){
        window_t* Window = (window_t*)Callback->Data;
        memcpy(&Window->Framebuffer, GP1, sizeof(framebuffer_t));
        Window->BufferKey = GP0;
        Sys_AcceptMemoryField(Sys_GetProcess(), Window->BufferKey, &Window->Framebuffer.Buffer);
    }
    return Status;
}

struct srv_graphics_callback_t* Srv_Graphics_ResizeWindow(window_t* Window, int64_t Width, int64_t Height, bool IsAwait){
    if(!srv_graphics_callback_thread) Srv_Graphics_Initialize();

    struct srv_graphics_callback_t* callback = (struct srv_graphics_callback_t*)malloc(sizeof(struct srv_graphics_callback_t));
    callback->Self = Sys_Getthread();
    callback->Data = Window;
    callback->Size = NULL;
    callback->IsAwait = IsAwait;
    callback->Status = KBUSY;
    callback->Handler = &Srv_Graphics_ResizeWindow_Callback; 

    struct arguments_t parameters;
    parameters.arg[0] = srv_graphics_callback_thread;
    parameters.arg[1] = callback;
    parameters.arg[2] = Window_Function_Resize;
    parameters.arg[3] = Width;
    parameters.arg[4] = Height;
    

    KResult Status = Sys_Execthread(Window->GraphicsHandler, &parameters, ExecutionTypeQueu, NULL);
    if(Status == KSUCCESS && IsAwait){
        Sys_Pause(false);
    }
    return callback;
}

/* ChangePostionWindow */
KResult Srv_Graphics_ChangePostionWindow_Callback(KResult Status, struct srv_graphics_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3){
    return Status;
}

struct srv_graphics_callback_t* Srv_Graphics_ChangePostionWindow(window_t* Window, uint64_t XPosition, uint64_t YPosition, bool IsAwait){
    if(!srv_graphics_callback_thread) Srv_Graphics_Initialize();

    struct srv_graphics_callback_t* callback = (struct srv_graphics_callback_t*)malloc(sizeof(struct srv_graphics_callback_t));
    callback->Self = Sys_Getthread();
    callback->Data = Window;
    callback->Size = NULL;
    callback->IsAwait = IsAwait;
    callback->Status = KBUSY;
    callback->Handler = &Srv_Graphics_ChangePostionWindow_Callback; 

    struct arguments_t parameters;
    parameters.arg[0] = srv_graphics_callback_thread;
    parameters.arg[1] = callback;
    parameters.arg[2] = Window_Function_ChangePostion;
    parameters.arg[3] = XPosition;
    parameters.arg[4] = YPosition;
    

    KResult Status = Sys_Execthread(Window->GraphicsHandler, &parameters, ExecutionTypeQueu, NULL);
    if(Status == KSUCCESS && IsAwait){
        Sys_Pause(false);
    }
    return callback;
}

/* ChangeState */
KResult Srv_Graphics_ChangeVisibility_Callback(KResult Status, struct srv_graphics_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3){
    if(Status == KSUCCESS){
        window_t* Window = (window_t*)Callback->Data;
        Window->IsVisible = GP0;
    }
    return Status;
}

struct srv_graphics_callback_t* Srv_Graphics_ChangeVisibility(window_t* Window, bool IsVisible, bool IsAwait){
    if(!srv_graphics_callback_thread) Srv_Graphics_Initialize();

    struct srv_graphics_callback_t* callback = (struct srv_graphics_callback_t*)malloc(sizeof(struct srv_graphics_callback_t));
    callback->Self = Sys_Getthread();
    callback->Data = Window;
    callback->Size = NULL;
    callback->IsAwait = IsAwait;
    callback->Status = KBUSY;
    callback->Handler = &Srv_Graphics_ChangeVisibility_Callback; 

    struct arguments_t parameters;
    parameters.arg[0] = srv_graphics_callback_thread;
    parameters.arg[1] = callback;
    parameters.arg[2] = Window_Function_ChangeVisibility;
    parameters.arg[3] = IsVisible;
    

    KResult Status = Sys_Execthread(Window->GraphicsHandler, &parameters, ExecutionTypeQueu, NULL);
    if(Status == KSUCCESS && IsAwait){
        Sys_Pause(false);
    }
    return callback;
}