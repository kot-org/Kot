#ifndef _FONT_H_
#define _FONT_H_

#include <kot/types.h>

#if defined(__cplusplus)
extern "C" {
#endif

typedef struct {
    uintptr_t Context;
    uintptr_t FrameBuffer;
    bool IsSet;
} kfont_t;

typedef struct {
    uintptr_t address;
    uint64_t width;
    uint64_t height;
    uint64_t pitch;
} font_fb_t;

kfont_t* LoadFont(uintptr_t data);
void FreeFont(kfont_t* font);

void SetFont(kfont_t* font, font_fb_t* buffer, uint64_t x, uint64_t y, uint32_t foregroundcolor, uint32_t backgroundcolor, uint8_t fontSize, int style);
void DrawFont(kfont_t* font, char* str);

#if defined(__cplusplus)
}
#endif

#endif