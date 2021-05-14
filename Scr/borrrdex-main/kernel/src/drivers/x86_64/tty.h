#pragma once

#include <cstdint>

class Spinlock;

namespace tty {
    constexpr uint32_t VIDEO_MEMORY_BASE    = 0xB8000;
    constexpr uint8_t VIDEO_MEMORY_COLUMNS  = 80;
    constexpr uint8_t VIDEO_MEMORY_ROWS     = 25;

    constexpr uint8_t COLOR_BLACK       = 0x00;
    constexpr uint8_t COLOR_BLUE        = 0x01;
    constexpr uint8_t COLOR_GREEN       = 0x02;
    constexpr uint8_t COLOR_CYAN        = 0x03;
    constexpr uint8_t COLOR_RED         = 0x04;
    constexpr uint8_t COLOR_MAGENTA     = 0x05;
    constexpr uint8_t COLOR_BROWN       = 0x06;
    constexpr uint8_t COLOR_LGRAY       = 0x07;
    constexpr uint8_t COLOR_DGRAY       = 0x08;
    constexpr uint8_t COLOR_LBLUE       = 0x09;
    constexpr uint8_t COLOR_LGREEN      = 0x0A;
    constexpr uint8_t COLOR_LCYAN       = 0x0B;
    constexpr uint8_t COLOR_LRED        = 0x0C;
    constexpr uint8_t COLOR_LMAGENTA    = 0x0D;
    constexpr uint8_t COLOR_YELLOW      = 0x0E;
    constexpr uint8_t COLOR_WHITE       = 0x0F;
}

typedef struct {
    uint64_t cursor_x;
    uint64_t cursor_y;
    uint64_t text_color;
    Spinlock* slock;
} tty_t;

