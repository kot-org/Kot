#include "../picture.h"
#include <kot/stdio.h>

namespace Ui {

    TGA_t* TGARead(TGAHeader_t* Buffer) {
        TGA_t* Image = (TGA_t*) malloc(sizeof(TGA_t));
        
        uint8_t Bpp = Buffer->Bpp;
        uint8_t Btpp = Buffer->Bpp/8;
        Image->Width = Buffer->Width;
        Image->Height = Buffer->Height;
        Image->x = 0;
        Image->y = 0;
        uint32_t Pitch = Image->Width * Btpp;

        uintptr_t ImageDataOffset = (uintptr_t) (Buffer->ColorMapOrigin + Buffer->ColorMapLength + 18),
            ImagePixelData = (uintptr_t) ((uint64_t)Buffer + (uint64_t)ImageDataOffset);

        Image->Pixels = (uint32_t*) malloc(Image->Height * Image->Width * sizeof(uint32_t));

        switch (Buffer->ImageType)
        {
            case TGAType::COLORMAP:
                Printlog("tga type 1");
                break;
            
            case TGAType::RGB:
            {
                // TODO: Retourner l'image si besoin
                //bool isReversed = !(Buffer->ImageDescriptor & (1 << 5));

                if(Bpp == 32) {
                    for(uint16_t y = 0; y < Image->Height; y++) {
                        for(uint16_t x = 0; x < Image->Width; x++) {
                            uint64_t index = (uint64_t)ImagePixelData + x*Btpp + y*Pitch;

                            uint8_t B = *(uint8_t*) (index + 0);
                            uint8_t G = *(uint8_t*) (index + 1);
                            uint8_t R = *(uint8_t*) (index + 2);
                            uint8_t A = *(uint8_t*) (index + 3);

                            Image->Pixels[x + y*Image->Width] = B | (G << 8) | (R << 16) | (A << 24);
                        }
                    }
                } else {
                    for(uint16_t y = 0; y < Image->Height; y++) {
                        for(uint16_t x = 0; x < Image->Width; x++) {
                            uint64_t index = (uint64_t)ImagePixelData + x*Btpp + y*Pitch;

                            uint8_t B = *(uint8_t*) (index + 0);
                            uint8_t G = *(uint8_t*) (index + 1);
                            uint8_t R = *(uint8_t*) (index + 2);
                            uint8_t A = 0xFF;

                            Image->Pixels[x + y*Image->Width] = B | (G << 8) | (R << 16) | (A << 24);
                        }
                    }
                }

                break;
            }
            case TGAType::COLORMAP_RLE:
                Printlog("tga type 9");
                break;
            
            case TGAType::RGB_RLE:
                Printlog("tga type 10");
                break;
            
            default:
                free(Image);
                return NULL;
        }

        return Image;
    }

    void TGADraw(kot_framebuffer_t* Fb, TGA_t* Image) {
        for(uint16_t y = 0; y < Image->Height; y++) {
            for(uint16_t x = 0; x < Image->Width; x++) {
                PutPixel(Fb, Image->x+x, Image->y+y, Image->Pixels[x + y*Image->Width]);
            }
        }
    }

    TGA_t* TGAResize(TGA_t* Image, uint16_t NewWidth, uint16_t NewHeight, bool KeepRatio) {
        TGA_t* ImageResize = (TGA_t*) malloc(sizeof(TGA_t));

        if(KeepRatio){
            if(NewWidth == 0){
                NewWidth = DivideRoundUp(NewHeight * Image->Width, Image->Height);
            }else if(NewHeight == 0){
                NewHeight = DivideRoundUp(NewWidth * Image->Height, Image->Width);
            }
        }
        
        ImageResize->Pixels = (uint32_t*) malloc(NewHeight * NewWidth * sizeof(uint32_t));
        ImageResize->Width = NewWidth;
        ImageResize->Height = NewHeight;
        ImageResize->x = 0;
        ImageResize->y = 0;

        for(uint16_t y = 0; y < NewHeight; y++) {
            uint32_t NewY = y * Image->Height / NewHeight;

            for(uint16_t x = 0; x < NewWidth; x++) {
                uint32_t NewX = x * Image->Width / NewWidth;

                ImageResize->Pixels[x + y*NewWidth] = Image->Pixels[NewX + NewY*Image->Width];
            }
        }

        return ImageResize;
    }

    TGA_t* TGACrop(TGA_t* Image, uint16_t Width, uint16_t Height, uint16_t x, uint16_t y) {
        TGA_t* ImageCrop = (TGA_t*) malloc(sizeof(TGA_t));

        ImageCrop->Pixels = (uint32_t*) malloc(Height * Width * sizeof(uint32_t));
        ImageCrop->Width = Width;
        ImageCrop->Height = Height;
        ImageCrop->x = 0;
        ImageCrop->y = 0;
        

        for(uint16_t h = 0; h < Height; h++) {
            memcpy((uintptr_t)&ImageCrop->Pixels[h*Width], (uintptr_t)&Image->Pixels[x + (h + y)*Image->Width], Width * sizeof(uint32_t));
        }

        return ImageCrop;
    }
    
}