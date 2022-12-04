#include "../picture.h"

#include <kot-graphics++/utils.h>

namespace Ui {

    TGA::TGA(char* path, Component* cpnt) {
        srv_system_callback_t* file = Srv_System_ReadFileInitrd(path, true);
        tgaHeader_t *data = (tgaHeader_t*) file->Data;

        if(!data) return;

        uint8_t btpp = data->bpp/8;
        uint32_t pitch = data->width * (btpp);

        cpnt->getFramebuffer()->size = data->height * pitch;
        cpnt->getFramebuffer()->addr = calloc(cpnt->getFramebuffer()->size);
        cpnt->getFramebuffer()->pitch = pitch;
        cpnt->getFramebuffer()->width = data->width;
        cpnt->getFramebuffer()->height = data->height;
        cpnt->getFramebuffer()->bpp = data->bpp;
        cpnt->getFramebuffer()->btpp = btpp;

        cpnt->getStyle()->width = data->width;
        cpnt->getStyle()->height = data->height;

        switch (data->imageType)
        {
            case 1:

                break;
            
            case 2:
            {
                uint64_t imageDataOffset = data->colorMapOrigin + data->colorMapLength;

                for(int w = 0; w < data->width; w++) {
                    for(int h = 0; h < data->height; h++) {

                        putPixel(cpnt->getFramebuffer(), cpnt->getStyle()->x+w, cpnt->getStyle()->y+h, *(uint32_t*) ((uint64_t)data+imageDataOffset+w*btpp+pitch*h));
                        
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