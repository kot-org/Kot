#pragma once

#include <kot-ui++/component.h>

#include <kot/uisd/srvs/storage.h>

namespace Ui {

    class TGA
    {
        private:
            typedef struct {
                uint8_t idLength;
                uint8_t colorMapType;
                uint8_t imageType;
                uint16_t colorMapOrigin, colorMapLength;
                uint8_t colorMapEntSz;
                uint16_t x, y;
                uint16_t Width, Height;
                uint8_t Bpp;
                uint8_t imageDescriptor;
            } __attribute__((__packed__)) tgaHeader_t;

        public:
            TGA(file_t* file);
    };

}