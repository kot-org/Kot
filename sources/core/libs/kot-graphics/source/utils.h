#ifndef KOT_GRAPHICS_UTILS
#define KOT_GRAPHICS_UTILS

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

typedef struct {
    void* buffer;
    size_t size;
    uint64_t pitch;
    uint64_t width;
    uint64_t height;
    uint64_t bpp;
    uint64_t btpp;
} kframebuffer_t;

typedef uint32_t color_t;

void put_pixel(kframebuffer_t* fb, uint32_t x, uint32_t y, uint32_t color);
uint32_t get_pixel(kframebuffer_t* fb, uint32_t x, uint32_t y);
uint32_t blend_colors(uint32_t color1, uint32_t color2, float factor);
uint32_t blend_alpha(uint32_t color, uint8_t factor);

#endif // KOT_GRAPHICS_UTILS