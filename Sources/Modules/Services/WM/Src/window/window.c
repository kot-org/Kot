#include "core/main.h"
#include "draw/draw.h"
#include "window/window.h"

void create_window(uint32_t x, uint32_t y, uint32_t w, uint32_t h)
{
    create_rect(x, y, w, h, 0x1A1A1A);
}

void clear_screen(void)
{
    memset(fb_addr, 0x00, fb_size);
}