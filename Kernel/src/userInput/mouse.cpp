#include "mouse.h"

uint8_t MouseCycle = 0;
uint8_t MousePacket[4];
bool MousePacketReady = false;
Point MousePosition;
Point MousePositionOld;

uint8_t MousePointer[] = {
    0b11111111, 0b11100000, 
    0b11111111, 0b10000000, 
    0b11111110, 0b00000000, 
    0b11111100, 0b00000000, 
    0b11111000, 0b00000000, 
    0b11110000, 0b00000000, 
    0b11100000, 0b00000000, 
    0b11000000, 0b00000000, 
    0b11000000, 0b00000000, 
    0b10000000, 0b00000000, 
    0b10000000, 0b00000000, 
    0b00000000, 0b00000000, 
    0b00000000, 0b00000000, 
    0b00000000, 0b00000000, 
    0b00000000, 0b00000000, 
    0b00000000, 0b00000000, 
};

void MouseWait(){
    uint64_t timeout = 100000;
    while(timeout--){
        if((IoRead8(0x64) & 0b10) == 0){
            return;
        }        
    }
}

void MouseWaitInput(){
    uint64_t timeout = 100000;
    while(timeout--){
        if(IoRead8(0x64) & 0b1){
            return;
        }        
    }
}

void MouseWrite(uint8_t value){
    MouseWait();
    IoWrite8(0x64, 0xD4);
    MouseWait();
    IoWrite8(0x60, value);
}

uint8_t MouseRead(){
    MouseWait();
    return IoRead8(0x60);
}

void HandlePS2Mouse(uint8_t data){
    ProcessMousePacket();
    static bool skip = true;
    if(skip) {
        skip = false;
        return;
    }


    switch(MouseCycle){
        case 0:
            if(MousePacketReady) break;
            if((data & 0b00001000) == 0) break;
            MousePacket[0] = data;
            MouseCycle++;
            break;
        case 1:
            if(MousePacketReady) break;
            MousePacket[1] = data;
            MouseCycle++;
            break;
        case 2:
            if(MousePacketReady) break;
            MousePacket[2] = data;
            MousePacketReady = true;
            MouseCycle = 0;
            break;
    }
}

void ProcessMousePacket(){
    if(!MousePacketReady){
        return;
    }else{
        bool xNegative, yNegative, xOverflow, yOverflow;
        if(MousePacket[0] & PS2XSign){
            xNegative = true;
        }else{
            xNegative = false;
        }
        if(MousePacket[0] & PS2YSign){
            yNegative = true;
        }else{
            yNegative = false;
        }
        if(MousePacket[0] & PS2Xoverflow){
            xOverflow = true;
        }else{
            xOverflow = false;
        }
        if(MousePacket[0] & PS2Yoverflow){
            yOverflow = true;
        }else{
            yOverflow = false;
        }

        /* Position */

        /* X */
        if(!xNegative){
            MousePosition.X += MousePacket[1];
            if(xOverflow){
                MousePosition.X += 255;
            }
        }else{
            MousePacket[1] = 256 - MousePacket[1];
            MousePosition.X -= MousePacket[1];
            if(xOverflow){
                MousePosition.X -= 255;
            }           
        }

        /* Y */
        // invert point Y value
        if(!yNegative){
            MousePosition.Y -= MousePacket[2];
            if(yOverflow){
                MousePosition.Y -= 255;
            }
        }else{
            MousePacket[2] = 256 - MousePacket[2];
            MousePosition.Y += MousePacket[2];
            if(yOverflow){
                MousePosition.Y += 255;
            }           
        }

        if(MousePosition.X < 0) MousePosition.X = 0;
        if(MousePosition.X > globalGraphics->framebuffer->Width - 1) MousePosition.X = globalGraphics->framebuffer->Width - 1;

        if(MousePosition.Y < 0) MousePosition.Y = 0;
        if(MousePosition.Y > globalGraphics->framebuffer->Height - 1) MousePosition.Y = globalGraphics->framebuffer->Height - 1;

        globalGraphics->ClearMouseCursor(MousePointer, MousePositionOld);
        globalGraphics->DrawOverlayMouseCursor(MousePointer, MousePosition, 0xffffffff);

        if(MousePacket[0] & PS2LeftButton){
            globalGraphics->PutChar('a', MousePosition.X, MousePosition.Y);
        }
        if(MousePacket[0] & PS2MiddleButton){
            uint32_t color = globalGraphics->Color;
            globalGraphics->Color = 0x00ff0000;
            globalGraphics->PutChar('a', MousePosition.X, MousePosition.Y);
            globalGraphics->Color = color;
        }
        if(MousePacket[0] & PS2RightButton){
            uint32_t color = globalGraphics->Color;
            globalGraphics->Color = 0x0000ff00;
            globalGraphics->PutChar('a', MousePosition.X, MousePosition.Y);
            globalGraphics->Color = color;
        }

        MousePacketReady = false;
        MousePositionOld = MousePosition;
        globalGraphics->Update();
    }

}

void InitPS2Mouse(){
    IoWrite8(0x64, 0xA8); //enabling the auxilary device - mouse
    MouseWait();
    IoWrite8(0x64, 0x20); //tells to the keyboard controller that we want to send a command to the mouse
    MouseWaitInput();
    uint8_t status = IoRead8(0x60);
    status |= 0b10;
    MouseWaitInput();
    IoWrite8(0x64, 0x60); 
    MouseWaitInput();
    IoWrite8(0x60, status);

    MouseWrite(0xF6);
    MouseRead();

    MouseWrite(0xF4);
    MouseRead();
}