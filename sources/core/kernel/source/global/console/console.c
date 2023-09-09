#include <boot/limine.h>
#include <global/console.h>

#include "vgafont.h"

#include <lib/log.h>
#include <lib/bitmap.h>
#include <lib/string.h>
#include <lib/assert.h>

static uint32_t bg_color;
static uint32_t fg_color;

static uint32_t* fb;
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
    fb = (uint32_t*) lifb->address;
    fb_width = lifb->width;
    fb_height = lifb->height;
    fb_pitch = lifb->pitch;
    fb_bpp = lifb->bpp;
    fb_btpp = fb_bpp/8;

    assert(fb_bpp == 32);

    cx_index = 0;
    cy_index = 0;
}

void console_set_bg_color(uint32_t bg) {
    bg_color = bg;
}
void console_set_fg_color(uint32_t fg) {
    fg_color = fg;
}

static void console_putpixel(uint16_t x, uint16_t y, uint32_t color) {
    fb[y*fb_width+x] = color;
}

void console_putchar(char c) {
    if(c == '\n') {
        goto new_line;
    }

    uint8_t* glyph = &vgafont[c*16]; // 16 because 1 char = 16 bytes

    // char pixel-position
    uint16_t cx_ppos = cx_index * VGAFONT_WIDTH;
    uint16_t cy_ppos = cy_index * VGAFONT_HEIGHT;

    for(uint16_t y = 0; y < VGAFONT_HEIGHT; y++) {
        for(uint16_t x = 0; x < VGAFONT_WIDTH; x++) {
            
            if(BIT_GET(glyph[y], VGAFONT_WIDTH-x) == BITSET) {
                console_putpixel(cx_ppos+x, cy_ppos, fg_color); 
            } else {
                console_putpixel(cx_ppos+x, cy_ppos, bg_color); 
            }
        }
        cy_ppos++;
    }
    cx_index++;

    if(cx_ppos >= fb_width) {
        new_line:
            cx_index = 0;
            cy_index++;
    }
}

void console_print(const char* str) {
    for(uint32_t i = 0; i < strlen(str); i++) {
        console_putchar(str[i]);
    }
}