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
    uintptr_t address;
    uint64_t width;
    uint64_t height;
    uint64_t pitch;
} font_fb_t;

kfont_t* LoadFont(uintptr_t data);
void FreeFont(kfont_t* font);

void LoadPen(kfont_t* font, font_fb_t* buffer, uint64_t x, uint64_t y, uint8_t size, uint16_t style, uint32_t color);

void DrawFontGetPos(kfont_t* font, char* str, uint64_t* x, uint64_t* y);
void DrawFont(kfont_t* font, char* str);

#if defined(__cplusplus)
}
#endif

#endif