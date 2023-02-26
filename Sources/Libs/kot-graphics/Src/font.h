#ifndef _FONT_H_
#define _FONT_H_

#include <kot/types.h>

#if defined(__cplusplus)
extern "C" {
#endif

typedef struct {
    uintptr_t FontContext;
    bool IsPen;
    uintptr_t PenContext;
} kfont_t;

typedef struct {
    uintptr_t Address;
    uint64_t Width;
    uint64_t Height;
    uint64_t Pitch;
} font_fb_t;

kfont_t* LoadFont(uintptr_t data);
void FreeFont(kfont_t* font);

void LoadPen(kfont_t* font, font_fb_t* buffer, int64_t x, int64_t y, int16_t size, int32_t style, int64_t color);
void EditPen(kfont_t* font, font_fb_t* buffer, int64_t x, int64_t y, int16_t size, int32_t style, int64_t color);

void DrawFont(kfont_t* font, char* str);
void GetTextboxInfo(kfont_t* font, char* str, int64_t* width, int64_t* height, int64_t* x, int64_t* y);

#if defined(__cplusplus)
}
#endif

#endif