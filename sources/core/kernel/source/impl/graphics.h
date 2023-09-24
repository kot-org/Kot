#ifndef _IMPL_GRAPHICS_H
#define _IMPL_GRAPHICS_H 1

#include <stdint.h>
#include <stddef.h>

typedef struct{
    void* base;
    size_t size;

    uint16_t width;
    uint16_t height;
    uint16_t pitch;
    uint8_t bpp;
    uint8_t btpp;
} graphics_boot_fb_t;

void graphics_init(void);

graphics_boot_fb_t* graphics_get_boot_fb(void);

#endif