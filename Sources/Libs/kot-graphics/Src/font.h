#ifndef _FONT_H_
#define _FONT_H_

#include <kot/types.h>
#include <kot-graphics/utils.h>

#if defined(__cplusplus)
extern "C" {
#endif

typedef void* kfont_t;
typedef uint64_t font_style_t;
typedef signed long kfont_pos_t;
typedef signed long kfont_dot_t;
typedef unsigned long kfont_glyph_t;

kfont_t LoadFont(void* data, size_t size);
KResult FreeFont(kfont_t opaque);

KResult LoadPen(kfont_t opaque, kot_framebuffer_t* fb, kfont_pos_t x, kfont_pos_t y, kfont_dot_t size, font_style_t style, color_t color);

KResult SetPenFb(kfont_t opaque, kot_framebuffer_t* fb);
kot_framebuffer_t* GetPenFb(kfont_t opaque);

KResult SetPenPosX(kfont_t opaque, kfont_pos_t x);
kfont_pos_t GetPenPosX(kfont_t opaque);

KResult SetPenPosY(kfont_t opaque, kfont_pos_t y);
kfont_pos_t GetPenPosY(kfont_t opaque);

KResult SetPenSize(kfont_t opaque, kfont_dot_t size);
kfont_dot_t GetPenSize(kfont_t opaque);

KResult SetPenStyle(kfont_t opaque, font_style_t style);
font_style_t GetPenStyle(kfont_t opaque);

KResult SetPenColor(kfont_t opaque, color_t color);
color_t GetPenColor(kfont_t opaque);

kfont_pos_t GetLineHeight(kfont_t opaque);

kfont_pos_t GetGlyphWidth(kfont_t opaque);
kfont_pos_t GetGlyphBearingY(kfont_t opaque);

KResult DrawFont(kfont_t opaque, char* str);
KResult DrawFontN(kfont_t opaque, char* str, size_t len);

KResult GetTextboxInfo(kfont_t opaque, char* str, kfont_pos_t* width, kfont_pos_t* height, kfont_pos_t* x, kfont_pos_t* y);
KResult GetTextboxInfoN(kfont_t opaque, char* str, size_t len, kfont_pos_t* width, kfont_pos_t* height, kfont_pos_t* x, kfont_pos_t* y);

KResult DrawGlyph(kfont_t opaque, kfont_glyph_t glyph, kfont_dot_t width, kfont_dot_t height);

#if defined(__cplusplus)
}
#endif

#endif