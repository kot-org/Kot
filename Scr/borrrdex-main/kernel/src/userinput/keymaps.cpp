#include "keymaps.h"

inline char translate(uint8_t scancode, const char table[], size_t size)
{
    if(scancode >= size) {
        return 0;
    }

    return table[scancode];
}

namespace USQwertyKeyboard {
    const char NormalMap[] = {
         0 ,  0 , '1', '2', 
        '3', '4', '5', '6',
        '7', '8', '9', '0',
        '-', '=',  0 ,  0 ,
        'q', 'w', 'e', 'r',
        't', 'y', 'u', 'i',
        'o', 'p', '[', ']',
        0 ,  0 , 'a', 's',
        'd', 'f', 'g', 'h',
        'j', 'k', 'l', ';',
        '\'','`',  0 , '\\',
        'z', 'x', 'c', 'v',
        'b', 'n', 'm', ',',
        '.', '/',  0 , '*',
        0 , ' '
    };

    const char ShiftMap[] = {
         0 ,  0 , '!', '@', 
        '#', '$', '%', '^',
        '&', '*', '(', ')',
        '_', '+',  0 ,  0 ,
        'Q', 'W', 'E', 'R',
        'T', 'Y', 'U', 'I',
        'O', 'P', '{', '}',
         0 ,  0 , 'A', 'S',
        'D', 'F', 'G', 'H',
        'J', 'K', 'L', ':',
        '"', '~',  0 , '|',
        'Z', 'X', 'C', 'V',
        'B', 'N', 'M', '<',
        '>', '?',  0 , '*',
        0 , ' '
    };

    char translate(uint8_t scancode, bool shift)
    {
        return ::translate(scancode, shift ? ShiftMap : NormalMap, 58);
    }
}

namespace JP109Keyboard
{
    const char NormalMap[] = {
         0 ,  0 , '1', '2', 
        '3', '4', '5', '6',
        '7', '8', '9', '0',
        '-', '^',  0 ,  0 ,
        'q', 'w', 'e', 'r',
        't', 'y', 'u', 'i',
        'o', 'p', '@', '[',
         0 ,  0 , 'a', 's',
        'd', 'f', 'g', 'h',
        'j', 'k', 'l', ';',
        ':', ']',  0 , ']',
        'z', 'x', 'c', 'v',
        'b', 'n', 'm', ',',
        '.', '/',  0 , '\\',
         0 , ' ',  0 ,  0 ,
         0 ,  0 ,  0 ,  0 ,
         0 ,  0 ,  0 ,  0 ,
         0 ,  0 ,  0 ,  0 ,
         0 ,  0 ,  0 ,  0 ,
         0 ,  0 ,  0 ,  0 ,
         0 ,  0 ,  0 ,  0 ,
         0 ,  0 ,  0 ,  0 ,
         0 ,  0 ,  0 ,  0 ,
         0 ,  0 ,  0 ,  0 ,
         0 ,  0 ,  0 ,  0 ,
         0 ,  0 ,  0 ,  0 ,
         0 ,  0 ,  0 ,  0 ,
         0 ,  0 ,  0 ,  0 ,
         0 ,  0 ,  0 ,  '\\',
         0 ,  0 ,  0 ,  0 ,
         0 ,  0 ,  0 ,  0 ,
         0 , '\\'
    };

    const char ShiftMap[] = {
         0 ,  0 , '!', '"', 
        '#', '$', '%', '&',
        '\'','(', ')',  0 ,
        '=', '~',  0 ,  0 ,
        'Q', 'W', 'E', 'R',
        'T', 'Y', 'U', 'I',
        'O', 'P', '`', '{',
         0 ,  0 , 'A', 'S',
        'D', 'F', 'G', 'H',
        'J', 'K', 'L', '+',
        '*', '}',  0 , '}',
        'Z', 'X', 'C', 'V',
        'B', 'N', 'M', '<',
        '>', '?',  0 , '_',
         0 , ' ',  0 ,  0 ,
         0 ,  0 ,  0 ,  0 ,
         0 ,  0 ,  0 ,  0 ,
         0 ,  0 ,  0 ,  0 ,
         0 ,  0 ,  0 ,  0 ,
         0 ,  0 ,  0 ,  0 ,
         0 ,  0 ,  0 ,  0 ,
         0 ,  0 ,  0 ,  0 ,
         0 ,  0 ,  0 ,  0 ,
         0 ,  0 ,  0 ,  0 ,
         0 ,  0 ,  0 ,  0 ,
         0 ,  0 ,  0 ,  0 ,
         0 ,  0 ,  0 ,  0 ,
         0 ,  0 ,  0 ,  0 ,
         0 ,  0 ,  0 , '_',
         0 ,  0 ,  0 ,  0 ,
         0 ,  0 ,  0 ,  0 ,
         0 , '|'
    };

    char translate(uint8_t scancode, bool shift)
    {
        return ::translate(scancode, shift ? ShiftMap : NormalMap, 126);
    }
}

char(*KeyboardMapFunction)(uint8_t, bool);
