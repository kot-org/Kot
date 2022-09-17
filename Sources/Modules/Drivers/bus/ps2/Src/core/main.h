#pragma once

#include <kot/sys.h>
#include <kot/bits.h>
#include <kot/cstring.h>
#include <mouse/mouse.h>
#include <keyboard/keyboard.h>
#include <kot/uisd/srvs/hid.h>
#include <kot/uisd/srvs/system.h>

#define PS2_PORT_NUMBER 0x2

#define PS2_IRQ_PORT1 0x1
#define PS2_IRQ_PORT2 0xC

#define PS2_DATA 0x60
#define PS2_STATUS 0x64
#define PS2_COMMAND 0x64

#define PS2_ACK 0xFA
#define PS2_RESEND 0xFE

#define PS2_TYPE_UNKNOW 0x0
#define PS2_TYPE_MOUSE 0x1
#define PS2_TYPE_MOUSE_SCROLL 0x2
#define PS2_TYPE_MOUSE_5BUTTONS 0x3
#define PS2_TYPE_KEYBOARD 0x4

#define PS2_STATU_OUPUTBUFFER       1 << 0
#define PS2_STATU_INPUTBUFFER       1 << 1
#define PS2_STATU_SYSTEMFALG        1 << 2
#define PS2_STATU_COMMANDDATA       1 << 3
#define PS2_STATU_KEYBOARDLOCK      1 << 4
#define PS2_STATU_SECONDBUFFERFULL  1 << 5
#define PS2_STATU_TIMEOUTERROR      1 << 6
#define PS2_STATU_PARTYERROR        1 << 7

extern process_t self;
extern struct PS2Port_t PS2Ports[2];
extern thread_t InterruptthreadHandler;
typedef KResult (*IRQRedirections)(uint8_t data);
extern IRQRedirections IRQRedirectionsArray[2];


struct PS2Port_t{
    bool IsPresent;
    uint8_t Type;
    uint8_t IRQ;
    uint8_t PortNumber;

    void PS2SendDataPort(uint8_t data); 
};

extern "C" int main(int argc, char* argv[]);

KResult PortsInitalize();

void PS2InterruptHandler(uint8_t interrupt);

void PS2SendCommand(uint8_t command);
uint8_t PS2GetStatus();
void PS2SendData(uint8_t data);
uint8_t PS2GetData();

void PS2WaitOutput();
void PS2WaitInput();

void PS2SendDataPort1(uint8_t data);
void PS2SendDataPort2(uint8_t data);

uint8_t PS2ConfigurationGet();
void PS2ConfigurationSet(uint8_t data);

uint8_t PS2ControllerOutputGet();
void PS2ControllerOutputSet(uint8_t data);