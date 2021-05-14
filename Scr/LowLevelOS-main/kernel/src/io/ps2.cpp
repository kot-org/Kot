#include "ps2.h"

void PS2Controller::Wait() {
    uint64_t timeout = 1000000;
    while (timeout--);
}

uint8_t PS2Controller::ReceiveData() {
    Wait();
    return inportb(PORT_DATA);
}

void PS2Controller::SendCommand(uint8_t data) {
    outportb(PORT_COMMAND,data);
    Wait();
}

void PS2Controller::SendData(uint8_t data) {
    outportb(PORT_DATA,data);
    Wait();
}

bool PS2Controller::Init() {
    //disable devices
    SendCommand(0xAD);
    SendCommand(0xA7);

    //flush data
    ReceiveData();

    //setting configuration
    SendCommand(0x20);
    SendCommand(0x60);
    SendData(0b01111111);   

    //self test
    SendCommand(0xAA);
    if(ReceiveData() != 0x55)
        return false;

    //setting configuration again
    SendCommand(0x20);
    SendCommand(0x60);
    SendData(0b01111111); 

    //enable devices
    SendCommand(0xAE);
    SendCommand(0xA8);

    //reset mouse
    SendCommand(0xD4);
    SendData(0xFF);

    //reset keyboard
    SendData(0xFF);

    //flush data again to be sure
    ReceiveData();

    return true;
}