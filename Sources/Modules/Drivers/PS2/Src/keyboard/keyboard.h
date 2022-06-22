#pragma once

#include <core/main.h>

#define CMD_KEYBOARD_DISABLE 0xAD 
#define CMD_KEYBOARD_ENABLE 0xAE
KResult KeyboardInitialize();

KResult KeyboardHandler(uint8_t data);