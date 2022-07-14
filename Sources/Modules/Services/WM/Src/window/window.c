#include <core/main.h>
#include <draw/draw.h>
#include <window/window.h>

WindowInfo *windowInfo;

void CreateWindow(uint32_t x, uint32_t y, uint32_t w, uint32_t h)
{
    windowInfo = (WindowInfo *) malloc(sizeof(WindowInfo));

    DrawWindow(x, y, w, h, 0x222222);
    
    windowInfo->visibility = true;
    windowInfo->width = w;
    windowInfo->height = h;
}

void ClearScreen(void)
{
    memset(screenInfo->fb_addr, 0x1A1A1A, screenInfo->fb_size);
}