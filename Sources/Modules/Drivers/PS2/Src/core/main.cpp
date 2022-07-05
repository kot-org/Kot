#include <core/main.h>

kprocess_t self;

PS2Port_t PS2Ports[2];

kthread_t InterruptThreadHandler = NULL;

IRQRedirections IRQRedirectionsArray[2];

extern "C" int main(int argc, char* argv[]){
    Printlog("[PS2] Initialization ...");
    /* Initialize PS2 drivers */
    SYS_GetProcessKey(&self);
    Sys_CreateThread(self, (uintptr_t)&PS2InterruptHandler, PriviledgeDriver, NULL, &InterruptThreadHandler);

    KResult status = KSUCCESS;

    /* Test ports */
    status = PortsInitalize();
    if(status != KSUCCESS) return status;

    /* Initialize keyboard */
    status = KeyboardInitialize();
    if(status != KSUCCESS) return status;

    /* Initialize mouse */
    status = MouseInitalize();
    if(status != KSUCCESS) return status;
    
    /* Clear buffer */
    PS2GetData();
    CallIPC("Test");

    Printlog("[PS2] Driver intialized successfully");
    return KSUCCESS;
}

KResult PortsInitalize(){
    uint8_t status = PS2ConfigurationGet();

    PS2SendCommand(0xAD); // disable port 1
    PS2SendCommand(0xA7); // disable port 2
    PS2GetData();

    /* Test PS2 controller */
    PS2SendCommand(0xAA);
    PS2WaitOutput();
    if(PS2GetData() != 0x55){
        return KFAIL;
    }

    PS2SendCommand(0xAB); // self test port 1
    PS2WaitOutput();
    PS2Ports[0].IsPresent = PS2GetData() == 0x0;

    PS2SendCommand(0xA9); // self test port 2
    PS2WaitOutput();
    PS2Ports[1].IsPresent = PS2GetData() == 0x0;

    PS2Ports[0].Type = PS2_TYPE_UNKNOW;
    PS2Ports[0].IRQ = PS2_IRQ_PORT1;
    PS2Ports[0].PortNumber = 0;

    if(PS2Ports[0].IsPresent){
        Printlog("[PS2] Port 1 is present");
        status |= (1 << 0);

        PS2SendCommand(0xAE); // enable port 1
        PS2SendDataPort1(0xFF); // reset
        PS2WaitOutput();
        PS2Ports[0].IsPresent = PS2GetData() == 0xFA;

        PS2SendDataPort1(0xF5);
        PS2WaitOutput();
        PS2GetData();
        PS2SendDataPort1(0xF2);
        PS2WaitOutput();
        PS2GetData();
        PS2WaitOutput();
        uint8_t response0 = PS2GetData();
        PS2WaitOutput();
        uint8_t response1 = PS2GetData();
        PS2SendDataPort1(0xF4);
        switch(response0){
            case 0x0:
                PS2Ports[0].Type = PS2_TYPE_MOUSE;
                break;
            case 0x3: 
                PS2Ports[0].Type = PS2_TYPE_MOUSE_SCROLL;
                break;
            case 0x4:
                PS2Ports[0].Type = PS2_TYPE_MOUSE_5BUTTONS;
                break;
            default:
                PS2Ports[0].Type = PS2_TYPE_KEYBOARD;
                break;
        }            
    }

    PS2Ports[1].Type = PS2_TYPE_UNKNOW;
    PS2Ports[1].IRQ = PS2_IRQ_PORT2;
    PS2Ports[1].PortNumber = 1;

    if(PS2Ports[1].IsPresent){
        Printlog("[PS2] Port 2 is present");
        status |= (1 << 1);

        PS2SendDataPort2(0xFF); // reset
        PS2SendCommand(0xA8); // enable port 2
        PS2WaitOutput();
        PS2Ports[1].IsPresent = PS2GetData() == 0xFA;

        PS2SendDataPort2(0xF5);
        PS2WaitOutput();
        PS2GetData();
        PS2SendDataPort2(0xF2);
        PS2WaitOutput();
        PS2GetData();
        PS2WaitOutput();
        uint8_t response0 = PS2GetData();
        PS2WaitOutput();
        uint8_t response1 = PS2GetData();
        PS2SendDataPort2(0xF4);
        switch(response0){
            case 0x0:
                PS2Ports[1].Type = PS2_TYPE_MOUSE;
                break;
            case 0x3: 
                PS2Ports[1].Type = PS2_TYPE_MOUSE_SCROLL;
                break;
            case 0x4:
                PS2Ports[1].Type = PS2_TYPE_MOUSE_5BUTTONS;
                break;
            default:
                PS2Ports[1].Type = PS2_TYPE_KEYBOARD;
                break;
        }     
    }
 
    PS2ConfigurationSet(status);     

    return KSUCCESS;
}

void PS2InterruptHandler(uint8_t interrupt){
    uint8_t IRQ = interrupt - 0x20;
    uint8_t data = (uint8_t)PS2GetData();
    Printlog("Clear ps2 Buffer");
    
    switch(IRQ){
        case PS2_IRQ_PORT1:
            IRQRedirectionsArray[0](data);
            break;
        case PS2_IRQ_PORT2:
            IRQRedirectionsArray[1](data);
            break;
    }
    Sys_Event_Close();
}

void PS2SendCommand(uint8_t command){
    IoWrite8(PS2_COMMAND, command);
    PS2WaitInput();
}


uint8_t PS2GetStatus(){
    return IoRead8(PS2_STATUS);
}

void PS2SendData(uint8_t data){
    IoWrite8(PS2_DATA, data);
    PS2WaitInput();
}


uint8_t PS2GetData(){
    return IoRead8(PS2_DATA);
}

void PS2WaitOutput(){
    for(uint32_t timeout = 0; timeout < 0xFFFFF; timeout++){
        if(PS2GetStatus() & 0b1){
            return;
        }
    }
}

void PS2WaitInput(){
    for(uint32_t timeout = 0; timeout < 0xFFFFF; timeout++){
        if(!(PS2GetStatus() & 0b10)){
            return;
        }
    }
}

void PS2SendDataPort1(uint8_t data){
    PS2SendData(data);
    PS2WaitInput();
}

void PS2SendDataPort2(uint8_t data){
    PS2SendCommand(0xD4);
    PS2SendData(data);
    PS2WaitInput();
}

uint8_t PS2ConfigurationGet(){
    PS2SendCommand(0x20);
    PS2WaitOutput();
    return PS2GetData();
}

void PS2ConfigurationSet(uint8_t data){
    PS2SendCommand(0x60);
    PS2SendCommand(data);
}

uint8_t PS2ControllerOutputGet(){
    PS2SendCommand(0xD0);
    return PS2GetData();
}

void PS2ControllerOutputSet(uint8_t data){
    PS2SendCommand(0xD1);
    PS2SendCommand(data);
}

void PS2Port_t::PS2SendDataPort(uint8_t data){
    if(PortNumber == 0){
        PS2SendDataPort1(data);
    }else if(PortNumber == 1){
        PS2SendDataPort2(data);
    }
}