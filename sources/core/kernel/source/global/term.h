#ifndef _GLOBAL_TERM_H
#define _GLOBAL_TERM_H 1

#include <stdint.h>
#include <stddef.h>

void terminal_init(void* fb_base, uint64_t fb_width, uint64_t fb_height, uint64_t fb_pitch, void* image_base, size_t image_size);

void terminal_put_char(char c);

void terminal_print(const char* text);

void terminal_write(const char* text, size_t size);

#endif // _GLOBAL_HEAP_H
