#ifndef GTERM_H
#define GTERM_H

#include "term.h"

#ifdef __cplusplus
extern "C" {
#endif

struct gterm_char
{
    uint32_t c;
    uint32_t fg;
    uint32_t bg;
};

struct gterm_queue_item
{
    size_t x, y;
    struct gterm_char c;
};

struct gterm_context
{
    uint32_t text_fg;
    uint32_t text_bg;
    bool cursor_status;
    size_t cursor_x;
    size_t cursor_y;
    bool scroll_enabled;

    uint32_t saved_state_text_fg;
    uint32_t saved_state_text_bg;
    size_t saved_state_cursor_x;
    size_t saved_state_cursor_y;
};

struct gterm_t
{
    struct framebuffer_t framebuffer;
    volatile uint32_t *framebuffer_addr;

    struct term_t *term;

    size_t font_width;
    size_t font_height;
    size_t font_bytes;
    size_t glyph_width;
    size_t glyph_height;

    size_t font_scale_x;
    size_t font_scale_y;

    size_t offset_x, offset_y;

    uint8_t *font_bits;
    size_t font_bool_size;
    bool *font_bool;

    uint32_t ansi_colours[8];
    uint32_t ansi_bright_colours[8];
    uint32_t default_fg, default_bg;

    struct image_t *background;

    size_t bg_canvas_size;
    uint32_t *bg_canvas;

    size_t rows;
    size_t cols;
    size_t margin;
    size_t margin_gradient;

    size_t grid_size;
    size_t queue_size;
    size_t map_size;

    struct gterm_char *grid;

    struct gterm_queue_item *queue;
    size_t queue_i;

    struct gterm_queue_item **map;

    struct gterm_context context;

    size_t old_cursor_x;
    size_t old_cursor_y;
};

void gterm_save_state(struct gterm_t *gterm);
void gterm_restore_state(struct gterm_t *gterm);
void gterm_swap_palette(struct gterm_t *gterm);
bool gterm_scroll_disable(struct gterm_t *gterm);
void gterm_scroll_enable(struct gterm_t *gterm);
void gterm_revscroll(struct gterm_t *gterm);
void gterm_scroll(struct gterm_t *gterm);
void gterm_clear(struct gterm_t *gterm, bool move);
void gterm_enable_cursor(struct gterm_t *gterm);
bool gterm_disable_cursor(struct gterm_t *gterm);
void gterm_set_cursor_pos(struct gterm_t *gterm, size_t x, size_t y);
void gterm_get_cursor_pos(struct gterm_t *gterm, size_t *x, size_t *y);
void gterm_move_character(struct gterm_t *gterm, size_t new_x, size_t new_y, size_t old_x, size_t old_y);
void gterm_set_text_fg(struct gterm_t *gterm, size_t fg);
void gterm_set_text_bg(struct gterm_t *gterm, size_t bg);
void gterm_set_text_fg_bright(struct gterm_t *gterm, size_t fg);
void gterm_set_text_bg_bright(struct gterm_t *gterm, size_t bg);
void gterm_set_text_fg_rgb(struct gterm_t *gterm, uint32_t fg);
void gterm_set_text_bg_rgb(struct gterm_t *gterm, uint32_t bg);
void gterm_set_text_fg_default(struct gterm_t *gterm);
void gterm_set_text_bg_default(struct gterm_t *gterm);
void gterm_double_buffer_flush(struct gterm_t *gterm);
void gterm_putchar(struct gterm_t *gterm, uint8_t c);

bool gterm_init(struct gterm_t *gterm, struct term_t *term, struct framebuffer_t frm, struct font_t font, struct style_t style, struct background_t back);
void gterm_deinit(struct gterm_t *gterm);

uint64_t gterm_context_size(struct gterm_t *gterm);
void gterm_context_save(struct gterm_t *gterm, uint64_t ptr);
void gterm_context_restore(struct gterm_t *gterm, uint64_t ptr);
void gterm_full_refresh(struct gterm_t *gterm);

#ifdef __cplusplus
}
#endif

#endif // GTERM_H