#include <kot/modules/wm/draw/draw.h>

void PutPixel(window_t* self, uint32_t x, uint32_t y, uint32_t color)
{
    uint8_t *fb = self->fb_addr;
    uint64_t index = (x + (y * self->width)) * (self->bpp / 8);

    fb[index + 2] = (color >> 16) & 255;    // RED
    fb[index + 1] = (color >> 8) & 255;     // GREEN
    fb[index] = color & 255;                // BLUE
}

void DrawWindow(window_t* self, uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t color)
{
    uint32_t bdr_color = 0x000000;

    for (int i = 0; i < h; i++)
    {
        for (int j = 0; j < w; j++)
        {
            PutPixel(self, j + x, i + y, color);
        }
    }
}

void DrawRect(window_t* self, uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t color)
{
    for (int i = 0; i < h; i++)
    {
        for (int j = 0; j < w; j++)
        {
            PutPixel(self, j + x, i + y, color);
        }
    }
}