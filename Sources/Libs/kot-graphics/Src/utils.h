#ifndef __KOT__GRAPHICS__UTILS__
#define __KOT__GRAPHICS__UTILS__

#include <stdlib.h>
#include <kot/math.h>

#include <kot/uisd/srvs/graphics.h>

#if defined(__cplusplus)
extern "C" {
#endif

int8_t PixelExist(kot_framebuffer_t* fb, uint32_t x, uint32_t y);
void PutPixel(kot_framebuffer_t* fb, uint32_t x, uint32_t y, uint32_t color);
uint32_t GetPixel(kot_framebuffer_t* fb, uint32_t x, uint32_t y);

void BlitFramebuffer(kot_framebuffer_t* To, kot_framebuffer_t* From, uint64_t PositionX, uint64_t PositionY);
void BlitFramebufferRadius(kot_framebuffer_t* To, kot_framebuffer_t* From, uint64_t PositionX, uint64_t PositionY, uint64_t BorderRadius);

void FillRect(kot_framebuffer_t* fb, uint32_t x, uint32_t y, uint32_t Width, uint32_t Height, uint32_t color);
void DrawRect(kot_framebuffer_t* fb, uint32_t x, uint32_t y, uint32_t Width, uint32_t Height, uint32_t color);
void DrawLine(kot_framebuffer_t* fb, uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, uint32_t color);

#if defined(__cplusplus)
}
#endif

#endif