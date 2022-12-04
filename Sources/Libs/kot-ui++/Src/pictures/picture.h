#pragma once

#include <kot-ui++/component.h>

#include <kot/uisd/srvs/system.h>

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
                uint16_t width, height;
                uint8_t bpp;
                uint8_t imageDescriptor;
            } __attribute__((packed)) tgaHeader_t;

        public:
            TGA(char* path, Component* cpnt);
    };

}