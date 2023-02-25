#ifndef __KOT__GRAPHICS__UTILS__
#define __KOT__GRAPHICS__UTILS__

#include <kot/heap.h>
#include <kot/math.h>

#include <kot/uisd/srvs/graphics.h>

#if defined(__cplusplus)
extern "C" {
#endif

int8_t PixelExist(framebuffer_t* fb, uint32_t x, uint32_t y);
void PutPixel(framebuffer_t* fb, uint32_t x, uint32_t y, uint32_t color);
uint32_t GetPixel(framebuffer_t* fb, uint32_t x, uint32_t y);

void BlitFramebuffer(framebuffer_t* To, framebuffer_t* From, uint64_t PositionX, uint64_t PositionY);
void BlitFramebufferRadius(framebuffer_t* To, framebuffer_t* From, uint64_t PositionX, uint64_t PositionY, uint64_t BorderRadius);

void FillRect(framebuffer_t* fb, uint32_t x, uint32_t y, uint32_t Width, uint32_t Height, uint32_t color);
void DrawRect(framebuffer_t* fb, uint32_t x, uint32_t y, uint32_t Width, uint32_t Height, uint32_t color);
void DrawLine(framebuffer_t* fb, uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, uint32_t color);

#if defined(__cplusplus)
}
#endif

#endif