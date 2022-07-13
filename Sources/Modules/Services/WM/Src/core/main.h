#pragma once

#include <kot/sys.h>

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

extern uint64_t fb_addr;
extern size_t fb_size;
extern uint16_t fb_width;
extern uint16_t fb_height;
extern uint16_t fb_pitch;
extern uint16_t fb_bpp;