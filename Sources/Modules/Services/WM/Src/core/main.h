#pragma once

#include <kot/sys.h>
#include <kot/modules/wm/draw/draw.h>

#define SCREEN_BKG_COLOR 0x1A1A1A
#define WINDOW_BKG_COLOR 0x222222

typedef struct
{
    uint64_t identifier;
    uint64_t NAME;
    uint64_t framebuffer_addr;
    uint16_t framebuffer_width;
    uint16_t framebuffer_height;
    uint16_t framebuffer_pitch;
    uint16_t framebuffer_bpp;
    uint8_t  memory_model;
    uint8_t  red_mask_size;
    uint8_t  red_mask_shift;
    uint8_t  green_mask_size;
    uint8_t  green_mask_shift;
    uint8_t  blue_mask_size;
    uint8_t  blue_mask_shift;
    uint8_t  unused;
} framebuffer_t;

extern window_t* screenInfo;