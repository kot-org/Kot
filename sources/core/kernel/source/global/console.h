#ifndef _GLOBAL_CONSOLE_H
#define _GLOBAL_CONSOLE_H

#define DEFAULT_BG_COLOR 0x000000
#define DEFAULT_FG_COLOR 0xEEEEEE

void console_init(void);

void console_putchar(char c);
void console_print(const char* str);

#endif // _GLOBAL_CONSOLE_H