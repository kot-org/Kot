#ifndef _UISD_HID_KEYBOARD_H
#define _UISD_HID_KEYBOARD_H 1

#include <kot/types.h>
#include <kot/atomic.h>

enum keyboardPortType{
    keyboardPortTypePS2        = 0x0,
    keyboardPortTypeUSB        = 0x1,
    keyboardPortTypeWireless   = 0x2,
    keyboardPortTypeUnknow     = 0xff,
};

enum keyboardType{
    keyboardTypeGeneric        = 0x0,
    keyboardTypeNumpad         = 0x1,  
    keyboardTypeUnknow         = 0xff,
};

struct KeyboardData_t{
    enum keyboardPortType keyboardPortType;
    enum keyboardType keyboardType;

    event_t onKeyboardStateChanged;

    bool IsInitialized;
}__attribute__((packed));

#endif