#include <kot/modules/wm/draw/draw.h>

#include <core/main.h>
#include <window/window.h>

window_t* CreateWindow(uint32_t x, uint32_t y, uint32_t w, uint32_t h)
{
    window_t* windowInfo = (window_t *) malloc(sizeof(window_t));

    DrawWindow(screenInfo, x, y, w, h, WINDOW_BKG_COLOR);
    
    windowInfo->visibility = true;
    windowInfo->width = w;
    windowInfo->height = h;
    windowInfo->xPos = x;
    windowInfo->yPos = y;

    return windowInfo;
}

void CloseWindow(window_t* windowInfo)
{
    DrawWindow(screenInfo, windowInfo->xPos, windowInfo->yPos, windowInfo->width, windowInfo->height, SCREEN_BKG_COLOR);
    
    free(&windowInfo);
}

void MoveWindow(window_t* windowInfo, uint32_t x, uint32_t y)
{
    DrawWindow(screenInfo, windowInfo->xPos, windowInfo->yPos, windowInfo->width, windowInfo->height, SCREEN_BKG_COLOR);
    DrawWindow(screenInfo, x, y, windowInfo->width, windowInfo->height, WINDOW_BKG_COLOR);

    windowInfo->xPos = x;
    windowInfo->yPos = y;
}

void ClearScreen(void)
{
    memset(screenInfo->fb_addr, SCREEN_BKG_COLOR, screenInfo->fb_size);
}
