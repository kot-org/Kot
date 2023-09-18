#ifndef _GLOBAL_CONSOLE_H
#define _GLOBAL_CONSOLE_H

#include <boot/limine.h>

#define DEFAULT_BG_COLOR 0x333333
#define DEFAULT_FG_COLOR 0xEEEEEE

void console_set_bg_color(uint32_t bg);
void console_set_fg_color(uint32_t fg);

void console_init(void* fb_base, uint64_t fb_width, uint64_t fb_height, uint64_t fb_pitch, uint8_t fb_bpp);

void console_putchar(char c);
void console_print(const char* str);

#endif // _GLOBAL_CONSOLE_H