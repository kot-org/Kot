#pragma once

#include <kot/sys.h>

#include <kot-graphics/utils.h>

namespace Ui {

    /* TGA */
    typedef struct {
        uint8_t IDLength;
        uint8_t ColorMapType;
        uint8_t ImageType;
        uint16_t ColorMapOrigin, ColorMapLength;
        uint8_t ColorMapEntSz;
        uint16_t x, y;
        uint16_t Width, Height;
        uint8_t Bpp;
        uint8_t ImageDescriptor;
    } __attribute__((__packed__)) TGAHeader_t;

    typedef enum {
        COLORMAP      = 1,
        RGB           = 2,
        COLORMAP_RLE  = 9,
        RGB_RLE       = 10
    } TGAType;

    typedef struct {
        uint32_t* Pixels;
        uint16_t Width;
        uint16_t Height;
    } TGA_t;

    TGA_t* TGARead(TGAHeader_t* Buffer);

    void TGADraw(framebuffer_t* Fb, TGA_t* Image);
    TGA_t* TGAResize(TGA_t* Image, uint16_t NewWidth, uint16_t NewHeight);

    /* ... */
}