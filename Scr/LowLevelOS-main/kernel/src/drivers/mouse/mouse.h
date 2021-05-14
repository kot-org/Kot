#pragma once
#include "../../io/ports.h"
#include "../display/displaydriver.h"

#define X_SIGN 0b00010000
#define Y_SIGN 0b00100000
#define X_OVER 0b01000000
#define Y_OVER 0b10000000

#define BTN_LEFT   0b00000001
#define BTN_MIDDLE 0b00000100
#define BTN_RIGHT  0b00000010

struct MouseState {
    int X;
    int Y;
    int ButtonLeft;
    int ButtonMiddle;
    int ButtonRight;
};

class Mouse {
public:
    MouseState state;

    int Cycle = 0;
    int Packet[4];
    bool PacketReady = false;
    void Init();
    void Send(int value);
    int Read();

    void Handle(uint8_t data);
    void HandlePacket();
};

extern Mouse* GlobalMouse;