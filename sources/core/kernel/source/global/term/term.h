#ifndef TERM_H
#define TERM_H

#include "image.h"

#ifdef __cplusplus
extern "C" {
#endif

extern void *alloc_mem(size_t size);
extern void free_mem(void *ptr, size_t size);
extern void *memcpy(void *dest, const void *src, size_t len);
extern void *memset(void *dest, int ch, size_t n);

#define FONT_GLYPHS 256

#define TERM_TABSIZE 8
#define MAX_ESC_VALUES 16

#define CHARSET_DEFAULT 0
#define CHARSET_DEC_SPECIAL 1

#define DEFAULT_ANSI_COLOURS { 0x00000000, 0x00AA0000, 0x0000AA00, 0x00AA5500, 0x000000AA, 0x00AA00AA, 0x0000AAAA, 0x00AAAAAA }
#define DEFAULT_ANSI_BRIGHT_COLOURS { 0x00555555, 0x00FF5555, 0x0055FF55, 0x00FFFF55, 0x005555FF, 0x00FF55FF, 0x0055FFFF, 0x00FFFFFF }

#define DEFAULT_BACKGROUND 0x00000000 // Black
#define DEFAULT_FOREGROUND 0x00AAAAAA // Grey

#define DEFAULT_MARGIN 64
#define DEFAULT_MARGIN_GRADIENT 4

struct term_t;
typedef void (*callback_t)(struct term_t*, uint64_t, uint64_t, uint64_t, uint64_t);
typedef size_t fixedp6;

static inline size_t fixedp6_to_int(fixedp6 value)
{
    return value / 64;
}
static inline fixedp6 int_to_fixedp6(size_t value)
{
    return value * 64;
}

enum callbacks
{
    TERM_CB_DEC = 10,
    TERM_CB_BELL = 20,
    TERM_CB_PRIVATE_ID = 30,
    TERM_CB_STATUS_REPORT = 40,
    TERM_CB_POS_REPORT = 50,
    TERM_CB_KBD_LEDS = 60,
    TERM_CB_MODE = 70,
    TERM_CB_LINUX = 80
};

enum term_type
{
    NOT_READY,
    VBE,
#if defined(__i386__) || defined(__x86_64__)
    TEXTMODE
#endif
};

struct framebuffer_t
{
    uintptr_t address;
    uint64_t width;
    uint64_t height;
    uint64_t pitch;
};

struct font_t
{
    uintptr_t address;
    uint8_t width;
    uint8_t height;
    uint8_t spacing;
    uint8_t scale_x;
    uint8_t scale_y;
};

struct style_t
{
    uint32_t ansi_colours[8];
    uint32_t ansi_bright_colours[8];
    uint32_t background;
    uint32_t foreground;
    uint16_t margin;
    uint16_t margin_gradient;
};

struct background_t
{
    struct image_t *background;
    enum image_style style;
    uint32_t backdrop;
};

struct term_context
{
    bool control_sequence;
    bool csi;
    bool escape;
    bool rrr;
    bool discard_next;
    bool bold;
    bool reverse_video;
    bool dec_private;
    bool insert_mode;
    uint64_t code_point;
    size_t unicode_remaining;
    uint8_t g_select;
    uint8_t charsets[2];
    size_t current_charset;
    size_t escape_offset;
    size_t esc_values_i;
    size_t saved_cursor_x;
    size_t saved_cursor_y;
    size_t current_primary;
    size_t scroll_top_margin;
    size_t scroll_bottom_margin;
    uint32_t esc_values[MAX_ESC_VALUES];

    bool saved_state_bold;
    bool saved_state_reverse_video;
    size_t saved_state_current_charset;
    size_t saved_state_current_primary;
};

struct gterm_t;
#if defined(__i386__) || defined(__x86_64__)
struct tterm_t;
#endif

struct term_t
{
    struct term_context context;
    struct gterm_t *gterm;
#if defined(__i386__) || defined(__x86_64__)
    struct tterm_t *tterm;
#endif

    bool bios;
    bool initialised;

    enum term_type term_backend;
    size_t rows, cols;

    size_t tab_size;
    bool autoflush;

    callback_t callback;
};

void term_init(struct term_t *term, callback_t callback, bool bios, size_t tabsize);
void term_deinit(struct term_t *term);
void term_reinit(struct term_t *term);
void term_vbe(struct term_t *term, struct framebuffer_t frm, struct font_t font, struct style_t style, struct background_t back);
#if defined(__i386__) || defined(__x86_64__)
void term_textmode(struct term_t *term);
#endif
void term_notready(struct term_t *term);
void term_putchar(struct term_t *term, uint8_t c);
void term_write(struct term_t *term, const char *buf, size_t count);
void term_sgr(struct term_t *term);
void term_dec_private_parse(struct term_t *term, uint8_t c);
void term_linux_private_parse(struct term_t *term);
void term_mode_toggle(struct term_t *term, uint8_t c);
void term_control_sequence_parse(struct term_t *term, uint8_t c);
void term_escape_parse(struct term_t *term, uint8_t c);

void term_raw_putchar(struct term_t *term, uint8_t c);
void term_clear(struct term_t *term, bool move);
void term_enable_cursor(struct term_t *term);
bool term_disable_cursor(struct term_t *term);
void term_set_cursor_pos(struct term_t *term, size_t x, size_t y);
void term_get_cursor_pos(struct term_t *term, size_t *x, size_t *y);
void term_set_text_fg(struct term_t *term, size_t fg);
void term_set_text_bg(struct term_t *term, size_t bg);
void term_set_text_fg_bright(struct term_t *term, size_t fg);
void term_set_text_bg_bright(struct term_t *term, size_t bg);
void term_set_text_fg_rgb(struct term_t *term, uint32_t fg);
void term_set_text_bg_rgb(struct term_t *term, uint32_t bg);
void term_set_text_fg_default(struct term_t *term);
void term_set_text_bg_default(struct term_t *term);
bool term_scroll_disable(struct term_t *term);
void term_scroll_enable(struct term_t *term);
void term_move_character(struct term_t *term, size_t new_x, size_t new_y, size_t old_x, size_t old_y);
void term_scroll(struct term_t *term);
void term_revscroll(struct term_t *term);
void term_swap_palette(struct term_t *term);
void term_save_state(struct term_t *term);
void term_restore_state(struct term_t *term);
void term_double_buffer_flush(struct term_t *term);
uint64_t term_context_size(struct term_t *term);
void term_context_save(struct term_t *term, uint64_t ptr);
void term_context_restore(struct term_t *term, uint64_t ptr);
void term_full_refresh(struct term_t *term);

#ifdef __cplusplus
}
#endif

#endif // TERM_H