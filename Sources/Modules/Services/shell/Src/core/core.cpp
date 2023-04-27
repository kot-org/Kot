#include <core/core.h>

#include <kot-graphics/orb.h>
#include <kot-graphics/context.h>


shell_t* NewShell(process_t Target){
    shell_t* Shell = (shell_t*)malloc(sizeof(shell_t));

    Shell->Target = Target;
    
    Shell->Wid = CreateWindow(NULL, Window_Type_Default);
    ResizeWindow(Shell->Wid, 500, 500);
    ChangeVisibilityWindow(Shell->Wid, true);

    Shell->Framebuffer = &Shell->Wid->Framebuffer;
    Shell->Backbuffer = (framebuffer_t*)malloc(sizeof(framebuffer_t));

    memcpy(Shell->Backbuffer, Shell->Framebuffer, sizeof(framebuffer_t));
    Shell->Backbuffer->Buffer = calloc(Shell->Framebuffer->Size);

    // Load font
    file_t* FontFile = fopen("d0:default-font.sfn", "r");
    fseek(FontFile, 0, SEEK_END);
    size64_t Size = ftell(FontFile);
    uintptr_t Buffer = malloc(Size);
    fseek(FontFile, 0, SEEK_SET);
    fread(Buffer, Size, 1, FontFile);
    Shell->Font = (kfont_t*)LoadFont(Buffer);
    free(Buffer);
    fclose(FontFile);

    font_fb_t FontFB;
    FontFB.Address = Shell->Backbuffer->Buffer;
    FontFB.Width = Shell->Backbuffer->Width;
    FontFB.Height = Shell->Backbuffer->Height;
    FontFB.Pitch = Shell->Backbuffer->Pitch;
    LoadPen(Shell->Font, &FontFB, 0, 0, 16, 0, 0xFFFFFFFF);

    ChangeVisibilityWindow(Shell->Wid, true);

    return Shell;
}

void ShellPrint(shell_t* Shell, uintptr_t Buffer, size64_t Size){
    DrawFontSize(Shell->Font, (char*)Buffer, Size);
    BlitFramebuffer(Shell->Framebuffer, Shell->Backbuffer, 0, 0);
}