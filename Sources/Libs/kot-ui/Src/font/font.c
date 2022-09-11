#include <kot/heap.h>

#include <kot-ui/font.h>
#include <kot-ui/font/ssfn.h>

kfont_t* LoadFont(uintptr_t data){
    kfont_t* font = malloc(sizeof(kfont_t));

    font->Context = calloc(sizeof(ssfn_t));

    ssfn_load(font->Context, data);
    
    return font;
}

void FreeFont(kfont_t* font){
    ssfn_free(font->Context);
    free(font);
}

void PrintFont(kfont_t* font, char* str, font_fb_t* buffer, uint64_t x, uint64_t y, uint8_t fontSize, uint32_t color){
    ssfn_buf_t ssfnBuff;

    ssfn_t* context = (ssfn_t*)font->Context;

    context->size = fontSize;

    ssfnBuff.ptr = buffer->address;
    ssfnBuff.w = buffer->width;
    ssfnBuff.h = buffer->height;
    ssfnBuff.p = buffer->pitch;
    ssfnBuff.x = x;
    ssfnBuff.y = y + context->size;
    ssfnBuff.fg = color;
    ssfnBuff.bg = 0x0;


    while(*str) {
        ssfn_render(font->Context, &ssfnBuff, str++);
    }
}