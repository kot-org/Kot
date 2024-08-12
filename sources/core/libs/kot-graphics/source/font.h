#ifndef KOT_GRAPHICS_FONT
#define KOT_GRAPHICS_FONT

#define PARAGRAPH_LEFT      0
#define PARAGRAPH_CENTER    1
#define PARAGRAPH_RIGHT     2
#define PARAGRAPH_JUSTIFY   3

#include <kot-graphics/utils.h>

typedef void* kfont_t;
typedef uint64_t font_style_t;
typedef signed long kfont_pos_t;
typedef signed long kfont_dot_t;
typedef unsigned long kfont_glyph_t;

kfont_t load_font(void* data, size_t size);
int free_font(kfont_t opaque);

int load_pen(kfont_t opaque, kframebuffer_t* fb, kfont_pos_t x, kfont_pos_t y, kfont_dot_t size, font_style_t style, color_t color);

int set_pen_fb(kfont_t opaque, kframebuffer_t* fb);
kframebuffer_t* get_pen_fb(kfont_t opaque);

int set_pen_pos_x(kfont_t opaque, kfont_pos_t x);
kfont_pos_t get_pen_pos_x(kfont_t opaque);

int set_pen_pos_y(kfont_t opaque, kfont_pos_t y);
kfont_pos_t get_pen_pos_y(kfont_t opaque);

int set_pen_size(kfont_t opaque, kfont_dot_t size);
kfont_dot_t get_pen_size(kfont_t opaque);

int set_pen_style(kfont_t opaque, font_style_t style);
font_style_t get_pen_style(kfont_t opaque);

int set_pen_color(kfont_t opaque, color_t color);
color_t get_pen_color(kfont_t opaque);

kfont_pos_t get_line_height(kfont_t opaque);

kfont_pos_t get_glyph_width(kfont_t opaque);
kfont_pos_t get_glyph_bearing_y(kfont_t opaque);

int draw_font(kfont_t opaque, char* str);
int draw_font_n(kfont_t opaque, char* str, size_t len);

int get_textbox_info(kfont_t opaque, char* str, kfont_pos_t* width, kfont_pos_t* height, kfont_pos_t* x, kfont_pos_t* y);
int get_textbox_info_n(kfont_t opaque, char* str, size_t len, kfont_pos_t* width, kfont_pos_t* height, kfont_pos_t* x, kfont_pos_t* y);

int draw_glyph(kfont_t opaque, kfont_glyph_t glyph, kfont_dot_t width, kfont_dot_t height);

int write_paragraph(kfont_t font, kfont_pos_t x, kfont_pos_t y, kfont_pos_t width, int format, char* text);

#endif // KOT_GRAPHICS_FONT