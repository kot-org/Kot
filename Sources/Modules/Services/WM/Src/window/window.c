#include <core/main.h>
#include <draw/draw.h>
#include <window/window.h>

void CreateWindow(uint32_t x, uint32_t y, uint32_t w, uint32_t h)
{
    CreateRect(x, y, w, h, 0x1A1A1A);
}

void ClearScreen(void)
{
    memset(screenInfo->fb_addr, 0x00, screenInfo->fb_size);
}