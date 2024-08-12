#include <kot-graphics/utils.h>

int pixel_exist(kframebuffer_t* fb, uint32_t x, uint32_t y){
    if(x < 0 || y < 0){
        return -1;
    } 
    if(x > fb->width || y > fb->height){
        return -1;
    } 
    return 1;
}

void put_pixel(kframebuffer_t* fb, uint32_t x, uint32_t y, uint32_t color){
    if(pixel_exist(fb, x, y) == -1){
        return;
    } 
    uint64_t index = x * fb->btpp + y * fb->pitch;
    *(uint32_t*)((uint64_t)fb->buffer + index) = color;
}

uint32_t get_pixel(kframebuffer_t* fb, uint32_t x, uint32_t y){
    if(pixel_exist(fb, x, y) == -1){
        return 0;
    } 
    uint64_t index = x * fb->btpp + y * fb->pitch;
    return *(uint32_t*)((uint64_t)fb->buffer + index);
}

uint32_t blend_colors(uint32_t color1, uint32_t color2, float factor){
    uint8_t r1 = (color1 >> 16) & 0xFF;
    uint8_t g1 = (color1 >> 8) & 0xFF;
    uint8_t b1 = color1 & 0xFF;

    uint8_t r2 = (color2 >> 16) & 0xFF;
    uint8_t g2 = (color2 >> 8) & 0xFF;
    uint8_t b2 = color2 & 0xFF;

    uint8_t r = (uint8_t)(r1 * (1 - factor) + r2 * factor);
    uint8_t g = (uint8_t)(g1 * (1 - factor) + g2 * factor);
    uint8_t b = (uint8_t)(b1 * (1 - factor) + b2 * factor);
    
    return (r << 16) | (g << 8) | b;
}

uint32_t blend_alpha(uint32_t color, uint8_t factor){
    uint8_t r = (color >> 16) & 0xFF;
    uint8_t g = (color >> 8) & 0xFF;
    uint8_t b = color & 0xFF;

    r = (uint8_t)((r * factor) / 255);
    g = (uint8_t)((g * factor) / 255);
    b = (uint8_t)((b * factor) / 255);

    return (r << 16) | (g << 8) | b;
}