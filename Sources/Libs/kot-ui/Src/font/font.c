#include <kot-ui/font.h>
#include <kot-ui/font/ssfn.h>

kfont_t* LoadFont(uintptr_t data, enum font_family family, const char* name, int style, int size){
    kfont_t* font = malloc(sizeof(kfont_t));
    font->ctx = calloc(sizeof(ssfn_t));
    ssfn_load(font->ctx, data);
    ssfn_select(font->ctx, family, name, style, size);
}

void FreeFont(kfont_t* font){
    ssfn_free(font->ctx);
    free(font);
}

void PrintFont(kfont_t* font, char* str, font_fb_t* buffer, uint64_t x, uint64_t y){
    ssfn_buf_t ssnfBuffer;
    ssnfBuffer.ptr = (uint8_t*)buffer->address;
    ssnfBuffer.w = (int)buffer->width;
    ssnfBuffer.h = (int)buffer->height;
    ssnfBuffer.p = (uint16_t)buffer->pitch;
    ssnfBuffer.x = (int)x;
    ssnfBuffer.y = (int)y;
    ssfn_render(font->ctx, &ssnfBuffer, str);
}