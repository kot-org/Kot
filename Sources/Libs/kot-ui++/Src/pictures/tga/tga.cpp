#include "../picture.h"

namespace Ui {

    TGA_t* TGARead(TGAHeader_t* Buffer) {
        TGA_t* Image = (TGA_t*) malloc(sizeof(TGA_t));
        
        uint8_t Bpp = Buffer->Bpp;
        uint8_t Btpp = Buffer->Bpp/8;
        Image->Width = Buffer->Width;
        Image->Height = Buffer->Height;
        uint32_t Pitch = Image->Width * Btpp;
        //bool isReversed = !(Buffer->ImageDescriptor & (1 << 5));

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
                Printlog("tga type 2");

                // TODO: Retourner l'image si besoin

                for(uint16_t y = 0; y < Image->Height; y++) {
                    for(uint16_t x = 0; x < Image->Width; x++) {
                        uint64_t index = (uint64_t)ImagePixelData + x*Btpp + y*Pitch;

                        uint8_t B = *(uint8_t*) (index + 0);
                        uint8_t G = *(uint8_t*) (index + 1);
                        uint8_t R = *(uint8_t*) (index + 2);

                        uint8_t A = 0xFF;
                        if(Bpp == 32)
                            A = *(uint8_t*) (index + 3);

                        Image->Pixels[x + y*Image->Width] = B | (G << 8) | (R << 16) | (A << 24);
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
                break;
        }

        return Image;
    }

    void TGADraw(framebuffer_t* Fb, TGA_t* Image) {
        for(uint16_t y = 0; y < Image->Height; y++) {
            for(uint16_t x = 0; x < Image->Width; x++) {
                PutPixel(Fb, x, y, Image->Pixels[x + y*Image->Width]);
            }
        }
    }

    TGA_t* TGAResize(TGA_t* Image, uint16_t NewWidth, uint16_t NewHeight) {
        TGA_t* ImageResize = (TGA_t*) malloc(sizeof(TGA_t));
        
        ImageResize->Pixels = (uint32_t*) malloc(NewHeight * NewWidth * sizeof(uint32_t));
        ImageResize->Width = NewWidth;
        ImageResize->Height = NewHeight;

        for(uint16_t y = 0; y < NewHeight; y++) {
            uint32_t NewY = y * Image->Height / NewHeight;

            for(uint16_t x = 0; x < NewWidth; x++) {
                uint32_t NewX = x * Image->Width / NewWidth;

                ImageResize->Pixels[x + y*NewWidth] = Image->Pixels[NewX + NewY*Image->Width];
            }
        }

        return ImageResize;
    }
    
}