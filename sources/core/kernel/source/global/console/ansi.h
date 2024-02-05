#ifndef ANSI_H
#define ANSI_H

#include <stdint.h>

#define ANSI_START      '['
#define ANSI_END        'm'
#define ANSI_SEPARATOR  ';'

// todo ?? (ex: [0;..m | [1;..m] | ...)
#define STYLE_REGULAR   '0'
#define STYLE_BOLD      '1'

#define ANSI_RESET      '0'
#define ANSI_FG         '3'
#define ANSI_BG         '4'
#define ANSI_BRIGHT_FG  '9'
#define ANSI_BRIGHT_BG  '1' // or 10

#define COLOR_BLACK     '0'
#define COLOR_RED       '1'
#define COLOR_GREEN     '2'
#define COLOR_YELLOW    '3'
#define COLOR_BLUE      '4'
#define COLOR_PURPLE    '5'
#define COLOR_CYAN      '6'
#define COLOR_WHITE     '7'

uint16_t ansi_read(const char* code);

#endif // ANSI_H