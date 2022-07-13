#include "core/main.h"

void put_pixel(uint32_t x, uint32_t y, int r, int g, int b)
{
    uint8_t *fb = fb_addr;
    uint64_t index = (x + (y * fb_width)) * (fb_bpp / 8);

    fb[index + 2] = r;      // RED
    fb[index + 1] = g;      // GREEN
    fb[index] = b;          // BLUE
}

void create_rect(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t color)
{
    for (int i = 0; i < h; i++)
    {
        for (int j = 0; j < w; j++)
        {
            put_pixel(j + x, i + y, (color >> 16) & 255, (color >> 8) & 255, color & 255);
        }
    }
}

void create_Gradientrect(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t fromColor, uint32_t toColor)
{
    uint8_t bf = fromColor & 255, gf = (fromColor >> 8) & 255, rf = (fromColor >> 16) & 255;
    uint8_t bt = toColor & 255, gt = (toColor >> 8) & 255, rt = (toColor >> 16) & 255;

    struct color
    {
        long double r;
        long double g;
        long double b;
    };

    struct color pxColor;
    pxColor.r = rf;
    pxColor.g = gf;
    pxColor.b = bf;

    struct color jumpPx;
    jumpPx.r = (long double)(rf - rt) / w;
    jumpPx.g = (long double)(gf - gt) / w;
    jumpPx.b = (long double)(bf - bt) / w;

    for (int j = 0; j < w; j++)
    {
        pxColor.r -= jumpPx.r;
        pxColor.g -= jumpPx.g;
        pxColor.b -= jumpPx.b;
        for (int i = 0; i < h; i++)
        {
            put_pixel(j + x, i + y, pxColor.r, pxColor.g, pxColor.b);
        }
    }
}