#pragma once
#include "../IO/IO.h"
#include "../math.h"
#include "../graphics.h"

#define PS2LeftButton   0b00000001
#define PS2MiddleButton 0b00000100
#define PS2RightButton  0b00000010

#define PS2XSign        0b00010000
#define PS2YSign        0b00100000
#define PS2Xoverflow    0b01000000
#define PS2Yoverflow    0b10000000

extern uint8_t MousePointer[];

void InitPS2Mouse();
void HandlePS2Mouse(uint8_t data);
void ProcessMousePacket();

extern Point MousePosition;