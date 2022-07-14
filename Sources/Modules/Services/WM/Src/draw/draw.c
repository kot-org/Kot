#include <core/main.h>

void PutPixel(uint32_t x, uint32_t y, uint32_t color)
{
    uint8_t *fb = screenInfo->fb_addr;
    uint64_t index = (x + (y * screenInfo->width)) * (screenInfo->bpp / 8);

    fb[index + 2] = (color >> 16) & 255;    // RED
    fb[index + 1] = (color >> 8) & 255;     // GREEN
    fb[index] = color & 255;                // BLUE
}

void DrawWindow(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t color)
{
    uint32_t bdr_color = 0x000000;
    uint32_t wmb_width = w, wmb_height = 40, wmb_color = 0x333333;

    /* Menu bar */
    for (int i = 0; i < wmb_height; i++)
    {
        for (int j = 0; j < wmb_width; j++)
        {
            PutPixel(j + x, i + y, wmb_color);
        }
    }

    for (int i = 0; i < h; i++)
    {
        for (int j = 0; j < w; j++)
        {
            PutPixel(j + x, i + y + wmb_height, color);
        }
    }
}

void DrawRect(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t color)
{
    for (int i = 0; i < h; i++)
    {
        for (int j = 0; j < w; j++)
        {
            PutPixel(j + x, i + y, color);
        }
    }
}