#include <stdlib.h>

#include <kot-graphics/font.h>


#include <ft2build.h>
#include FT_FREETYPE_H

typedef struct {
    FT_Library library;
    FT_Face face;
    FT_GlyphSlot slot;
    FT_UInt glyph_index;
    color_t color;
    kot_framebuffer_t fb;
    FT_Vector pen; 
    kfont_dot_t size;
}local_kfont_t;

kfont_t* LoadFont(void* data, size_t size){
    FT_Error error;

    local_kfont_t* font = malloc(sizeof(local_kfont_t));

    error = FT_Init_FreeType(&font->library);
    if(error){
        free(font);
        return NULL;
    }
    error = FT_New_Memory_Face(font->library, data, size, 0, &font->face);
    if(error){
        free(font);
        return NULL;
    }

    return font;
}

KResult FreeFont(kfont_t* opaque){
    if(opaque == NULL) return KFAIL;

    local_kfont_t* font = (local_kfont_t*)opaque;

    FT_Done_Face(font->face);
    FT_Done_FreeType(font->library);
    free(font);

    return KSUCCESS;
}

KResult LoadPen(kfont_t* opaque, kot_framebuffer_t* fb, kfont_pos_t x, kfont_pos_t y, kfont_dot_t size, font_style_t style, color_t color){
    if(opaque == NULL) return KFAIL;

    local_kfont_t* font = (local_kfont_t*)opaque;

    FT_Set_Char_Size(font->face, 0, size * 64, 0, 0);

    font->pen.x = x;
    font->pen.y = y;

    font->color = color;

    font->slot = font->face->glyph;

    memcpy(&font->fb, fb, sizeof(kot_framebuffer_t));

    return KSUCCESS;
}

KResult SetPenFb(kfont_t* opaque, kot_framebuffer_t* fb){
    if(opaque == NULL) return KFAIL;
    if(fb == NULL) return KFAIL;
    
    local_kfont_t* font = (local_kfont_t*)opaque;

    memcpy(&font->fb, fb, sizeof(kot_framebuffer_t));
    return KSUCCESS;
}

kot_framebuffer_t* GetPenFb(kfont_t* opaque){
    if(opaque == NULL) return KFAIL;
    local_kfont_t* font = (local_kfont_t*)opaque;
    return &font->fb;
}

KResult SetPenPosX(kfont_t* opaque, kfont_pos_t x){
    if(opaque == NULL) return KFAIL;

    local_kfont_t* font = (local_kfont_t*)opaque;

    font->pen.x = x;
    return KSUCCESS;
}

kfont_pos_t GetPenPosX(kfont_t* opaque){
    if(opaque == NULL) return KFAIL;
    local_kfont_t* font = (local_kfont_t*)opaque;
    return font->pen.x;
}

KResult SetPenPosY(kfont_t* opaque, kfont_pos_t y){
    if(opaque == NULL) return KFAIL;

    local_kfont_t* font = (local_kfont_t*)opaque;

    font->pen.y = y;
    return KSUCCESS;
}

kfont_pos_t GetPenPosY(kfont_t* opaque){
    if(opaque == NULL) return KFAIL;
    local_kfont_t* font = (local_kfont_t*)opaque;
    return font->pen.y;
}

KResult SetPenSize(kfont_t* opaque, kfont_dot_t size){
    if(opaque == NULL) return KFAIL;

    local_kfont_t* font = (local_kfont_t*)opaque;

    if(FT_Set_Char_Size(font->face, 0, size * 64, 0, 0)){
        return KFAIL;
    }

    font->size = size;

    return KSUCCESS;
}

kfont_dot_t GetPenSize(kfont_t* opaque){
    if(opaque == NULL) return KFAIL;
    local_kfont_t* font = (local_kfont_t*)opaque;
    return font->size;
}

KResult SetPenStyle(kfont_t* opaque, font_style_t style){
    if(opaque == NULL) return KFAIL;

    local_kfont_t* font = (local_kfont_t*)opaque;

    // TODO
    return KSUCCESS;
}

font_style_t GetPenStyle(kfont_t* opaque){
    if(opaque == NULL) return KFAIL;
    local_kfont_t* font = (local_kfont_t*)opaque;

    // TODO
    return 0;
}

KResult SetPenColor(kfont_t* opaque, color_t color){
    if(opaque == NULL) return KFAIL;

    local_kfont_t* font = (local_kfont_t*)opaque;

    font->color = color;
    return KSUCCESS;
}

color_t GetPenColor(kfont_t* opaque){
    if(opaque == NULL) return KFAIL;
    local_kfont_t* font = (local_kfont_t*)opaque;
    return font->color;
}

kfont_pos_t GetLineHeight(kfont_t* opaque){
    if(opaque == NULL) return 0;

    local_kfont_t* font = (local_kfont_t*)opaque;
    return font->face->size->metrics.height >> 6;
}

KResult DrawFont(kfont_t* opaque, char* str){
    if(opaque == NULL) return KFAIL;

    local_kfont_t* font = (local_kfont_t*)opaque;

    FT_GlyphSlot slot = font->slot;


    while(*str){
        if(*str == '\n'){
            font->pen.x = 0;
            font->pen.y += GetLineHeight(opaque);
            str++;
            continue;
        }

        if(FT_Load_Char(font->face, *str, FT_LOAD_RENDER)){
            str++;
            continue;
        }

        for(FT_Int x = 0; x < slot->bitmap.width; x++){
            for(FT_Int y = 0; y < slot->bitmap.rows; y++){
                FT_Int XPos = font->pen.x + slot->bitmap_left + x;
                FT_Int YPos = font->pen.y - slot->bitmap_top + y + GetLineHeight(opaque);
                PutPixel(&font->fb, XPos, YPos, 0xffffff | (slot->bitmap.buffer[y * slot->bitmap.width + x] << 24));
            }
        }

        font->pen.x += slot->advance.x >> 6;
        font->pen.y += slot->advance.y;

        str++;
    }

    return KSUCCESS;
}

KResult DrawFontN(kfont_t* opaque, char* str, size_t len){
    if(opaque == NULL) return KFAIL;

    local_kfont_t* font = (local_kfont_t*)opaque;

    FT_GlyphSlot slot = font->slot;


    for(size_t i = 0; i < len; i++){
        if(str[i] == '\n'){
            font->pen.x = 0;
            font->pen.y += GetLineHeight(opaque);
            continue;
        }

        if(FT_Load_Char(font->face, str[i], FT_LOAD_RENDER)){
            continue;
        }

        for(FT_Int x = 0; x < slot->bitmap.width; x++){
            for(FT_Int y = 0; y < slot->bitmap.rows; y++){
                FT_Int XPos = font->pen.x + slot->bitmap_left + x;
                FT_Int YPos = font->pen.y - slot->bitmap_top + y + GetLineHeight(opaque);
                PutPixel(&font->fb, XPos, YPos, 0xffffff | (slot->bitmap.buffer[y * slot->bitmap.width + x] << 24));
            }
        }

        font->pen.x += slot->advance.x >> 6;
        font->pen.y += slot->advance.y;
    }

    return KSUCCESS;
}

KResult GetTextboxInfo(kfont_t* opaque, char* str, kfont_pos_t* width, kfont_pos_t* height, kfont_pos_t* x, kfont_pos_t* y){
    if(opaque == NULL) return KFAIL;

    local_kfont_t* font = (local_kfont_t*)opaque;

    FT_GlyphSlot slot = font->slot;

    *x = 0;
    *y = 0;

    *width = 0;

    while(*str){
        if(*str == '\n'){
            *x = 0;
            *y += GetLineHeight(opaque);
            str++;
            continue;
        }

        if(FT_Load_Char(font->face, *str, FT_LOAD_RENDER)){
            str++;
            continue;
        }

        *x += slot->advance.x >> 6;
        *y += slot->advance.y;

        if(*x > *width){
            *width = *x;
        }

        str++;
    }

    *height = *y + GetLineHeight(opaque);

    return KSUCCESS;
}

KResult GetTextboxInfoN(kfont_t* opaque, char* str, size_t len, kfont_pos_t* width, kfont_pos_t* height, kfont_pos_t* x, kfont_pos_t* y){
    if(opaque == NULL) return KFAIL;

    local_kfont_t* font = (local_kfont_t*)opaque;

    FT_GlyphSlot slot = font->slot;

    *x = 0;
    *y = 0;

    *width = 0;

    for(size_t i = 0; i < len; i++){
        if(str[i] == '\n'){
            *x = 0;
            *y += GetLineHeight(opaque);
            continue;
        }

        if(FT_Load_Char(font->face, str[i], FT_LOAD_RENDER)){
            continue;
        }

        *x += slot->advance.x >> 6;
        *y += slot->advance.y;

        if(*x > *width){
            *width = *x;
        }
    }

    *height = *y + GetLineHeight(opaque);

    return KSUCCESS;
}

KResult DrawGlyph(kfont_t* opaque, kfont_glyph_t glyph, kfont_dot_t width, kfont_dot_t height){
    if(opaque == NULL) return KFAIL;

    local_kfont_t* font = (local_kfont_t*)opaque;

    FT_GlyphSlot slot = font->slot;

    FT_Set_Pixel_Sizes(font->face, width, height);

    if(FT_Load_Glyph(font->face, glyph, FT_LOAD_RENDER)){
        return KFAIL;
    }
    
    for(FT_Int x = 0; x < slot->bitmap.width; x++){
        for(FT_Int y = 0; y < slot->bitmap.rows; y++){
            FT_Int XPos = font->pen.x + slot->bitmap_left + x;
            FT_Int YPos = font->pen.y - slot->bitmap_top + y + GetLineHeight(opaque);
            PutPixel(&font->fb, XPos, YPos, 0xffffff | (slot->bitmap.buffer[y * slot->bitmap.width + x] << 24));
        }
    }
    
    return KSUCCESS;
}
