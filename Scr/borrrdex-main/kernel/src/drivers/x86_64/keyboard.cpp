#include "keyboard.h"
#include "userinput/keymaps.h"
#include "graphics/BasicRenderer.h"
#include "arch/x86_64/io/io.h"
#include "arch/x86_64/interrupt/interrupt.h"
#include "arch/x86_64/pic.h"

volatile int kb_code = -1;

extern "C" void __keyboard_irq_handler();

bool leftShift, rightShift, cursor;

void HandleCursor(uint8_t scancode) {
    cursor = false;
    switch(scancode) {
        case CursorUp:
            GlobalRenderer->Up();
            return;
        case CursorDown:
            GlobalRenderer->Down();
            return;
        case CursorLeft:
            GlobalRenderer->Left();
            return;
        case CursorRight:
            GlobalRenderer->Right();
            return;
    }
}

void HandleKeyboard(uint8_t scancode) {
    if(cursor) {
        HandleCursor(scancode);
        return;
    }

    switch(scancode) {
        case LeftShift:
            kb_code = 0;
            leftShift = true;
            return;
        case LeftShift + 0x80:
            leftShift = false;
            return;
        case RightShift:
            kb_code = 0;
            rightShift = true;
            return;
        case RightShift + 0x80:
            rightShift = false;
            return;
        case Enter:
            kb_code = '\n';
            GlobalRenderer->Next();
            return;
        case BackSpace:
            GlobalRenderer->ClearChar();
            return;
        case CursorStart:
            cursor = true;
            return;
    }

    char ascii = KeyboardMapFunction(scancode, leftShift || rightShift);
    if(ascii != 0) {
        kb_code = ascii;
        GlobalRenderer->PutChar(ascii);
    }
}

void keyboard_init() {
    interrupt_register(PIC_IRQ_KEYBOARD, __keyboard_irq_handler);
    interrupt_register(0xE4 - 0x20, __keyboard_irq_handler);
}

extern "C" void keyboard_handle() {
    uint8_t scancode = port_read_8(0x60);
    HandleKeyboard(scancode);
    pic_eoi(PIC_IRQ_KEYBOARD);
}

char keyboard_getkey() {
    int key = -1;
    while(key == -1) {
        key = kb_code;
    }

    kb_code = -1;
    return (char)key;
}