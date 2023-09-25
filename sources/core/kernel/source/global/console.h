#ifndef _GLOBAL_CONSOLE_H
#define _GLOBAL_CONSOLE_H

#include <stdint.h>
#include <impl/graphics.h>

#define DEFAULT_BG_COLOR 0x1B1B1B
#define DEFAULT_FG_COLOR 0xEEEEEE

void console_set_bg_color(uint32_t bg);
void console_set_fg_color(uint32_t fg);

void console_init(void);

void console_putchar(char c);
void console_print(const char* str);

#endif // _GLOBAL_CONSOLE_H