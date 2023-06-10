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
    error = FT_Set_Char_Size(font->face, 0, 16 * 64, 96, 96); // Set default font size
    if(error){
        free(font);
        return NULL;
    }

    return font;
}

void FreeFont(kfont_t* opaque){
    if(opaque == NULL) return;

    local_kfont_t* font = (local_kfont_t*)opaque;

    FT_Done_Face(font->face);
    FT_Done_FreeType(font->library);
    free(font);
}

void LoadPen(kfont_t* opaque, kot_framebuffer_t* fb, int64_t x, int64_t y, int16_t size, int32_t style, int64_t color) {
    if(opaque == NULL) return;

    local_kfont_t* font = (local_kfont_t*)opaque;

    FT_Set_Pixel_Sizes(font->face, 0, size); // Set font size

    font->pen.x = x;
    font->pen.y = y;

    font->color = 0xffffffff;

    font->slot = font->face->glyph;

    memcpy(&font->fb, fb, sizeof(kot_framebuffer_t));
}

void EditPen(kfont_t* font, kot_framebuffer_t* fb, int64_t x, int64_t y, int16_t size, int32_t style, int64_t color){

}

void DrawFont(kfont_t* opaque, char* str){
    kot_Printlog(str);
    return;
    if(opaque == NULL) return;

    local_kfont_t* font = (local_kfont_t*)opaque;

    FT_GlyphSlot slot = font->slot;


    while(*str){
        if(*str == '\n'){
            str++;
            continue;
        }

        if(FT_Load_Char(font->face, *str, FT_LOAD_RENDER)){
            str++;
            continue;
        }

        for(FT_Int x = 0; x < slot->bitmap.width; x++){
            for(FT_Int y = 0; y < slot->bitmap.rows; y++){
                // uint32_t buffer_x = font->pen.x + slot->bitmap_left + x;
                // uint32_t buffer_y = font->pen.y - slot->bitmap_top + y;
                // PutPixel(&font->fb, buffer_x, buffer_y, 0xffffff | (slot->bitmap.buffer[y * slot->bitmap.width + x] << 24));
            }
        }

        font->pen.x += slot->advance.x >> 6;
        font->pen.y += slot->advance.y;

        str++;
    }
}

void GetTextboxInfo(kfont_t* opaque, char* str, int64_t* width, int64_t* height, int64_t* x, int64_t* y){
    if(opaque == NULL) return;

    local_kfont_t* font = (local_kfont_t*)opaque;

    FT_GlyphSlot slot = font->slot;

    *width = 0;
    *height = 0;
    *x = 0;
    *y = 0;

    while (*str) {
        if(FT_Load_Char(font->face, *str, FT_LOAD_RENDER)){
            continue;
        }

        *width += slot->advance.x >> 6;
        *height = slot->bitmap.rows;

        str++;
    }
}
