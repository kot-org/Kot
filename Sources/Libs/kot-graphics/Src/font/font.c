#include <kot/heap.h>

#include <kot-graphics/font.h>
#include <kot-graphics/font/ssfn.h>

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

void LoadPen(kfont_t* font, font_fb_t* buffer, int64_t x, int64_t y, int16_t size, int32_t style, int64_t color){
    if(font->IsPen){
        EditPen(font, buffer, x, y, size, style, color);
    }else if(buffer != NULL){
        if(x < 0){
            x = 0;
        }
        if(y < 0){
            y = 0;
        }
        if(size < 0){
            size = 8;
        }else if(size < 8 || size > SSFN_SIZE_MAX){
            size = 8;
        }
        if(style < 0){
            style = 0;
        }
        if(color < 0){
            color = 0;
        }

        ssfn_buf_t* Pen = (ssfn_buf_t*)font->PenContext;
        Pen->ptr = buffer->Address;
        Pen->w = buffer->Width;
        Pen->h = buffer->Height;
        Pen->p = buffer->Pitch;
        Pen->x = x;
        Pen->y = y;
        Pen->fg = color;
        Pen->bg = 0x0;

        ssfn_select(font->FontContext, SSFN_FAMILY_ANY, NULL, style, size);
        ssfn_newline(font->FontContext, font->PenContext);
        font->IsPen = true;
    }
}

void EditPen(kfont_t* font, font_fb_t* buffer, int64_t x, int64_t y, int16_t size, int32_t style, int64_t color){
    ssfn_t* Font = (ssfn_t*)font->FontContext; 
    ssfn_buf_t* Pen = (ssfn_buf_t*)font->PenContext;
    if(buffer != NULL){
        Pen->ptr = buffer->Address;
        Pen->w = buffer->Width;
        Pen->h = buffer->Height;
        Pen->p = buffer->Pitch;
    }
    if(x >= 0){
        Pen->x = x;
    }
    if(y >= 0){
        Pen->y = y;
    }
    if(size >= 0){
        if(size < 8 || size > SSFN_SIZE_MAX){
            size = 8;
        }
        Font->size = size;
    }
    if(style >= 0){
        Font->style = style;
    }
    if(color >= 0){
        Pen->fg = color;
    }

    ssfn_newline(font->FontContext, font->PenContext);
    font->IsPen = true;
}

void DrawFontGetPos(kfont_t* font, char* str, int64_t* x, int64_t* y){
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