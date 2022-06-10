#pragma once

#include <kot/sys.h>
#include <keyboard.h>
#include <mouse.h>


#define PS2_IRQ_KEYBOARD 0x1
#define PS2_IRQ_MOUSE 0xC

#define PS2_DATA 0x60
#define PS2_STATUS 0x64
#define PS2_COMMAND 0x64

extern kprocess_t self;

int main(int argc, char* argv[]);