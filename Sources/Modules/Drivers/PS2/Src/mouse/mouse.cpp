#include <mouse/mouse.h>

uint8_t MouseMaxCycles = 0;
uint8_t MousePacket[4];
uint8_t MouseCycle = 0;

MouseData_t* MouseData;
PS2Port_t* MousePS2Port;
parameters_t* MouseEventParameters;
kthread_t MouseThread = NULL;

KResult MouseInitalize(){
    MouseData = (MouseData_t*)malloc(sizeof(MouseData_t));
    for(int i = 0; i < PS2_PORT_NUMBER; i++){
        if(PS2Ports[i].IsPresent){
            if(PS2Ports[i].Type == PS2_TYPE_MOUSE
            || PS2Ports[i].Type == PS2_TYPE_MOUSE_SCROLL
            || PS2Ports[i].Type == PS2_TYPE_MOUSE_5BUTTONS){
                Printlog("[PS2] Mouse device found");

                MousePS2Port = &PS2Ports[i];
                Sys_Event_Bind(NULL, InterruptThreadHandler, IRQ_START + MousePS2Port->IRQ, false);

                MousePS2Port->PS2SendDataPort(0xF6);
                MousePS2Port->PS2SendDataPort(0xF4);
                IRQRedirectionsArray[MousePS2Port->PortNumber] = MouseHandler;

                PS2WaitOutput();
                PS2GetData();

                // Identify mouse type
                EnableMouseScroll();
                EnableMouse5Buttons();

                MouseData->mousePortType = mousePortTypePS2;

                Sys_Event_Create(&MouseData->onMouseStateChanged);
                
                MouseEventParameters = (parameters_t*)malloc(sizeof(parameters_t));

                MouseData->IsInitialized = true;
                
                break; // Enable only one mouse
            }            
        }
    }

    return KSUCCESS;
} 

void MouseSetRate(uint8_t rate, uint8_t port){
    if(port == 0){
        PS2SendDataPort1(0xF3);
        PS2WaitOutput();
        PS2GetData();
        PS2SendDataPort1(100);
        PS2WaitOutput();
        PS2GetData();
    }else if(port == 1){
        PS2SendDataPort2(0xF3);
        PS2WaitOutput();
        PS2GetData();
        PS2SendDataPort2(100);
        PS2WaitOutput();
        PS2GetData();
    }
}

uint8_t MouseGetID(uint8_t port){
    if(port == 0){
        PS2SendDataPort1(0xF2);
        PS2WaitOutput();
        return PS2GetData();
    }else if(port == 1){
        PS2SendDataPort2(0xF2);
        PS2WaitOutput();
        return PS2GetData();
    }
}

void EnableMouseScroll(){
    MouseSetRate(200, MousePS2Port->PortNumber);
    MouseSetRate(100, MousePS2Port->PortNumber);
    MouseSetRate(80, MousePS2Port->PortNumber);
    if(MouseGetID(MousePS2Port->PortNumber) == 3){
        MouseMaxCycles = 4;
        MouseData->mouseType = mouseTypeScroll;
        Printlog("[PS2][Mouse] Scroll enabled");
    }else{
        MouseMaxCycles = 3;
        MousePS2Port->Type = mouseTypeGeneric;
    }
}

void EnableMouse5Buttons(){
    EnableMouseScroll();
    if(MouseData->mouseType != mouseTypeScroll) return;
    MouseSetRate(200, MousePS2Port->PortNumber);
    MouseSetRate(200, MousePS2Port->PortNumber);
    MouseSetRate(80, MousePS2Port->PortNumber);
    if(MouseGetID(MousePS2Port->PortNumber) == 4){
        MouseMaxCycles = 4;
        MouseData->mouseType = mouseTypeScroll5Buttons;
        Printlog("[PS2][Mouse] 5 buttons enabled");
    }else{
        MouseData->mouseType = mouseTypeScroll;
    }
}

void MouseParser(uint8_t data){
    if(data & (1 << 3) == 0){
        return;
    }

    MousePacket[MouseCycle] = data;
    MouseCycle++;

    if(MouseCycle == MouseMaxCycles){
        MouseCycle = 0;

        if(MousePacket[PacketGlobalInfo] & (1 << 6)){
            MousePacket[PacketXPosition] = 0xff;
        }

        if(MousePacket[PacketGlobalInfo] & (1 << 7)){
            MousePacket[PacketYPosition] = 0xff;
        }

        if(MousePacket[PacketGlobalInfo] & (1 << 4)){
            if(MouseData->xAxisOffset < MousePacket[PacketXPosition]){
                MouseData->xAxisOffset = 0;
            }
            MouseData->xAxisOffset -= MousePacket[PacketXPosition];      
        }else{
            MouseData->xAxisOffset += MousePacket[PacketXPosition];  
        }
        if(MousePacket[PacketGlobalInfo] & (1 << 5)){
            if(MouseData->yAxisOffset < MousePacket[PacketYPosition]){
                MouseData->yAxisOffset = 0;
            }
            MouseData->yAxisOffset -= MousePacket[PacketYPosition];
        }else{
            MouseData->yAxisOffset += MousePacket[PacketYPosition];
        }

        if(MouseData->xAxisOffset < MouseData->xAxisOffsetMax){
            MouseData->xAxisOffset = MouseData->xAxisOffsetMax;
        }

        if(MouseData->yAxisOffset < MouseData->yAxisOffsetMax){
            MouseData->yAxisOffset = MouseData->yAxisOffsetMax;
        }       

        bool leftClick    = MousePacket[PacketGlobalInfo] & (1 << 0);
        bool rightClick   = MousePacket[PacketGlobalInfo] & (1 << 1);
        bool middleClick  = MousePacket[PacketGlobalInfo] & (1 << 2);
        bool button4Click = false;
        bool button5Click = false;
        int8_t zAxisOffset = 0;

        if(MouseData->mouseType == mouseTypeScroll){
            zAxisOffset = MousePacket[ExtendedInfos];
        }else if(MouseData->mouseType == mouseTypeScroll5Buttons){
            zAxisOffset = (MousePacket[ExtendedInfos] & 0b1111);
            button4Click = MousePacket[PacketGlobalInfo] & (1 << 4);
            button5Click = MousePacket[PacketGlobalInfo] & (1 << 5);
        }

        /* Absolute position */
        MouseEventParameters->Parameter0 = MouseData->xAxisOffset;
        MouseEventParameters->Parameter1 = MouseData->yAxisOffset;

        /* Relative position */
        MouseEventParameters->Parameter2 = (int64_t)MousePacket[PacketXPosition] | ((1 & (MousePacket[PacketGlobalInfo] & (1 << 4))) << 63);
        MouseEventParameters->Parameter3 = (int64_t)MousePacket[PacketYPosition] | ((1 & (MousePacket[PacketGlobalInfo] & (1 << 5))) << 63);

        /* Buttons status */
        MouseEventParameters->Parameter4 |= leftClick << 0;
        MouseEventParameters->Parameter4 |= rightClick << 1;
        MouseEventParameters->Parameter4 |= middleClick << 2;
        MouseEventParameters->Parameter4 |= button4Click << 3;
        MouseEventParameters->Parameter4 |= button5Click << 4;

        if(leftClick){
            Printlog("OK");
        }

        Sys_Event_Trigger(MouseData->onMouseStateChanged, MouseEventParameters);
    }
}

KResult MouseHandler(uint8_t data){
    MouseParser(data);
    return KSUCCESS;
}