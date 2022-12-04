#include "../picture.h"

#include <kot-graphics++/utils.h>

namespace Ui {

    TGA::TGA(char* path, Component* cpnt) {
        srv_system_callback_t* file = Srv_System_ReadFileInitrd(path, true);
        tgaHeader_t *data = (tgaHeader_t*) file->Data;

        if(!data) return;
        
        uint8_t btpp = data->bpp/8;
        uint32_t pitch = data->width * btpp;

        uint64_t Bpp = 32; 
        uint64_t Btpp = Bpp / 4; 
        uint64_t Height = data->height; // Todo user arguments
        uint64_t Width = data->width; // Todo user arguments
        uint64_t Pitch = Width * Btpp;

        cpnt->getFramebuffer()->size = Pitch * (Height + 1);
        cpnt->getFramebuffer()->addr = calloc(cpnt->getFramebuffer()->size);
        cpnt->getFramebuffer()->pitch = Pitch;
        cpnt->getFramebuffer()->width = data->width;
        cpnt->getFramebuffer()->height = data->height;
        cpnt->getFramebuffer()->bpp = Bpp;
        cpnt->getFramebuffer()->btpp = Bpp / 8;

        cpnt->getStyle()->width = data->width;
        cpnt->getStyle()->height = data->height;

        switch (data->imageType)
        {
            case 1:

                break;
            
            case 2:
            {
                uint64_t imageDataOffset = data->colorMapOrigin + data->colorMapLength + 18;

                for(uint64_t y = 0; y < Height; y++) {
                    for(uint64_t x = 0; x < Width; x++) {
                        uint8_t R = *(uint8_t*) ((uint64_t)data+imageDataOffset+x*btpp+pitch*y + 2);
                        uint8_t G = *(uint8_t*) ((uint64_t)data+imageDataOffset+x*btpp+pitch*y + 1);
                        uint8_t B = *(uint8_t*) ((uint64_t)data+imageDataOffset+x*btpp+pitch*y + 0);
                        uint32_t Pixel = B | (G << 8) | (R << 16);
                        putPixel(cpnt->getFramebuffer(), cpnt->getStyle()->x+x, cpnt->getStyle()->y+Height-y, Pixel);
                    }
                }

                break;
            }
            case 9:
            
                break;
            
            case 10:
            
                break;
        }
    }
    
}