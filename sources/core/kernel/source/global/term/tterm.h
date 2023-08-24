#if defined(__i386__) || defined(__x86_64__)

#ifndef TTERM_H
#define TTERM_H

#include "term.h"

#ifdef __cplusplus
extern "C" {
#endif

#define VD_COLS (80 * 2)
#define VD_ROWS 25
#define VIDEO_BOTTOM (VD_COLS * VD_ROWS) - 1

struct tterm_context
{
    size_t cursor_offset;
    bool cursor_status;
    uint8_t text_palette;
    bool scroll_enabled;

    uint8_t saved_state_text_palette;
    size_t saved_state_cursor_offset;
};

struct tterm_t
{
    volatile uint8_t *video_mem;
    uint8_t *back_buffer;
    uint8_t *front_buffer;

    size_t old_cursor_offset;

    struct tterm_context context;
    struct term_t *term;
};

void tterm_init(struct tterm_t *tterm, struct term_t *term);
void tterm_putchar(struct tterm_t *tterm, uint8_t c);
void tterm_clear(struct tterm_t *tterm, bool move);
void tterm_enable_cursor(struct tterm_t *tterm);
bool tterm_disable_cursor(struct tterm_t *tterm);
void tterm_set_cursor_pos(struct tterm_t *tterm, size_t x, size_t y);
void tterm_get_cursor_pos(struct tterm_t *tterm, size_t *x, size_t *y);
void tterm_set_text_fg(struct tterm_t *tterm, size_t fg);
void tterm_set_text_bg(struct tterm_t *tterm, size_t bg);
void tterm_set_text_fg_bright(struct tterm_t *tterm, size_t fg);
void tterm_set_text_bg_bright(struct tterm_t *tterm, size_t bg);
void tterm_set_text_fg_default(struct tterm_t *tterm);
void tterm_set_text_bg_default(struct tterm_t *tterm);
bool tterm_scroll_disable(struct tterm_t *tterm);
void tterm_scroll_enable(struct tterm_t *tterm);
void tterm_move_character(struct tterm_t *tterm, size_t new_x, size_t new_y, size_t old_x, size_t old_y);
void tterm_scroll(struct tterm_t *tterm);
void tterm_revscroll(struct tterm_t *tterm);
void tterm_swap_palette(struct tterm_t *tterm);
void tterm_save_state(struct tterm_t *tterm);
void tterm_restore_state(struct tterm_t *tterm);
void tterm_double_buffer_flush(struct tterm_t *tterm);
uint64_t tterm_context_size(struct tterm_t *tterm);
void tterm_context_save(struct tterm_t *tterm, uint64_t ptr);
void tterm_context_restore(struct tterm_t *tterm, uint64_t ptr);
void tterm_full_refresh(struct tterm_t *tterm);

#ifdef __cplusplus
}
#endif

#endif // TTERM_H

#endif