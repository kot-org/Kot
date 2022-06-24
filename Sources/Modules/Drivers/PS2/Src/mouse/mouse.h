#pragma once

#include <core/main.h>
#include <kot/heap.h>
#include <kot/modules/uisd/hid/mouse.h>

#define PacketGlobalInfo    0x0
#define PacketXPosition     0x1
#define PacketYPosition     0x2
#define ExtendedInfos       0x3

extern struct PS2Port_t* MousePS2Port;

KResult MouseInitalize();

void MouseWait();
void MouseWrite(uint8_t value);
void MouseSetRate(uint8_t rate, uint8_t port);
uint8_t MouseGetID(uint8_t port);

void EnableMouseScroll();
void EnableMouse5Buttons();

KResult MouseHandler(uint8_t data);