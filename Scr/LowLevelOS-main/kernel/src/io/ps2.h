#pragma once
#include <stdint.h>
#include "ports.h"

#define PORT_DATA 0x60
#define PORT_STATUS 0x64
#define PORT_COMMAND 0x64

class PS2Controller
{
public:
    void Wait();
    bool Init();
    void SendCommand(uint8_t data);
    void SendData(uint8_t data);
    uint8_t ReceiveData();
};