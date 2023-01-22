#ifndef _SRV_GRAPHICS_H
#define _SRV_GRAPHICS_H 1

#include <kot/sys.h>
#include <kot/uisd.h>
#include <kot/math.h>
#include <kot/types.h>

#if defined(__cplusplus)
extern "C" {
#endif

#define Window_Function_Close               0x0
#define Window_Function_Resize              0x1
#define Window_Function_ChangePostion       0x2
#define Window_Function_ChangeVisibility    0x3

#define Window_Function_Count               0x4

#define Window_Type_Default                 0x0
#define Window_Type_Background              0x1
#define Window_Type_Foreground              0x2
#define Window_Type_DockTop                 0x3 // Dock 
#define Window_Type_DockBottom              0x4 // Dock
#define Window_Type_DockLeft                0x5 // Dock
#define Window_Type_DockRight               0x6 // Dock

#define Window_Max_Size                     -1

typedef KResult (*GraphicsCallbackHandler)(KResult Status, struct srv_graphics_callback* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3);

enum Window_Event{
    Winwow_Event_Focus = 0x0,
    Winwow_Event_Mouse = 0x1,
    Winwow_Event_Keyboard = 0x2,
};

struct srv_graphics_callback_t{
    thread_t Self;
    uint64_t Data;
    size64_t Size;
    bool IsAwait;
    KResult Status;
    GraphicsCallbackHandler Handler;
};

typedef struct {
    uintptr_t Buffer;
    size64_t Size;
    uint64_t Pitch;
    uint64_t Width;
    uint64_t Height;
    uint64_t Bpp;
    uint64_t Btpp;
} framebuffer_t;

typedef struct {
    framebuffer_t Framebuffer;
    ksmem_t BufferKey;
    thread_t GraphicsHandler;
    bool IsVisible;
    thread_t EventHandler;
    point_t Position;
    event_t Event;
} window_t;

void Srv_Graphics_Callback(KResult Status, struct srv_graphics_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3);

struct srv_graphics_callback_t* Srv_Graphics_CreateWindow(uintptr_t EventHandler, uint64_t WindowType, bool IsAwait);

struct srv_graphics_callback_t* Srv_Graphics_CloseWindow(window_t* Window, bool IsAwait);
struct srv_graphics_callback_t* Srv_Graphics_ResizeWindow(window_t* Window, int64_t Width, int64_t Height, bool IsAwait);
struct srv_graphics_callback_t* Srv_Graphics_ChangePostionWindow(window_t* Window, uint64_t XPosition, uint64_t YPosition, bool IsAwait);
struct srv_graphics_callback_t* Srv_Graphics_ChangeVisibility(window_t* Window, bool IsVisible, bool IsAwait);

#if defined(__cplusplus)
}
#endif


#endif