#include <core/core.h>

#include <kot-graphics/orb.h>
#include <kot-graphics/context.h>

shell_t* NewShell(){
    shell_t* Shell = (shell_t*)malloc(sizeof(shell_t));

    window_t* wid = CreateWindow(NULL, Window_Type_Default);
    ResizeWindow(wid, 300, 300);
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
        fillRect(Shell->Backbuffer, 0, 0, 10, 16, 0xffffff);
        blitFramebuffer(Shell->Framebuffer, Shell->Backbuffer, 0, 0);
        SleepFromTick(&TimerState, 500);
        fillRect(Shell->Backbuffer, 0, 0, 10, 16, 0x0);
        blitFramebuffer(Shell->Framebuffer, Shell->Backbuffer, 0, 0);
        SleepFromTick(&TimerState, 500);
    }
}