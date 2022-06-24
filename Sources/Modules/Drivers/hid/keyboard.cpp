#include "keyboard.h"

bool isShiftPressed;

void HandleKeyboard(uint8_t scancode){

    switch(scancode){
        case LeftShift: //Upper press
            isShiftPressed = true;
            break;
        case LeftShift + 0x80: //Upper case release
            isShiftPressed = false;
            break;
        case RightShift: //Upper press
            isShiftPressed = true;
            break;
        case RightShift  + 0x80: //Upper case release
            isShiftPressed = false;
            break;
        case Enter: 
            globalGraphics->Next();
            break;
        case Backspace: 
            globalGraphics->ClearChar();
            break;
        case SpaceBar:
            globalGraphics->PutChar('  ');
            break;
        default:
            char ascii = AZERTYKeyboard::Translate(scancode, isShiftPressed);
            
            if(ascii != 0){
                globalGraphics->PutChar(ascii);
            }
            break;
    }
    globalGraphics->Update();
}