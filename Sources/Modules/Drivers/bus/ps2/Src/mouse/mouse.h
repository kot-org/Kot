#pragma once

#include <core/main.h>
#include <kot/heap.h>

#define PacketGlobalInfo    0x0
#define PacketXPosition     0x1
#define PacketYPosition     0x2
#define ExtendedInfos       0x3

extern struct PS2Port_t* MousePS2Port;

KResult MouseInitalize();

void MouseWait();
void MouseWrite(uint8_t value);
void MouseSetRate(uint8_t rate, struct PS2Port_t* Self);
uint8_t MouseGetID(struct PS2Port_t* Self);

void EnableMouseScroll(struct PS2Port_t* Self);
void EnableMouse5Buttons(struct PS2Port_t* Self);

KResult MouseHandler(uint8_t data);