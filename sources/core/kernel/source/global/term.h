#ifndef _GLOBAL_TERM_H
#define _GLOBAL_TERM_H 1

#include <stdint.h>
#include <stddef.h>

void init_term(void* fb_base, uint64_t fb_width, uint64_t fb_height, uint64_t fb_pitch, void* image_base, size_t image_size);

void put_char_terminal(char c);

void print_terminal(const char* text);

void write_terminal(const char* text, size_t size);

#endif // _GLOBAL_HEAP_H
