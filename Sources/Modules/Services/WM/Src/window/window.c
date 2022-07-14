#include <kot/modules/wm/draw/draw.h>

#include <core/main.h>
#include <window/window.h>

WindowInfo_t* CreateWindow(uint32_t x, uint32_t y, uint32_t w, uint32_t h)
{
    WindowInfo_t* windowInfo = (WindowInfo_t *) malloc(sizeof(WindowInfo_t));

    DrawWindow(screenInfo, x, y, w, h, 0x222222);
    
    windowInfo->visibility = true;
    windowInfo->width = w;
    windowInfo->height = h;
    windowInfo->xPos = x;
    windowInfo->yPos = y;

    return windowInfo;
}

void MoveWindow(WindowInfo_t* windowInfo, uint32_t x, uint32_t y)
{
    DrawWindow(screenInfo, windowInfo->xPos, windowInfo->yPos, windowInfo->width, windowInfo->height, 0x1A1A1A);
    DrawWindow(screenInfo, x, y, windowInfo->width, windowInfo->height, 0x222222);
}

void ClearScreen(void)
{
    memset(screenInfo->fb_addr, 0x1A1A1A, screenInfo->fb_size);
}