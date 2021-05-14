#include "keyboarddriver.h"

Keyboard* GlobalKeyboard;

const char ScanCodeSet1[] = {0,0,'1','2','3','4','5','6','7','8','9','0','-','=','\b',0,'q','w','e','r','t','y','u','i','o','p','[',']','\n',0,'a','s','d','f','g','h','j','k','l',';','\'','`',0,'\\','z','x','c','v','b','n','m',',','.','/',0,'*',0,' '};
const char ScanCodeUpperSet1[] = {0,0,'1','@','#','$','%','^','&','*','(',')','_','+','\b',0,'Q','W','E','R','T','Y','U','I','O','P','{','}','\n',0,'A','S','D','F','G','H','J','K','L',':','"','~',0,'|','Z','X','C','V','B','N','M','<','>','?',0,'*',0,' '};
const char ScanCodeCapsSet1[] = {0,0,'1','2','3','4','5','6','7','8','9','0','-','=','\b',0,'Q','W','E','R','T','Y','U','I','O','P','[',']','\n',0,'A','S','D','F','G','H','J','K','L',';','\'','`',0,'\\','Z','X','C','V','B','N','M',',','.','/',0,'*',0,' '};


//thanks for help, @borrrden

char Keyboard::Translate(uint8_t keycode,KeyState isUpperCase,KeyState isCaps) {
    // 0x3E > number of entries in scan code set!!
    if(keycode > /* 0x3E */ sizeof(ScanCodeSet1)-1) return 0; 
    if(isUpperCase == DOWN && keycode != KEY_SPACE)
        return ScanCodeUpperSet1[keycode];
    else if(isCaps == DOWN && keycode != KEY_SPACE)
        return ScanCodeCapsSet1[keycode]; //fixed caps and shift to print random symbols
    else if(isUpperCase == DOWN && isCaps)
        return ScanCodeSet1[keycode];
    else 
        return ScanCodeSet1[keycode];
}

void Keyboard::Handle(uint8_t keycode) {
    ReceivedIntrerupt = true;
    LastKeycode = (int)Translate(keycode,(KeyState)((int)LeftShift | (int)RightShift),CapsLock);
    switch (keycode)
    {
        case KEY_BACKSPACE:
            if(bufferIndex)
                buffer[--bufferIndex] = 0; 
            break;
        case KEY_LEFTSHIFT:
            LeftShift = DOWN;
            break;
        case KEY_RIGHTSHIFT:
            RightShift = DOWN;
            break;
        case KEY_LEFTSHIFT + KEYUP_OFFSET:
            LeftShift = UP;
            break;
        case KEY_RIGHTSHIFT + KEYUP_OFFSET:
            RightShift = UP;
            break;
        case KEY_CAPSLOCK:
            if(CapsLock == UP)
                CapsLock = DOWN;
            else
                CapsLock = UP;
            break;
        default:
            char translated = Translate(keycode,(KeyState)((int)LeftShift | (int)RightShift),CapsLock);
            if(translated)
                buffer[bufferIndex++] = translated;
            break;
    }
}

int kbhit() {
    if(ReceivedIntrerupt) {
        ReceivedIntrerupt = false;
        return 1;
    } else {
        return 0;
    }
}

int getch() {
    return LastKeycode;
}
