#pragma once

#include <core/main.h>
#include <kot/modules/uisd/hid/keyboard.h>

extern struct PS2Port_t* KeyboardPS2Port;

enum KeyboardLEDS{
    KeyboardLEDSScrollLock  = 0,
    KeyboardLEDSNumberLock  = 1,
    KeyboardLEDSCapsLock    = 2,
};

KResult KeyboardInitialize();

KResult KeyboardHandler(uint8_t data);

KResult KeyboardSetLedState(enum KeyboardLEDS LEDID, bool IsOn);