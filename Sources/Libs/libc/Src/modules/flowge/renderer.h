#pragma once

#include <kot/math.h>
#include <kot/sys.h>
#include <kot/heap.h>

typedef struct {
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

typedef struct {
    uint64_t fb_addr;
    size_t fb_size;
    uint32_t width;
    uint32_t height;
    uint32_t pitch;
    uint32_t bpp;
    uint32_t bps;
    uint32_t btpp;
} screen_t;

void clear(screen_t* screen);

void putPixel(screen_t* screen, uint32_t x, uint32_t y, uint32_t colour);
uint32_t getPixel(screen_t* screen, uint32_t x, uint32_t y);
bool pixelExist(screen_t* screen, uint32_t x, uint32_t y);

void drawLine(screen_t* screen, uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, uint32_t colour);

void fillRect(screen_t* screen, uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t colour);
void drawRect(screen_t* screen, uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t colour);
