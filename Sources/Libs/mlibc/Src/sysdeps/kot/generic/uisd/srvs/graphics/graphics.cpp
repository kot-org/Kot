#include <kot/uisd/srvs/graphics.h>
#include <stdlib.h>
#include <string.h>

extern "C" {

kot_thread_t kot_srv_graphics_callback_thread = NULL;
kot_uisd_graphics_t* kot_GraphicsData = NULL;
kot_process_t kot_ShareProcessGraphics = NULL;

void Srv_Graphics_Initialize(){
    kot_GraphicsData = (kot_uisd_graphics_t*)kot_FindControllerUISD(ControllerTypeEnum_Graphics);
    kot_process_t Process = kot_Sys_GetProcess();
    kot_ShareProcessGraphics = kot_ShareProcessKey(Process);

    kot_thread_t GraphicsThreadKeyCallback = NULL;
    kot_Sys_CreateThread(Process, (void*)&kot_Srv_Graphics_Callback, PriviledgeDriver, NULL, &GraphicsThreadKeyCallback);
    kot_srv_graphics_callback_thread = kot_MakeShareableThreadToProcess(GraphicsThreadKeyCallback, kot_GraphicsData->ControllerHeader.Process);
}

void kot_Srv_Graphics_Callback(KResult Status, struct kot_srv_graphics_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3){
    Callback->Status = Callback->Handler(Status, Callback, GP0, GP1, GP2, GP3);

    if(Callback->IsAwait){
        kot_Sys_Unpause(Callback->Self);
    }else{
        free(Callback);
    }
        
    kot_Sys_Close(KSUCCESS);
}

/* CreateWindow */
KResult Srv_Graphics_Createwindowcallback(KResult Status, struct kot_srv_graphics_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3){
    if(Status == KSUCCESS){
        kot_window_t* Window = (kot_window_t*)Callback->Data;
        Window->GraphicsHandler = GP2;
        memcpy((void*)&Window->Framebuffer, (void*)GP1, sizeof(kot_framebuffer_t));
        Window->BufferKey = GP0;
        kot_Sys_AcceptMemoryField(kot_Sys_GetProcess(), Window->BufferKey, &Window->Framebuffer.Buffer);
    }
    return Status;
}

struct kot_srv_graphics_callback_t* kot_Srv_Graphics_CreateWindow(kot_event_t Event, uint64_t WindowType, bool IsAwait){
    if(!kot_srv_graphics_callback_thread) Srv_Graphics_Initialize();

    kot_window_t* Window = (kot_window_t*)malloc(sizeof(kot_window_t));

    struct kot_srv_graphics_callback_t* callback = (struct kot_srv_graphics_callback_t*)malloc(sizeof(struct kot_srv_graphics_callback_t));
    callback->Self = kot_Sys_GetThread();
    callback->Data = (uint64_t)Window;
    callback->Size = sizeof(kot_window_t);
    callback->IsAwait = IsAwait;
    callback->Status = KBUSY;
    callback->Handler = &Srv_Graphics_Createwindowcallback;

    Window->Event = Event;

    struct kot_arguments_t parameters;
    parameters.arg[0] = kot_srv_graphics_callback_thread;
    parameters.arg[1] = (uint64_t)callback;
    parameters.arg[2] = kot_ShareProcessGraphics;
    parameters.arg[4] = WindowType;
    
    kot_Sys_Keyhole_CloneModify(Window->Event, &parameters.arg[3], kot_GraphicsData->ControllerHeader.Process, KeyholeFlagPresent | KeyholeFlagDataTypeEventIsTriggerable, PriviledgeApp);

    KResult Status = kot_Sys_ExecThread(kot_GraphicsData->CreateWindow, &parameters, ExecutionTypeQueu, NULL);
    if(Status == KSUCCESS && IsAwait){
        kot_Sys_Pause(false);
    }
    return callback;
}

/* CloseWindow */
KResult Srv_Graphics_Closewindowcallback(KResult Status, struct kot_srv_graphics_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3){
    if(Status == KSUCCESS){
        kot_window_t* Window = (kot_window_t*)Callback->Data;
        kot_Sys_CloseMemoryField(kot_Sys_GetProcess(), Window->BufferKey, Window->Framebuffer.Buffer);
        free(Window);
    }
    return Status;
}

struct kot_srv_graphics_callback_t* kot_Srv_Graphics_CloseWindow(kot_window_t* Window, bool IsAwait){
    if(!kot_srv_graphics_callback_thread) Srv_Graphics_Initialize();

    struct kot_srv_graphics_callback_t* callback = (struct kot_srv_graphics_callback_t*)malloc(sizeof(struct kot_srv_graphics_callback_t));
    callback->Self = kot_Sys_GetThread();
    callback->Data = (uint64_t)Window;
    callback->Size = NULL;
    callback->IsAwait = IsAwait;
    callback->Status = KBUSY;
    callback->Handler = &Srv_Graphics_Closewindowcallback; 

    struct kot_arguments_t parameters;
    parameters.arg[0] = kot_srv_graphics_callback_thread;
    parameters.arg[1] = (uint64_t)callback;
    parameters.arg[2] = Window_Function_Close;
    

    KResult Status = kot_Sys_ExecThread(Window->GraphicsHandler, &parameters, ExecutionTypeQueu, NULL);
    if(Status == KSUCCESS && IsAwait){
        kot_Sys_Pause(false);
    }
    return callback;
}

/* ResizeWindow */
KResult Srv_Graphics_Resizewindowcallback(KResult Status, struct kot_srv_graphics_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3){
    if(Status == KSUCCESS){
        kot_window_t* Window = (kot_window_t*)Callback->Data;
        memcpy((void*)&Window->Framebuffer, (void*)GP1, sizeof(kot_framebuffer_t));
        Window->BufferKey = GP0;
        kot_Sys_AcceptMemoryField(kot_Sys_GetProcess(), Window->BufferKey, &Window->Framebuffer.Buffer);
    }
    return Status;
}

struct kot_srv_graphics_callback_t* kot_Srv_Graphics_ResizeWindow(kot_window_t* Window, int64_t Width, int64_t Height, bool IsAwait){
    if(!kot_srv_graphics_callback_thread) Srv_Graphics_Initialize();

    struct kot_srv_graphics_callback_t* callback = (struct kot_srv_graphics_callback_t*)malloc(sizeof(struct kot_srv_graphics_callback_t));
    callback->Self = kot_Sys_GetThread();
    callback->Data = (uint64_t)Window;
    callback->Size = NULL;
    callback->IsAwait = IsAwait;
    callback->Status = KBUSY;
    callback->Handler = &Srv_Graphics_Resizewindowcallback; 

    struct kot_arguments_t parameters;
    parameters.arg[0] = (uint64_t)kot_srv_graphics_callback_thread;
    parameters.arg[1] = (uint64_t)callback;
    parameters.arg[2] = Window_Function_Resize;
    parameters.arg[3] = Width;
    parameters.arg[4] = Height;
    

    KResult Status = kot_Sys_ExecThread(Window->GraphicsHandler, &parameters, ExecutionTypeQueu, NULL);
    if(Status == KSUCCESS && IsAwait){
        kot_Sys_Pause(false);
    }
    return callback;
}

/* ChangePostionWindow */
KResult Srv_Graphics_ChangePostionwindowcallback(KResult Status, struct kot_srv_graphics_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3){
    if(Status == KSUCCESS){
        kot_window_t* Window = (kot_window_t*)Callback->Data;
        Window->Position.x = GP0;
        Window->Position.y = GP1;
    }
    return Status;
}

struct kot_srv_graphics_callback_t* kot_Srv_Graphics_ChangePostionWindow(kot_window_t* Window, uint64_t XPosition, uint64_t YPosition, bool IsAwait){
    if(!kot_srv_graphics_callback_thread) Srv_Graphics_Initialize();

    struct kot_srv_graphics_callback_t* callback = (struct kot_srv_graphics_callback_t*)malloc(sizeof(struct kot_srv_graphics_callback_t));
    callback->Self = kot_Sys_GetThread();
    callback->Data = (uint64_t)Window;
    callback->Size = NULL;
    callback->IsAwait = IsAwait;
    callback->Status = KBUSY;
    callback->Handler = &Srv_Graphics_ChangePostionwindowcallback; 

    struct kot_arguments_t parameters;
    parameters.arg[0] = kot_srv_graphics_callback_thread;
    parameters.arg[1] = (uint64_t)callback;
    parameters.arg[2] = Window_Function_ChangePostion;
    parameters.arg[3] = XPosition;
    parameters.arg[4] = YPosition;
    

    KResult Status = kot_Sys_ExecThread(Window->GraphicsHandler, &parameters, ExecutionTypeQueu, NULL);
    if(Status == KSUCCESS && IsAwait){
        kot_Sys_Pause(false);
    }
    return callback;
}

/* ChangeState */
KResult Srv_Graphics_ChangeVisibility_Callback(KResult Status, struct kot_srv_graphics_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3){
    if(Status == KSUCCESS){
        kot_window_t* Window = (kot_window_t*)Callback->Data;
        Window->IsVisible = GP0;
    }
    return Status;
}

struct kot_srv_graphics_callback_t* kot_Srv_Graphics_ChangeVisibility(kot_window_t* Window, bool IsVisible, bool IsAwait){
    if(!kot_srv_graphics_callback_thread) Srv_Graphics_Initialize();

    struct kot_srv_graphics_callback_t* callback = (struct kot_srv_graphics_callback_t*)malloc(sizeof(struct kot_srv_graphics_callback_t));
    callback->Self = kot_Sys_GetThread();
    callback->Data = (uint64_t)Window;
    callback->Size = NULL;
    callback->IsAwait = IsAwait;
    callback->Status = KBUSY;
    callback->Handler = &Srv_Graphics_ChangeVisibility_Callback; 

    struct kot_arguments_t parameters;
    parameters.arg[0] = kot_srv_graphics_callback_thread;
    parameters.arg[1] = (uint64_t)callback;
    parameters.arg[2] = Window_Function_ChangeVisibility;
    parameters.arg[3] = IsVisible;
    

    KResult Status = kot_Sys_ExecThread(Window->GraphicsHandler, &parameters, ExecutionTypeQueu, NULL);
    if(Status == KSUCCESS && IsAwait){
        kot_Sys_Pause(false);
    }
    return callback;
}

}