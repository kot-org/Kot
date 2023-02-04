#pragma once

#include <kot/sys.h>

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

    uint32_t* TGARead(TGAHeader_t* image, uint16_t Width, uint16_t Height);

    /* ... */
}