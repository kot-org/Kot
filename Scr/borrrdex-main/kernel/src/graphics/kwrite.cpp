#include "kwrite.h"
#include "drivers/polltty.h"
#include "arch/x86_64/io/io.h"
#include <cstdarg>

static void write_char(char s, void* context) {
    int character = (int)s;
    polltty_putchar(character);
}

void kwrite(const char* s) {
    while(*s) {
        write_char(*s, nullptr);
        s++;
    }
}

void kprintf(const char* fmt, ...) {
    va_list va;
    va_start(va, fmt);
    __vprintf(write_char, nullptr, fmt, va);
    va_end(va);
}