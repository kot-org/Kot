#include "serial.h"
#include "io.h"

#include <cstdint>
#include <cstdarg>

constexpr uint16_t COM1 = 0x3f8;

int is_transmit_empty() {
   return port_read_8(COM1 + 5) & 0x20;
}

void putc(char c, __attribute__((unused)) void* ctx) {
    while(is_transmit_empty() == 0) {
        // no-op
    }

    port_write_8(COM1, c);
}

int uart_init() {
    port_write_8(COM1 + 1, 0x00);
    port_write_8(COM1 + 3, 0x80);
    port_write_8(COM1, 0x03);
    port_write_8(COM1 + 1, 0x00);
    port_write_8(COM1 + 3, 0x03);
    port_write_8(COM1 + 2, 0xC7);
    port_write_8(COM1 + 4, 0x0B);

    return 0;
}

void uart_print(const char* s) {
    uart_printn(s, __SIZE_MAX__);
}

void uart_printn(const char* s, size_t len) {
    const char* cur = s;
    while(*s && len--) {
        putc(*s++, NULL);
    }
}

int uart_printf(const char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);

    int ret = __vprintf(putc, NULL, fmt, ap);

    va_end(ap);
    return ret;
}