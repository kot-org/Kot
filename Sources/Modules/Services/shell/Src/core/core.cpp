#include <core/core.h>

#include <kot-graphics/orb.h>
#include <kot-graphics/context.h>

shell_t* NewShell(){
    shell_t* Shell = (shell_t*)malloc(sizeof(shell_t));

    window_t* wid = CreateWindow(NULL, Window_Type_DockRight);
    ResizeWindow(wid, 50, 50);

    Shell->Framebuffer = &wid->Framebuffer;
    Shell->Backbuffer = (framebuffer_t*)malloc(sizeof(framebuffer_t));

    memcpy(Shell->Backbuffer, Shell->Framebuffer, sizeof(framebuffer_t));
    Shell->Backbuffer->Buffer = calloc(Shell->Framebuffer->Size);

    Shell->FontFB = (font_fb_t*)malloc(sizeof(font_fb_t));
    Shell->FontFB->address = Shell->Backbuffer->Buffer;
    Shell->FontFB->Width = Shell->Backbuffer->Width;
    Shell->FontFB->Height = Shell->Backbuffer->Height;
    Shell->FontFB->Pitch = Shell->Backbuffer->Pitch;
    LoadPen(Font, Shell->FontFB, 0, 0, 16, 0, 0xFFFFFFFF);

    ChangeVisibilityWindow(wid, true);

    uint64_t TimerState;
    GetActualTick(&TimerState);
    
    while (true){
        FillRect(Shell->Backbuffer, 0, 0, 10, 16, 0xffffff);
        BlitFramebuffer(Shell->Framebuffer, Shell->Backbuffer, 0, 0);
        SleepFromTick(&TimerState, 500);
        FillRect(Shell->Backbuffer, 0, 0, 10, 16, 0x0);
        BlitFramebuffer(Shell->Framebuffer, Shell->Backbuffer, 0, 0);
        SleepFromTick(&TimerState, 500);
    }
}