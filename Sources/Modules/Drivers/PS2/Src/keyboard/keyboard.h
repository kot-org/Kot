#pragma once

#include <core/main.h>

extern struct PS2Port_t* KeyboardPS2Port;

enum KeyboardLEDS{
    ScrollLock  = 0,
    NumberLock  = 1,
    CapsLock    = 2,
};

KResult KeyboardInitialize();

KResult KeyboardHandler(uint8_t data);

KResult KeyboardSetLedState(enum KeyboardLEDS LEDID, bool IsOn);