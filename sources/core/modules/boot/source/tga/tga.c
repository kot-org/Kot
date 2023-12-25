#include "tga.h"

#include <lib/math.h>

#include <global/heap.h>

tga_t* tga_read(tga_header_t* buffer) {
    tga_t* image = (tga_t*) malloc(sizeof(tga_t));
    
    uint8_t bpp = buffer->bpp;
    uint8_t btpp = buffer->bpp/8;
    image->width = buffer->width;
    image->height = buffer->height;
    image->x = 0;
    image->y = 0;
    uint32_t pitch = image->width * btpp;

    void* image_data_offset = (void*) (buffer->colorMapOrigin + buffer->colorMapLength + 18);
    void* image_pixel_data = (void*) ((uint64_t)buffer + (uint64_t)image_data_offset);

    image->pixels = (uint32_t*) malloc(image->height * image->width * sizeof(uint32_t));

    switch (buffer->imageType)
    {
        case TYPE_COLORMAP:
            kot_Printlog("tga type 1");
            break;
        
        case TYPE_RGB:
        {
            // TODO: Retourner l'image si besoin
            //bool isReversed = !(Buffer->ImageDescriptor & (1 << 5));

            if(bpp == 32) {
                for(uint16_t y = 0; y < image->height; y++) {
                    for(uint16_t x = 0; x < image->width; x++) {
                        uint64_t index = (uint64_t)image_pixel_data + x*btpp + y*pitch;

                        uint8_t B = *(uint8_t*) (index + 0);
                        uint8_t G = *(uint8_t*) (index + 1);
                        uint8_t R = *(uint8_t*) (index + 2);
                        uint8_t A = *(uint8_t*) (index + 3);

                        image->pixels[x + y*image->width] = B | (G << 8) | (R << 16) | (A << 24);
                    }
                }
            } else {
                for(uint16_t y = 0; y < image->height; y++) {
                    for(uint16_t x = 0; x < image->width; x++) {
                        uint64_t index = (uint64_t)image_pixel_data + x*btpp + y*pitch;

                        uint8_t B = *(uint8_t*) (index + 0);
                        uint8_t G = *(uint8_t*) (index + 1);
                        uint8_t R = *(uint8_t*) (index + 2);
                        uint8_t A = 0xFF;

                        image->pixels[x + y*image->width] = B | (G << 8) | (R << 16) | (A << 24);
                    }
                }
            }

            break;
        }
        case TYPE_COLORMAP_RLE:
            kot_Printlog("tga type 9");
            break;
        
        case TYPE_RGB_RLE:
            kot_Printlog("tga type 10");
            break;
        
        default:
            free(image);
            return NULL;
    }

    return image;
}

void tga_draw(graphics_boot_fb_t* fb, tga_t* image) {
    for(uint16_t y = 0; y < image->height; y++) {
        for(uint16_t x = 0; x < image->width; x++) {
            PutPixel(fb, image->x+x, image->y+y, image->pixels[x + y*image->width]);
        }
    }
}

tga_t* tga_resize(tga_t* image, uint16_t new_width, uint16_t new_height, bool keep_ratio) {
    tga_t* image_resize = (tga_t*) malloc(sizeof(tga_t));

    if(keep_ratio){
        if(new_width == 0){
            new_width = DIV_ROUNDUP(new_height * image->width, image->height);
        }else if(new_height == 0){
            new_height = DIV_ROUNDUP(new_width * image->height, image->width);
        }
    }
    
    image_resize->pixels = (uint32_t*) malloc(new_height * new_width * sizeof(uint32_t));
    image_resize->width = new_width;
    image_resize->height = new_height;
    image_resize->x = 0;
    image_resize->y = 0;

    for(uint16_t y = 0; y < new_height; y++) {
        uint32_t newY = y * image->height / new_height;

        for(uint16_t x = 0; x < new_width; x++) {
            uint32_t newX = x * image->width / new_width;

            image_resize->pixels[x + y*new_width] = image->pixels[newX + newY*image->width];
        }
    }

    return image_resize;
}

tga_t* tga_crop(tga_t* image, uint16_t width, uint16_t height, uint16_t x, uint16_t y) {
    tga_t* image_crop = (tga_t*) malloc(sizeof(tga_t));

    image_crop->pixels = (uint32_t*) malloc(height * width * sizeof(uint32_t));
    image_crop->width = width;
    image_crop->height = height;
    image_crop->x = 0;
    image_crop->y = 0;
    

    for(uint16_t h = 0; h < height; h++) {
        memcpy((void*)&image_crop->pixels[h*width], (void*)&image->pixels[x + (h + y)*image->width], width * sizeof(uint32_t));
    }

    return image_crop;
}