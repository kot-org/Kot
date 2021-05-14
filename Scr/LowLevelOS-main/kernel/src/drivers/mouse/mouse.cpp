#include "mouse.h"

#define outb(x,y) outportb(x,y)
#define inb(x) inportb(x)

Mouse* GlobalMouse;

void Wait() {
    uint64_t timeout = 100000;
    while (timeout--){
        if ((inportb(0x64) & 0b10) == 0){
            return;
        }
    }
}

void WaitInput() {
    uint64_t timeout = 100000;
    while (timeout--){
        if (inportb(0x64) & 0b1){
            return;
        }
    }
}

void Mouse::Send(int val) {
    Wait();
    outb(0x64, 0xD4);
    Wait();
    outb(0x60, val);
}

int Mouse::Read() {
    WaitInput();
    return inb(0x60);
}

void Mouse::HandlePacket() {
    if(!PacketReady) return;
    PacketReady = false;
    bool xN=false,yN=false,xO=false,yO=false;

    if(Packet[0] & X_SIGN) 
        xN = true;

    if(Packet[0] & Y_SIGN) 
        yN = true;

    if(Packet[0] & X_OVER) 
        xO = true;

    if(Packet[0] & Y_OVER) 
        yO = true;

    if(!xN) {
        state.X += Packet[1];
        if(xO)
            state.X += 255;
    } else {
        Packet[1] = 256-Packet[1];
        state.X -= Packet[1];
        if(xO)
            state.X -= 255;       
    }

    if(!yN) {
        state.Y -= Packet[2];
        if(yO)
            state.Y -= 255;
    } else {
        Packet[2] = 256-Packet[2];
        state.Y += Packet[2];
        if(yO)
            state.Y += 255;      
    }

    if(state.X < 0) state.X = 0;
    if(state.X > GlobalDisplay->getWidth()-1) state.X = GlobalDisplay->getWidth()-1;

    if(state.Y < 0) state.Y = 0;
    if(state.Y > GlobalDisplay->getHeight()-1) state.Y = GlobalDisplay->getHeight()-1;   

    if(Packet[0] & BTN_LEFT)
        state.ButtonLeft = true;
    else 
        state.ButtonLeft = false;

    if(Packet[0] & BTN_MIDDLE)
        state.ButtonMiddle = true;
    else 
        state.ButtonMiddle = false;
    
    if(Packet[0] & BTN_RIGHT)
        state.ButtonRight = true;
    else 
        state.ButtonRight = false;
}

bool skipPacket = true;
void Mouse::Handle(uint8_t data) {
    HandlePacket();
    if(skipPacket) {skipPacket = false;return;}
    switch(Cycle){
        case 0:
            if (PacketReady) break;
            Packet[0] = data;
            Cycle++;
            break;
        case 1:
            if (PacketReady) break;
            Packet[1] = data;
            Cycle++;
            break;
        case 2:
            if (PacketReady) break;
            Packet[2] = data;
            PacketReady = true;
            Cycle = 0;
            break;
    }
}

void Mouse::Init() {
    outb(0x64, 0xA8);

    Wait();
    outb(0x64, 0x20);
    WaitInput();
    uint8_t status = inb(0x60);
    status |= 0b10;
    Wait();
    outb(0x64, 0x60);
    Wait();
    outb(0x60, status);

    Send(0xF6);
    Read();

    Send(0xF4);
    Read();
}