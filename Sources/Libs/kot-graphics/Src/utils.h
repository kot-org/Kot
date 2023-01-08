#ifndef __KOT__GRAPHICS__UTILS__
#define __KOT__GRAPHICS__UTILS__

#include <kot/heap.h>
#include <kot/math.h>

#include <kot/uisd/srvs/graphics.h>

#if defined(__cplusplus)
extern "C" {
#endif

int8_t pixelExist(framebuffer_t* fb, uint32_t x, uint32_t y);
void PutPixel(framebuffer_t* fb, uint32_t x, uint32_t y, uint32_t colour);
uint32_t GetPixel(framebuffer_t* fb, uint32_t x, uint32_t y);

void BlitFramebuffer(framebuffer_t* to, framebuffer_t* from, uint32_t x, uint32_t y);
void BlitFramebufferRadius(framebuffer_t* to, framebuffer_t* from, uint32_t x, uint32_t y, uint16_t borderRadius);

void FillRect(framebuffer_t* fb, uint32_t x, uint32_t y, uint32_t Width, uint32_t Height, uint32_t colour);
void DrawRect(framebuffer_t* fb, uint32_t x, uint32_t y, uint32_t Width, uint32_t Height, uint32_t colour);
void DrawLine(framebuffer_t* fb, uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, uint32_t colour);

#if defined(__cplusplus)
}
#endif

#endif