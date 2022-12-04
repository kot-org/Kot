#include "../picture.h"

#include <kot-graphics++/utils.h>

namespace Ui {

    TGA::TGA(char* path, Component* cpnt) {
        srv_system_callback_t* file = Srv_System_ReadFileInitrd(path, true);
        tgaHeader_t *data = (tgaHeader_t*) file->Data;

        if(!data) return;

        framebuffer_t* fb = (framebuffer_t*) malloc(sizeof(framebuffer_t));

        switch (data->imageType)
        {
            case 1:

                break;
            
            case 2:
            {
                uint64_t imageDataOffset = data->colorMapOrigin + data->colorMapLength;

                uint32_t pitch = data->width * (data->bpp/8);

                cpnt->getFramebuffer()->size = data->height * pitch;
                cpnt->getFramebuffer()->addr = calloc(cpnt->getFramebuffer()->size);
                cpnt->getFramebuffer()->pitch = pitch;
                cpnt->getFramebuffer()->width = data->width;
                cpnt->getFramebuffer()->height = data->height;
                cpnt->getFramebuffer()->bpp = data->bpp;
                cpnt->getFramebuffer()->btpp = data->bpp/8;

                cpnt->getStyle()->width = data->width;
                cpnt->getStyle()->height = data->height;

                for(int w = 0; w < data->width; w++) {
                    for(int h = 0; h < data->height; h++) {

                        putPixel(cpnt->getFramebuffer(), 0, 0, *(uint32_t*) ((uint64_t)data+imageDataOffset+w+pitch*h));
                        
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