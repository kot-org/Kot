#include <kot/heap.h>

#include <kot-ui/font.h>
#include <kot-ui/font/ssfn.h>

kfont_t* LoadFont(uintptr_t data){
    kfont_t* font = malloc(sizeof(kfont_t));

    font->FontContext = calloc(sizeof(ssfn_t));
    font->PenContext = calloc(sizeof(ssfn_buf_t));
    font->IsPen = false;

    ssfn_load(font->FontContext, data);
    
    return font;
}

void FreeFont(kfont_t* font){
    ssfn_free(font->FontContext);
    free(font->FontContext);
    free(font->PenContext);
}

void LoadPen(kfont_t* font, font_fb_t* buffer, uint64_t x, uint64_t y, uint8_t size, uint16_t style, uint32_t color){
    ssfn_buf_t* Pen = (ssfn_buf_t*)font->PenContext;
    Pen->ptr = buffer->address;
    Pen->w = buffer->width;
    Pen->h = buffer->height;
    Pen->p = buffer->pitch;
    Pen->x = x;
    Pen->y = y;
    Pen->fg = color;
    Pen->bg = 0x0;

    ssfn_select(font->FontContext, SSFN_FAMILY_ANY, NULL, style, size);
    if(y == 0) ssfn_newline(font->FontContext, font->PenContext);
    font->IsPen = true;
}

void DrawFontGetPos(kfont_t* font, char* str, uint64_t* x, uint64_t* y){
    if(font->IsPen){
        ssfn_buf_t* Pen = (ssfn_buf_t*)font->PenContext;
        while(*str) {
            ssfn_render(font->FontContext, font->PenContext, str++);
        }

        *x = Pen->x;
        *y = Pen->y;
    }
}

void DrawFont(kfont_t* font, char* str){
    if(font->IsPen){
        ssfn_buf_t* Pen = (ssfn_buf_t*)font->PenContext;
        while(*str) {
            ssfn_render(font->FontContext, font->PenContext, str++);
        }
    }
}