#pragma once

#include <stdint.h>
#include <stddef.h>

struct CharacterMapping {
    const char* table;
    size_t size;
};

#define LeftShift 0x2A
#define RightShift 0x36
#define Enter 0x1C
#define BackSpace 0x0E
#define Spacebar 0x39
#define CursorStart 0xE0
#define CursorUp 0x48
#define CursorLeft 0x4B
#define CursorRight 0x4D
#define CursorDown 0x50


namespace USQwertyKeyboard {
    char translate(uint8_t scancode, bool shift);
}

namespace JP109Keyboard {
    char translate(uint8_t scancode, bool shift);
}

extern char(*KeyboardMapFunction)(uint8_t, bool);