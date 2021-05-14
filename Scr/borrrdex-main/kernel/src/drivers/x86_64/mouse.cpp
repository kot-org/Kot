#include "mouse.h"
#include "arch/x86_64/io/io.h"
#include "arch/x86_64/interrupt/interrupt.h"
#include "arch/x86_64/pic.h"
#include "string.h"
#include "graphics/BasicRenderer.h"

extern "C" void __mouse_irq_handler();

union MousePacket {
    uint8_t val;
    struct {
        bool lmb:1;
        bool mmb:1;
        bool rmb:1;
        bool alwaysOne:1;
        bool xNegative:1;
        bool yNegative:1;
        bool xOverflow:1;
        bool yOverflow:1;
    };

    MousePacket(uint8_t val) {
        MousePacket::val = val;
    }
};

void ps2_mouse_wait_output() {
    uint64_t timeout = 100000;
    while(timeout-- && (port_read_8(0x64) & 0x2)) {
        port_yield();
    }
}

void ps2_mouse_wait_input() {
    uint64_t timeout = 100000;
    while(timeout-- && !(port_read_8(0x64) & 0x1)) {
        port_yield();
    }
}

void ps2_mouse_write(uint8_t value) {
    ps2_mouse_wait_output();
    port_write_8(0x64, 0xD4);
    ps2_mouse_wait_output();
    port_write_8(0x60, value);
}

uint8_t ps2_mouse_read() {
    ps2_mouse_wait_input();
    return port_read_8(0x60);
}

extern "C" void ps2_mouse_init() {
    port_write_8(0x64, 0xA8);
    ps2_mouse_wait_output();
    port_write_8(0x64, 0x20);
    ps2_mouse_wait_input();

    uint8_t status = port_read_8(0x60);
    status |= 0x2;
    ps2_mouse_wait_output();
    port_write_8(0x64, 0x60);
    ps2_mouse_wait_output();
    port_write_8(0x60, status);

    ps2_mouse_write(0xF6);
    ps2_mouse_read();
    ps2_mouse_write(0xF4);
    ps2_mouse_read();

    interrupt_register(PIC_IRQ_PS2MOUSE, __mouse_irq_handler);
}

uint8_t mouseCycle = 0;
uint8_t mousePacket[4];
bool mousePacketReady = false;
Point MousePosition; 
extern "C" void ps2_mouse_handle(uint8_t data) {
    switch(mouseCycle) {
        case 0:
        {
            MousePacket mp(data);
            if(mousePacketReady || !mp.alwaysOne) {
                break;
            }

            mousePacket[0] = data;
            mouseCycle++;
            break;
        }
        case 1:
            if(mousePacketReady) {
                break;
            }

            mousePacket[1] = data;
            mouseCycle++;
            break;
        case 2:
            if(mousePacketReady) {
                break;
            }

            mousePacket[2] = data;
            mousePacketReady = true;
            mouseCycle = 0;
            break;
    }
}

extern "C" void mouse_handle() {
    uint8_t mouseData = port_read_8(0x60);
    ps2_mouse_handle(mouseData);
    pic_eoi(PIC_IRQ_PS2MOUSE);
}

extern "C" void ps2_mouse_process_packet() {
    if(!mousePacketReady) {
        return;
    }

    uint8_t mousePacketCopy[4];
    memcpy(mousePacketCopy, mousePacket, 4);
    mousePacketReady = false; 

    MousePacket mp(mousePacketCopy[0]);
    if(!mp.xNegative) {
        MousePosition.x += mousePacketCopy[1];
        if(mp.xOverflow) {
            MousePosition.x += 255;
        }
    } else {
        mousePacketCopy[1] = 256 - mousePacketCopy[1];
        MousePosition.x -= mousePacketCopy[1];
        if(mp.xOverflow) {
            MousePosition.x -= 255;
        }
    }

    if(!mp.yNegative) {
        MousePosition.y -= mousePacketCopy[2];
        if(mp.yOverflow) {
            MousePosition.y -= 255;
        }
    } else {
        mousePacketCopy[2] = 256 - mousePacketCopy[2];
        MousePosition.y += mousePacketCopy[2];
        if(mp.yOverflow) {
            MousePosition.y += 255;
        }
    }

    if(MousePosition.x < 0) {
        MousePosition.x = 0;
    } else if(MousePosition.x > GlobalRenderer->Width() - 8) {
        MousePosition.x = GlobalRenderer->Width() - 8;
    }

    if(MousePosition.y < 0) {
        MousePosition.y = 0;
    } else if(MousePosition.y > GlobalRenderer->Height() - 16) {
        MousePosition.y = GlobalRenderer->Height() - 16;
    }

    GlobalRenderer->PutCharAt('a', MousePosition.x, MousePosition.y);
}