#include <stdlib.h>

#include <kot-graphics/font.h>

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H

#define ROUND_UP_DIVISION(a, b) (((a) + (b) - 1) / (b))

typedef struct {
    FT_Library library;
    FT_Face face;
    FT_GlyphSlot slot;
    FT_UInt glyph_index;
    color_t color;
    kframebuffer_t fb;
    FT_Vector pen; 
    kfont_dot_t size;
    size_t space_size;
}local_kfont_t;

kfont_t load_font(void* data, size_t size){
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

int free_font(kfont_t opaque){
    if(opaque == NULL) return -1;

    local_kfont_t* font = (local_kfont_t*)opaque;

    FT_Done_Face(font->face);
    FT_Done_FreeType(font->library);
    free(font);

    return 0;
}

int load_pen(kfont_t opaque, kframebuffer_t* fb, kfont_pos_t x, kfont_pos_t y, kfont_dot_t size, font_style_t style, color_t color){
    if(opaque == NULL) return -1;

    local_kfont_t* font = (local_kfont_t*)opaque;

    FT_Set_Char_Size(font->face, 0, size * 64, 0, 0);

    font->pen.x = x;
    font->pen.y = y;

    font->color = color;

    font->slot = font->face->glyph;
    FT_Load_Glyph(font->face, ' ', FT_LOAD_RENDER);
    font->space_size = get_glyph_width(font);


    memcpy(&font->fb, fb, sizeof(kframebuffer_t));

    return 0;
}

int set_pen_fb(kfont_t opaque, kframebuffer_t* fb){
    if(opaque == NULL) return -1;
    if(fb == NULL) return -1;
    
    local_kfont_t* font = (local_kfont_t*)opaque;

    memcpy(&font->fb, fb, sizeof(kframebuffer_t));
    return 0;
}

kframebuffer_t* get_pen_fb(kfont_t opaque){
    if(opaque == NULL) return NULL;
    local_kfont_t* font = (local_kfont_t*)opaque;
    return &font->fb;
}

int set_pen_pos_x(kfont_t opaque, kfont_pos_t x){
    if(opaque == NULL) return -1;

    local_kfont_t* font = (local_kfont_t*)opaque;

    font->pen.x = x;
    return 0;
}

kfont_pos_t get_pen_pos_x(kfont_t opaque){
    if(opaque == NULL) return -1;
    local_kfont_t* font = (local_kfont_t*)opaque;
    return font->pen.x;
}

int set_pen_pos_y(kfont_t opaque, kfont_pos_t y){
    if(opaque == NULL) return -1;

    local_kfont_t* font = (local_kfont_t*)opaque;

    font->pen.y = y;
    return 0;
}

kfont_pos_t get_pen_pos_y(kfont_t opaque){
    if(opaque == NULL) return -1;
    local_kfont_t* font = (local_kfont_t*)opaque;
    return font->pen.y;
}

int set_pen_size(kfont_t opaque, kfont_dot_t size){
    if(opaque == NULL) return -1;

    local_kfont_t* font = (local_kfont_t*)opaque;

    if(FT_Set_Char_Size(font->face, 0, size * 64, 0, 0)){
        return -1;
    }

    font->size = size;

    return 0;
}

kfont_dot_t get_pen_size(kfont_t opaque){
    if(opaque == NULL) return -1;
    local_kfont_t* font = (local_kfont_t*)opaque;
    return font->size;
}

int set_pen_style(kfont_t opaque, font_style_t style){
    if(opaque == NULL) return -1;

    local_kfont_t* font = (local_kfont_t*)opaque;

    // TODO
    return 0;
}

font_style_t get_pen_style(kfont_t opaque){
    if(opaque == NULL) return -1;
    local_kfont_t* font = (local_kfont_t*)opaque;

    // TODO
    return 0;
}

int set_pen_color(kfont_t opaque, color_t color){
    if(opaque == NULL) return -1;

    local_kfont_t* font = (local_kfont_t*)opaque;

    font->color = color & 0xffffff;
    return 0;
}

color_t get_pen_color(kfont_t opaque){
    if(opaque == NULL) return -1;
    local_kfont_t* font = (local_kfont_t*)opaque;
    return font->color;
}

kfont_pos_t get_line_height(kfont_t opaque){
    if(opaque == NULL) return 0;

    local_kfont_t* font = (local_kfont_t*)opaque;
    return font->face->size->metrics.height >> 6;
}

kfont_pos_t get_glyph_width(kfont_t opaque){
    if(opaque == NULL) return 0;

    local_kfont_t* font = (local_kfont_t*)opaque;
    return font->face->glyph->metrics.width >> 6;
}

kfont_pos_t get_glyph_bearing_y(kfont_t opaque){
    if(opaque == NULL) return 0;

    local_kfont_t* font = (local_kfont_t*)opaque;
    return font->face->glyph->metrics.horiBearingY >> 6;
}

int draw_font(kfont_t opaque, char* str){
    return draw_font_n(opaque, str, strlen(str));
}

int draw_font_n(kfont_t opaque, char* str, size_t len){
    if(opaque == NULL) return -1;

    local_kfont_t* font = (local_kfont_t*)opaque;

    FT_GlyphSlot slot = font->slot;

    kfont_pos_t x_pos_initial = font->pen.x;

    for(size_t i = 0; i < len; i++){
        if(str[i] == '\n'){
            font->pen.x = x_pos_initial;
            font->pen.y += get_line_height(opaque);
            continue;
        }

        if(str[i] == ' '){
            font->pen.x += font->space_size;
            continue;
        }

        if(FT_Load_Char(font->face, str[i], FT_LOAD_RENDER)){
            continue;
        }

        uint64_t delta_x = font->pen.x + slot->bitmap_left;
        uint64_t delta_y = font->pen.y - slot->bitmap_top + get_line_height(opaque);

        for(FT_Int x = 0; x < slot->bitmap.width; x++){
            for(FT_Int y = 0; y < slot->bitmap.rows; y++){
                FT_Int x_pos = delta_x + x;
                FT_Int y_pos = delta_y + y;
                if(slot->bitmap.buffer[y * slot->bitmap.width + x]){
                    put_pixel(&font->fb, x_pos, y_pos, blend_colors(get_pixel(&font->fb, x_pos, y_pos), font->color, slot->bitmap.buffer[y * slot->bitmap.width + x]));
                }
            }
        }

        font->pen.x += slot->advance.x >> 6;
        font->pen.y += slot->advance.y;
    }

    return 0;
}

int get_textbox_info(kfont_t opaque, char* str, kfont_pos_t* width, kfont_pos_t* height, kfont_pos_t* x, kfont_pos_t* y){
    return get_textbox_info_n(opaque, str, strlen(str), width, height, x, y);
}

int get_textbox_info_n(kfont_t opaque, char* str, size_t len, kfont_pos_t* width, kfont_pos_t* height, kfont_pos_t* x, kfont_pos_t* y){
    if(opaque == NULL) return -1;

    local_kfont_t* font = (local_kfont_t*)opaque;

    FT_GlyphSlot slot = font->slot;

    kfont_pos_t x_delta = 0;
    kfont_pos_t y_delta = 0;

    y_delta = get_line_height(opaque);

    if(width != NULL){
        *width = 0;
    }

    for(size_t i = 0; i < len; i++){
        if(str[i] == '\n'){
            x_delta = 0;
            y_delta += get_line_height(opaque);
            continue;
        }

        if(FT_Load_Char(font->face, str[i], FT_LOAD_RENDER)){
            continue;
        }

        x_delta += slot->advance.x >> 6;
        y_delta += slot->advance.y;

        if(width != NULL){
            if(x_delta > *width){
                *width = x_delta;
            }
        }
    }

    if(height != NULL){
        *height = y_delta;
    }
    if(x != NULL){
        *x = y_delta + font->pen.x;
    }
    if(y != NULL){
        *y = y_delta + font->pen.y;
    }

    return 0;
}

int draw_glyph(kfont_t opaque, kfont_glyph_t glyph, kfont_dot_t width, kfont_dot_t height){
    if(opaque == NULL) return -1;

    local_kfont_t* font = (local_kfont_t*)opaque;

    FT_GlyphSlot slot = font->slot;

    FT_Set_Pixel_Sizes(font->face, 0, height);

    if(FT_Load_Glyph(font->face, glyph, FT_LOAD_RENDER)){
        return -1;
    }

    uint64_t delta_x = font->pen.x + slot->bitmap_left + (width - get_glyph_width(opaque)) / 2;
    uint64_t delta_y = font->pen.y - slot->bitmap_top + get_glyph_bearing_y(opaque) + (height - get_glyph_bearing_y(opaque)) / 2;
    
    for(FT_Int x = 0; x < slot->bitmap.width; x++){
        for(FT_Int y = 0; y < slot->bitmap.rows; y++){
            FT_Int x_pos = delta_x + x;
            FT_Int y_pos = delta_y + y;
            if(slot->bitmap.buffer[y * slot->bitmap.width + x]){
                put_pixel(&font->fb, x_pos, y_pos, blend_alpha(font->color, slot->bitmap.buffer[y * slot->bitmap.width + x]));
            }
        }
    }
    
    return 0;
}

int write_paragraph(kfont_t opaque, kfont_pos_t x, kfont_pos_t y, kfont_pos_t width, int format, char* text){
    local_kfont_t* font = (local_kfont_t*)opaque;
    FT_Load_Glyph(font->face, ' ', FT_LOAD_RENDER);
    
    kfont_pos_t line_height = get_line_height(opaque);
    kfont_pos_t char_width = get_glyph_width(opaque);
    kfont_pos_t text_width;
    kfont_pos_t max_char_line = width / char_width;

    if(x < 0){
        x = get_pen_pos_x(opaque);
    }

    if(y < 0){
        y = get_pen_pos_y(opaque);
    }
    kfont_pos_t current_x = x;
    kfont_pos_t current_y = y;

    font->space_size = char_width;

    char* line_start = text;
    char* line_end = text;
    
    while(*line_end){
        while (*line_end && *line_end != '\n') line_end++;

        bool is_dash_next_line = false;

        set_pen_pos_x(opaque, current_x);
        set_pen_pos_y(opaque, current_y);
        
        text_width = strlen(line_start) * char_width;

        if(text_width > width){
            text_width = width;
            line_end = line_start + max_char_line;
            while(*line_end != ' ' || text_width > width){
                line_end--;

                text_width -= char_width;

                if(line_end <= line_start){
                    text_width = width;
                    line_end = line_start + max_char_line;
                    is_dash_next_line = true;
                    break;
                }
            }

            if(format == PARAGRAPH_JUSTIFY){
                int space_available = 0;

                char* c = line_start;
                while(c != line_end){
                    if(*c == ' '){
                        space_available++;
                    }
                    c++;
                }

                if(space_available > 0){
                    kfont_pos_t real_text_width = text_width;
                    get_textbox_info_n(opaque, line_start, line_end - line_start, &real_text_width, NULL, NULL, NULL);
                    font->space_size += (width - real_text_width) / space_available;
                }
            }
        }

        if(format == PARAGRAPH_CENTER){
            kfont_pos_t real_text_width = text_width;
            get_textbox_info_n(opaque, line_start, line_end - line_start, &real_text_width, NULL, NULL, NULL);
            current_x += (width - real_text_width) / 2;
            set_pen_pos_x(opaque, current_x);
        }

        if(format == PARAGRAPH_RIGHT){
            kfont_pos_t real_text_width = text_width;
            get_textbox_info_n(opaque, line_start, line_end - line_start, &real_text_width, NULL, NULL, NULL);
            current_x += width - real_text_width;
            set_pen_pos_x(opaque, current_x);            
        }

        draw_font_n(opaque, line_start, line_end - line_start);
        
        if(is_dash_next_line){
            draw_font_n(opaque, "-", 1);
        }

        current_y += line_height;
        current_x = x;
        line_start = line_end;
        if(*line_start == ' '){
            line_start++;
        }
        while (*line_start == '\n') line_start++;
        line_end = line_start;


        if(format == PARAGRAPH_JUSTIFY){
            font->space_size = char_width;
        }
    }

    set_pen_pos_x(opaque, current_x);
    set_pen_pos_y(opaque, current_y);

    return 0;
}