#pragma once

#include <stddef.h>

int uart_init();
void uart_print(const char* s);
void uart_printn(const char* s, size_t len);

int uart_printf(const char* fmt, ...);