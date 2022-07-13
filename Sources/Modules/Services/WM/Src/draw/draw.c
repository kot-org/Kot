#include <core/main.h>

void PutPixel(uint32_t x, uint32_t y, uint32_t color)
{
    uint8_t *fb = screenInfo->fb_addr;
    uint64_t index = (x + (y * screenInfo->width)) * (screenInfo->bpp / 8);

    fb[index + 2] = (color >> 16) & 255;
    fb[index + 1] = (color >> 8) & 255;
    fb[index] = color & 255;
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

void CreateGradientRect(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t fromColor, uint32_t toColor)
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

        uint32_t color = (((uint32_t) pxColor.r << 16) & 255) + (((uint32_t) pxColor.g << 8) & 255) + ((uint32_t) pxColor.b & 255);

        for (int i = 0; i < h; i++)
        {
            PutPixel(j + x, i + y, color);
        }
    }
}