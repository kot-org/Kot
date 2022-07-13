#include <core/main.h>

void PutPixel(uint32_t x, uint32_t y, uint32_t color)
{
    uint8_t *fb = screenInfo->fb_addr;
    uint64_t index = (x + (y * screenInfo->width)) * (screenInfo->bpp / 8);

    fb[index + 2] = (color >> 16) & 255;    // RED
    fb[index + 1] = (color >> 8) & 255;     // GREEN
    fb[index] = color & 255;                // BLUE
}

void CreateRect(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t color)
{
    for (int i = 0; i < h; i++)
    {
        for (int j = 0; j < w; j++)
        {
            PutPixel(j + x, i + y, color);
        }
    }
}