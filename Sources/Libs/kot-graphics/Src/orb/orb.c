#include <kot-graphics/orb.h>

kot_window_t* CreateWindow(kot_event_t Event, uint64_t WindowType){
    struct kot_srv_graphics_callback_t* Callback = kot_Srv_Graphics_CreateWindow(Event, WindowType, true);
    kot_window_t* Window = Callback->Data;
    free(Callback);
    return Window;
}

kot_window_t* CloseWindow(kot_window_t* Window){
    struct kot_srv_graphics_callback_t* Callback = kot_Srv_Graphics_CloseWindow(Window, true);
    free(Callback);
    return Window;
}

KResult ResizeWindow(kot_window_t* Window, int64_t Width, int64_t Height){
    struct kot_srv_graphics_callback_t* Callback = kot_Srv_Graphics_ResizeWindow(Window, Width, Height, true);
    KResult Status = Callback->Status;
    free(Callback);
    return Status;
}

KResult WindowChangePosition(kot_window_t* Window, uint64_t XPosition, uint64_t YPosition){
    struct kot_srv_graphics_callback_t* Callback = kot_Srv_Graphics_ChangePostionWindow(Window, XPosition, YPosition, true);
    KResult Status = Callback->Status;
    free(Callback);
    return Status;
}

KResult ChangeVisibilityWindow(kot_window_t* Window, bool IsVisible){
    struct kot_srv_graphics_callback_t* Callback = kot_Srv_Graphics_ChangeVisibility(Window, IsVisible, true);
    KResult Status = Callback->Status;
    free(Callback);
    return Status;
}