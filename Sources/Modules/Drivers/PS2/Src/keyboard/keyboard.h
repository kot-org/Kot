#pragma once

#include <core/main.h>

extern struct PS2Port_t* KeyboardPS2Port;

KResult KeyboardInitialize();

KResult KeyboardHandler(uint8_t data);