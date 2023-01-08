#include "../picture.h"

#include <kot-graphics/utils.h>

namespace Ui {

    TGA::TGA(char* path, Component* cpnt) {
        srv_system_callback_t* file = Srv_System_ReadFileInitrd(path, true);
        tgaHeader_t *image = (tgaHeader_t*) file->Data;

        if(!image) return;
        if(image->Width <= 0 || image->Height <= 0) { free(image); return; }

        uintptr_t imageDataOffset = (uintptr_t) (image->colorMapOrigin + image->colorMapLength + 18),
            imagePixelData = (uintptr_t) ((uint64_t)image + (uint64_t)imageDataOffset);

        uint8_t Bpp = image->Bpp;
        uint8_t Btpp = image->Bpp/8;
        uint32_t Width = image->Width,
                 cpntWidth = cpnt->getStyle()->Width;
        uint32_t Height = image->Height,
                 cpntHeight = cpnt->getStyle()->Height;
        uint32_t Pitch = Width * Btpp;
        bool isReversed = !(image->imageDescriptor & (1 << 5));

        /* Todo: dessiner l'image dans la fonction draw */

        if(cpntWidth == NULL)
            cpntWidth = Width;
        else
            Width = cpntWidth;
        
        if(cpntHeight == NULL)
            cpntHeight = Height;
        else
            Height = cpntHeight;
        
        cpnt->updateFramebuffer(Width, Height);

        switch (image->imageType)
        {
            case 1:
                Printlog("tga type 1");
                break;
            
            case 2:
            {
                for(uint64_t y = 0; y < Height; y++) {
                    uint32_t YPosition = ((isReversed) ? Height-y-1 : y) * image->Height / Height;

                    for(uint64_t x = 0; x < Width; x++) {
                        uint32_t XPosition = x * image->Width / Width;

                        uint8_t R = *(uint8_t*) ((uint64_t)imagePixelData+XPosition*Btpp+Pitch*YPosition + 2);
                        uint8_t G = *(uint8_t*) ((uint64_t)imagePixelData+XPosition*Btpp+Pitch*YPosition + 1);
                        uint8_t B = *(uint8_t*) ((uint64_t)imagePixelData+XPosition*Btpp+Pitch*YPosition + 0);
                        uint32_t Pixel = B | (G << 8) | (R << 16);
                        PutPixel(cpnt->GetFramebuffer(), cpnt->getStyle()->x+x, cpnt->getStyle()->y+y, Pixel);
                    }
                }

                break;
            }
            case 9:
                Printlog("tga type 9");
                break;
            
            case 10:
                Printlog("tga type 10");
                break;
            
            default:
                free(image);
                break;
        }
    }
    
}