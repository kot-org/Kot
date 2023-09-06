#include <boot/limine.h>
#include <global/console.h>

#include "vgafont.h"

#include <lib/log.h>
#include <lib/bitmap.h>
#include <lib/string.h>

static uint32_t bg_color;
static uint32_t fg_color;

static uintptr_t* fb;
static uint16_t fb_width, fb_height;
static uint16_t fb_pitch;
static uint8_t fb_bpp;
static uint8_t fb_btpp;

// char position
static uint16_t cx_index;
static uint16_t cy_index;

static volatile struct limine_framebuffer_request framebuffer_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST,
    .revision = 0
};

void console_init(void) {
    bg_color = DEFAULT_BG_COLOR;
    fg_color = DEFAULT_FG_COLOR;
    
    struct limine_framebuffer* lifb = framebuffer_request.response->framebuffers[0];
    fb = (uintptr_t*) lifb->address;
    fb_width = lifb->width;
    fb_height = lifb->height;
    fb_pitch = lifb->pitch;
    fb_bpp = lifb->bpp;
    fb_btpp = fb_bpp/8;

    cx_index = 0;
    cy_index = 0;
}

static void console_putpixel(uint16_t x, uint16_t y, uint32_t color) {
    fb[y*fb_width+x] = color;
}

void console_putchar(char c) {
    uint8_t* glyph = &vgafont[c*16]; // 16 because 1 char = 16 bytes

    // char pixel-position
    uint16_t cx_ppos = cx_index * 8;
    uint16_t cy_ppos = cy_index * 8;

    for(uint16_t y = 0; y < 16; y++) {
        for(uint16_t x = 8; x > 0; x--) {
            if(BIT_GET(glyph[y], x))
                console_putpixel(cx_ppos+x, cy_ppos, DEFAULT_FG_COLOR);
            /* else
                console_putpixel(cx_ppos+x, cy_ppos, DEFAULT_BG_COLOR); */
        }
        cy_ppos++;
    }

    cx_index++;
}

void console_print(const char* str) {
    for(uint32_t i = 0; i < strlen(str); i++) {
        console_putchar(str[i]);
    }
}