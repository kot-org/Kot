#include <kot/heap.h>

#include <kot-ui/font.h>
#include <kot-ui/font/ssfn.h>

kfont_t* LoadFont(uintptr_t data){
    kfont_t* font = malloc(sizeof(kfont_t));

    font->ctx = malloc(sizeof(ssfn_t));
    ssfn_load(font->ctx, data);
    ssfn_select(font->ctx, SSFN_FAMILY_ANY, NULL, SSFN_STYLE_REGULAR, NULL);
    
    return font;
}

void FreeFont(kfont_t* font){
    ssfn_free(font->ctx);
    free(font);
}

/** 
 * @param fontSize Set null to have the default value
 */
void PrintFont(kfont_t* font, char* str, font_fb_t* buffer, uint64_t x, uint64_t y, uint8_t fontSize, uint32_t color){
    ssfn_buf_t ssfnBuff;

    ssfnBuff.ptr = buffer->address;
    ssfnBuff.w = buffer->width;
    ssfnBuff.h = buffer->height;
    ssfnBuff.p = buffer->pitch;
    if(fontSize != NULL)
        ((ssfn_t*) font->ctx)->size = fontSize;
    else
        ((ssfn_t*) font->ctx)->size = 16;
    ssfnBuff.x = x;
    ssfnBuff.y = y + ((ssfn_t*) font->ctx)->size;
    ssfnBuff.fg = color;
    ssfnBuff.bg = 0x0;

    while(*str) {
        if(str == '\n') {
            ssfnBuff.x = 0;
            ssfnBuff.y = ssfnBuff.h;
        }
        ssfn_render(font->ctx, &ssfnBuff, str++);
    }
}