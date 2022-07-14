#pragma once

#include <kot/sys.h>

typedef struct
{
    uint64_t fb_addr;
    size_t fb_size;
    bool visibility;
    uint32_t width;
    uint32_t height;
    uint32_t pitch;
    uint32_t bpp;
    uint32_t xPos;
    uint32_t yPos;
} WindowInfo_t;

void PutPixel(WindowInfo_t* self, uint32_t x, uint32_t y, uint32_t color);
void DrawWindowMenuBar(WindowInfo_t* self, uint32_t x, uint32_t y, uint32_t w, uint32_t h);
void DrawWindow(WindowInfo_t* self, uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t color);
void DrawRect(WindowInfo_t* self, uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t color);