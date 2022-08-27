#ifndef _FONT_H_
#define _FONT_H_

#include <kot/types.h>

#if defined(__cplusplus)
extern "C" {
#endif

typedef struct {
    uintptr_t ctx;
} kfont_t;

typedef struct {
    uintptr_t address;
    uint64_t width;
    uint64_t height;
    uint64_t pitch;
} font_fb_t;

enum font_family{
    font_family_serif = 0,
    font_family_sans = 1,
    font_family_decor = 2,
    font_family_monospace = 3,
    font_family_hand = 4,
    font_family_byname = 5,
};


#define font_style_regular 1 << 0x0
#define font_style_bold 1 << 0x1
#define font_style_italic 1 << 0x2
#define font_style_usrdef2 1 << 0x3
#define font_style_underline 1 << 0x4
#define font_style_sthrough 1 << 0x5
#define font_style_a 1 << 0x6
#define font_style_noaa 1 << 0x7
#define font_style_nosmooth 1 << 0x8
#define font_style_nokern 1 << 0x9
#define font_style_nodefglyph 1 << 0xA
#define font_style_nocache 1 << 0xB
#define font_style_rtl 1 << 0xC
#define font_style_ab_size 1 << 0xD

kfont_t* LoadFont(uintptr_t data, enum font_family family, const char* name, int style, int size);
void FreeFont(kfont_t* font);

void PrintFont(kfont_t* font, char* str, font_fb_t* buffer, uint64_t x, uint64_t y);

#if defined(__cplusplus)
}
#endif

#endif