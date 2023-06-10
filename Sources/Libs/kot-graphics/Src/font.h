#ifndef _FONT_H_
#define _FONT_H_

#include <kot/types.h>
#include <kot-graphics/utils.h>

#if defined(__cplusplus)
extern "C" {
#endif

typedef void* kfont_t;

kfont_t* LoadFont(void* data, size_t size);
void FreeFont(kfont_t* font);

void LoadPen(kfont_t* font, kot_framebuffer_t* fb, int64_t x, int64_t y, int16_t size, int32_t style, int64_t color);
void EditPen(kfont_t* font, kot_framebuffer_t* fb, int64_t x, int64_t y, int16_t size, int32_t style, int64_t color);

void DrawFont(kfont_t* font, char* str);
void GetTextboxInfo(kfont_t* font, char* str, int64_t* width, int64_t* height, int64_t* x, int64_t* y);

#if defined(__cplusplus)
}
#endif

#endif