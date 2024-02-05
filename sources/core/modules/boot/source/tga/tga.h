#ifndef _BOOT_TGA_H
#define _BOOT_TGA_H

#include <stdint.h>
#include <stdbool.h>

#include <impl/graphics.h>

typedef struct {
    uint8_t idLength;
    uint8_t colorMapType;
    uint8_t imageType;
    uint16_t colorMapOrigin, colorMapLength;
    uint8_t colorMapEntSz;
    uint16_t x, y;
    uint16_t width, height;
    uint8_t bpp;
    uint8_t imageDescriptor;
} __attribute__((__packed__)) tga_header_t;

typedef enum {
    TYPE_COLORMAP      = 1,
    TYPE_RGB           = 2,
    TYPE_COLORMAP_RLE  = 9,
    TYPE_RGB_RLE       = 10
} tga_type_t;

typedef struct {
    uint32_t* pixels;
    uint16_t width;
    uint16_t height;
    uint16_t x;
    uint16_t y;
} tga_t;

tga_t* tga_read(tga_header_t* buffer);

void tga_draw(graphics_boot_fb_t* fb, tga_t* image);
tga_t* tga_resize(tga_t* image, uint16_t new_width, uint16_t new_height, bool keep_ratio);
tga_t* tga_crop(tga_t* image, uint16_t width, uint16_t height, uint16_t x, uint16_t y);

#endif