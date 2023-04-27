#pragma once

#include <main/main.h>
#include <kot-graphics/utils.h>

struct shell_t{
    framebuffer_t* Framebuffer;
    framebuffer_t* Backbuffer;
    kfont_t* Font;

    window_t* Wid;

    process_t Target;
};

struct shell_t* NewShell(process_t Target);

void ShellPrint(shell_t* Shell, uintptr_t Buffer, size64_t Size);