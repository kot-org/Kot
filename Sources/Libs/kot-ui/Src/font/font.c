#include <kot/heap.h>

#include <kot-ui/font.h>
#include <kot-ui/font/ssfn.h>

kfont_t* LoadFont(uintptr_t data){
    kfont_t* font = malloc(sizeof(kfont_t));

    font->Context = calloc(sizeof(ssfn_t));
    font->FrameBuffer = malloc(sizeof(ssfn_buf_t));

    ssfn_load(font->Context, data);
    
    font->IsSet = false;

    return font;
}

void FreeFont(kfont_t* font){
    ssfn_free(font->Context);
    free(font);
}

void SetFont(kfont_t* font, font_fb_t* buffer, uint64_t x, uint64_t y, uint32_t foregroundcolor, uint32_t backgroundcolor, uint8_t fontSize, int style){
    ssfn_t* context = (ssfn_t*)font->Context;
    ssfn_buf_t* framebuffer = (ssfn_buf_t*)font->FrameBuffer;

    ssfn_select(context, SSFN_FAMILY_ANY, NULL, style, fontSize);

    framebuffer->ptr = buffer->address;
    framebuffer->w = buffer->width;
    framebuffer->h = buffer->height;
    framebuffer->p = buffer->pitch;
    framebuffer->x = x;
    framebuffer->y = y + context->size;
    framebuffer->fg = foregroundcolor;
    framebuffer->bg = backgroundcolor;

    font->IsSet = true;
}

void DrawFont(kfont_t* font, char* str){
    if(font->IsSet){
        while(*str){
            ssfn_render((ssfn_t*)font->Context, (ssfn_buf_t*)font->FrameBuffer, str++);
        }        
    }
}