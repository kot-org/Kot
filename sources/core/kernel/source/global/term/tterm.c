#if defined(__i386__) || defined(__x86_64__)

#include "tterm.h"
#include "term.h"

static void outb(uint16_t port, uint8_t val)
{
    asm volatile (
        ".att_syntax prefix\n\t"
        "outb %0, %1"
        :: "a"(val), "Nd"(port)
    );
}

void tterm_init(struct tterm_t *tterm, struct term_t *term)
{
    tterm->term = term;
    tterm->video_mem = (volatile uint8_t*)0xB8000;

    if (tterm->back_buffer == NULL)
        tterm->back_buffer = alloc_mem(VD_ROWS * VD_COLS);
    else
        memset(tterm->back_buffer, 0, VD_ROWS * VD_COLS);

    if (tterm->front_buffer == NULL)
        tterm->front_buffer = alloc_mem(VD_ROWS * VD_COLS);
    else
        memset(tterm->front_buffer, 0, VD_ROWS * VD_COLS);

    tterm->context.cursor_offset = 0;
    tterm->context.cursor_status = true;
    tterm->context.text_palette = 0x07;
    tterm->context.scroll_enabled = true;

    tterm_clear(tterm, false);

    term->rows = VD_ROWS;
    term->cols = VD_COLS / 2;

    outb(0x3D4, 0x0A);
    outb(0x3D5, 0x20);

    tterm_double_buffer_flush(tterm);
}

void tterm_putchar(struct tterm_t *tterm, uint8_t c)
{
    tterm->back_buffer[tterm->context.cursor_offset] = c;
    tterm->back_buffer[tterm->context.cursor_offset + 1] = tterm->context.text_palette;
    if (tterm->context.cursor_offset / VD_COLS == tterm->term->context.scroll_bottom_margin - 1 && tterm->context.cursor_offset % VD_COLS == VD_COLS - 2)
    {
        if (tterm->context.scroll_enabled)
        {
            tterm_scroll(tterm);
            tterm->context.cursor_offset -= tterm->context.cursor_offset % VD_COLS;
        }
    }
    else if (tterm->context.cursor_offset >= (VIDEO_BOTTOM - 1))
        tterm->context.cursor_offset -= tterm->context.cursor_offset % VD_COLS;
    else
        tterm->context.cursor_offset += 2;
}

void tterm_clear(struct tterm_t *tterm, bool move)
{
    for (size_t i = 0; i < VIDEO_BOTTOM; i += 2)
    {
        tterm->back_buffer[i] = ' ';
        tterm->back_buffer[i + 1] = tterm->context.text_palette;
    }
    if (move)
        tterm->context.cursor_offset = 0;
}

static void draw_cursor(struct tterm_t *tterm)
{
    uint8_t pal = tterm->back_buffer[tterm->context.cursor_offset + 1];
    tterm->video_mem[tterm->context.cursor_offset + 1] = ((pal & 0xF0) >> 4) | ((pal & 0x0F) << 4);
}

void tterm_enable_cursor(struct tterm_t *tterm)
{
    tterm->context.cursor_status = true;
}

bool tterm_disable_cursor(struct tterm_t *tterm)
{
    bool ret = tterm->context.cursor_status;
    tterm->context.cursor_status = false;
    return ret;
}

void tterm_set_cursor_pos(struct tterm_t *tterm, size_t x, size_t y)
{
    if (x >= VD_COLS / 2)
    {
        if ((int)x < 0) x = 0;
        else x = VD_COLS / 2 - 1;
    }
    if (y >= VD_ROWS)
    {
        if ((int)y < 0) y = 0;
        else y = VD_ROWS - 1;
    }
    tterm->context.cursor_offset = y * VD_COLS + x * 2;
}

void tterm_get_cursor_pos(struct tterm_t *tterm, size_t *x, size_t *y)
{
    *x = (tterm->context.cursor_offset % VD_COLS) / 2;
    *y = tterm->context.cursor_offset / VD_COLS;
}

static uint8_t ansi_colours[] = { 0, 4, 2, 6, 1, 5, 3, 7 };
void tterm_set_text_fg(struct tterm_t *tterm, size_t fg)
{
    tterm->context.text_palette = (tterm->context.text_palette & 0xF0) | ansi_colours[fg];
}

void tterm_set_text_bg(struct tterm_t *tterm, size_t bg)
{
    tterm->context.text_palette = (tterm->context.text_palette & 0x0F) | (ansi_colours[bg] << 4);
}

void tterm_set_text_fg_bright(struct tterm_t *tterm, size_t fg)
{
    tterm->context.text_palette = (tterm->context.text_palette & 0xF0) | (ansi_colours[fg] | (1 << 3));
}

void tterm_set_text_bg_bright(struct tterm_t *tterm, size_t bg)
{
    tterm->context.text_palette = (tterm->context.text_palette & 0x0F) | ((ansi_colours[bg] | (1 << 3)) << 4);
}

void tterm_set_text_fg_default(struct tterm_t *tterm)
{
    tterm->context.text_palette = (tterm->context.text_palette & 0xF0) | 7;
}

void tterm_set_text_bg_default(struct tterm_t *tterm)
{
    tterm->context.text_palette &= 0x0F;
}

bool tterm_scroll_disable(struct tterm_t *tterm)
{
    bool ret = tterm->context.scroll_enabled;
    tterm->context.scroll_enabled = false;
    return ret;
}

void tterm_scroll_enable(struct tterm_t *tterm)
{
    tterm->context.scroll_enabled = true;
}

void tterm_move_character(struct tterm_t *tterm, size_t new_x, size_t new_y, size_t old_x, size_t old_y)
{
    if (old_x >= VD_COLS / 2 || old_y >= VD_ROWS || new_x >= VD_COLS / 2 || new_y >= VD_ROWS)
        return;
    tterm->back_buffer[new_y * VD_COLS + new_x * 2] = tterm->back_buffer[old_y * VD_COLS + old_x * 2];
}

void tterm_scroll(struct tterm_t *tterm)
{
    for (size_t i = tterm->term->context.scroll_top_margin * VD_COLS; i < (tterm->term->context.scroll_bottom_margin - 1) * VD_COLS; i++)
        tterm->back_buffer[i] = tterm->back_buffer[i + VD_COLS];

    for (size_t i = (tterm->term->context.scroll_bottom_margin - 1) * VD_COLS; i < tterm->term->context.scroll_bottom_margin * VD_COLS; i += 2)
    {
        tterm->back_buffer[i] = ' ';
        tterm->back_buffer[i + 1] = tterm->context.text_palette;
    }
}

void tterm_revscroll(struct tterm_t *tterm)
{
    for (size_t i = (tterm->term->context.scroll_bottom_margin - 1) * VD_COLS - 2; ; i--)
    {
        tterm->back_buffer[i + VD_COLS] = tterm->back_buffer[i];
        if (i == tterm->term->context.scroll_top_margin * VD_COLS)
            break;
    }

    for (size_t i = tterm->term->context.scroll_top_margin * VD_COLS; i < (tterm->term->context.scroll_top_margin + 1) * VD_COLS; i += 2)
    {
        tterm->back_buffer[i] = ' ';
        tterm->back_buffer[i + 1] = tterm->context.text_palette;
    }
}

void tterm_swap_palette(struct tterm_t *tterm)
{
    tterm->context.text_palette = (tterm->context.text_palette << 4) | (tterm->context.text_palette >> 4);
}

void tterm_save_state(struct tterm_t *tterm)
{
    tterm->context.saved_state_text_palette = tterm->context.text_palette;
    tterm->context.saved_state_cursor_offset = tterm->context.cursor_offset;
}

void tterm_restore_state(struct tterm_t *tterm)
{
    tterm->context.text_palette = tterm->context.saved_state_text_palette;
    tterm->context.cursor_offset = tterm->context.saved_state_cursor_offset;
}

void tterm_double_buffer_flush(struct tterm_t *tterm)
{
    if (tterm->context.cursor_status)
        draw_cursor(tterm);

    if (tterm->context.cursor_offset != tterm->old_cursor_offset || tterm->context.cursor_status == false)
        tterm->video_mem[tterm->old_cursor_offset + 1] = tterm->back_buffer[tterm->old_cursor_offset + 1];

    for (size_t i = 0; i < VD_ROWS * VD_COLS; i++)
    {
        if (tterm->back_buffer[i] == tterm->front_buffer[i])
            continue;

        if (tterm->context.cursor_status && i == tterm->context.cursor_offset + 1)
            continue;

        tterm->front_buffer[i] = tterm->back_buffer[i];
        tterm->video_mem[i] = tterm->back_buffer[i];
    }

    if (tterm->context.cursor_status)
        tterm->old_cursor_offset = tterm->context.cursor_offset;
}

uint64_t tterm_context_size(struct tterm_t *tterm)
{
    (void)tterm;
    uint64_t ret = 0;

    ret += sizeof(struct tterm_context);
    ret += VD_ROWS * VD_COLS;

    return ret;
}

void tterm_context_save(struct tterm_t *tterm, uint64_t ptr)
{
    memcpy((void*)ptr, &tterm->context, sizeof(struct tterm_context));
    ptr += sizeof(struct tterm_context);

    memcpy((void*)ptr, tterm->front_buffer, VD_ROWS * VD_COLS);
}

void tterm_context_restore(struct tterm_t *tterm, uint64_t ptr)
{
    memcpy(&tterm->context, (void*)ptr, sizeof(struct tterm_context));
    ptr += sizeof(struct tterm_context);

    memcpy(tterm->front_buffer, (void*)ptr, VD_ROWS * VD_COLS);

    for (size_t i = 0; i < VD_ROWS * VD_COLS; i++)
    {
        tterm->video_mem[i] = tterm->front_buffer[i];
        tterm->back_buffer[i] = tterm->front_buffer[i];
    }

    if (tterm->context.cursor_status)
    {
        draw_cursor(tterm);
        tterm->old_cursor_offset = tterm->context.cursor_offset;
    }
}

void tterm_full_refresh(struct tterm_t *tterm)
{
    for (size_t i = 0; i < VD_ROWS * VD_COLS; i++)
    {
        tterm->video_mem[i] = tterm->front_buffer[i];
        tterm->back_buffer[i] = tterm->front_buffer[i];
    }

    if (tterm->context.cursor_status)
    {
        draw_cursor(tterm);
        tterm->old_cursor_offset = tterm->context.cursor_offset;
    }
}

#endif