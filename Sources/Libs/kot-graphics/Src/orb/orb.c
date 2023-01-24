#include <kot-graphics/orb.h>

window_t* CreateWindow(uintptr_t EventHandler, uint64_t WindowType){
    struct srv_graphics_callback_t* Callback = Srv_Graphics_CreateWindow(EventHandler, WindowType, true);
    window_t* Window = Callback->Data;
    free(Callback);
    return Window;
}

window_t* CloseWindow(window_t* Window){
    struct srv_graphics_callback_t* Callback = Srv_Graphics_CloseWindow(Window, true);
    free(Callback);
    return Window;
}

KResult ResizeWindow(window_t* Window, int64_t Width, int64_t Height){
    struct srv_graphics_callback_t* Callback = Srv_Graphics_ResizeWindow(Window, Width, Height, true);
    KResult Status = Callback->Status;
    free(Callback);
    return Status;
}

KResult WindowChangePosition(window_t* Window, uint64_t XPosition, uint64_t YPosition){
    struct srv_graphics_callback_t* Callback = Srv_Graphics_ChangePostionWindow(Window, XPosition, YPosition, true);
    KResult Status = Callback->Status;
    free(Callback);
    return Status;
}

KResult ChangeVisibilityWindow(window_t* Window, bool IsVisible){
    struct srv_graphics_callback_t* Callback = Srv_Graphics_ChangeVisibility(Window, IsVisible, true);
    KResult Status = Callback->Status;
    free(Callback);
    return Status;
}