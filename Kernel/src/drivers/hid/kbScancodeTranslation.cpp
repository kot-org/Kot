#include "kbScancodeTranslation.h"

namespace AZERTYKeyboard{

    const char ASCIITable[] = {
         0 ,  0 , '1', '2',
        '3', '4', '5', '6',
        '7', '8', '9', '0',
        '-', '=',  0 ,  0 ,
        'a', 'z', 'e', 'r',
        't', 'y', 'u', 'i',
        'o', 'p', '[', ']',
         0 ,  0 , 'q', 's',
        'd', 'f', 'g', 'h',
        'j', 'k', 'l', 'm',
        '\'','`',  0 , '\\',
        'w', 'x', 'c', 'v',
        'b', 'n', ';', ',',
        '.', '/',  0 , '*',
         0 , ' '
    };

    char Translate(uint8_t scancode, bool uppercase){
        if(scancode > 58) return 0;
        if(uppercase){
            return ASCIITable[scancode] - 32; //offset between the lower case and upper case pointer, to translate low case to upper case
        }else{
            return ASCIITable[scancode];
        }        
    }
}