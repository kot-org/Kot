#pragma once

#include <main/main.h>
#include <kot-graphics/utils.h>

struct shell_t{
    framebuffer_t* Framebuffer;
    framebuffer_t* Backbuffer;
    font_fb_t* FontFB;
};

struct shell_t* NewShell();