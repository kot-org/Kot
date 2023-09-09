#ifndef _GLOBAL_CONSOLE_H
#define _GLOBAL_CONSOLE_H

#define DEFAULT_BG_COLOR 0x000000
#define DEFAULT_FG_COLOR 0xEEEEEE

void console_init(void* fb_base, uint64_t fb_width, uint64_t fb_height, uint64_t fb_pitch, uint8_t fb_bpp);

void console_putchar(char c);
void console_print(const char* str);

#endif // _GLOBAL_CONSOLE_H