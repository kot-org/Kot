#include <mouse/mouse.h>

uint8_t MouseMaxCycles = 0;
uint8_t MousePacket[4];
uint8_t MouseCycle = 0;

MouseData_t* MouseData;
PS2Port_t* MousePS2Port;

KResult MouseInitalize(){
    MouseData = (MouseData_t*)malloc(sizeof(MouseData_t));
    for(int i = 0; i < PS2_PORT_NUMBER; i++){
        if(PS2Ports[i].IsPresent){
            if(PS2Ports[i].Type == PS2_TYPE_MOUSE
            || PS2Ports[i].Type == PS2_TYPE_MOUSE_SCROLL
            || PS2Ports[i].Type == PS2_TYPE_MOUSE_5BUTTONS){
                Printlog("[PS2] Mouse device found");
                MousePS2Port = &PS2Ports[i];

                if(MousePS2Port->PortNumber == 0){
                    PS2SendDataPort1(0xF6);
                    PS2SendDataPort1(0xF4);
                }else if(MousePS2Port->PortNumber == 1){
                    PS2SendDataPort2(0xF6);
                    PS2SendDataPort2(0xF4);
                }
                PS2WaitOutput();
                PS2GetData();


                // Identify mouse type
                atomicAcquire(&MouseData->lock, 0);
                EnableMouseScroll();
                EnableMouse5Buttons();

                MouseData->mousePortType = mousePortTypePS2;

                kthread_t MouseThread = NULL;
                Sys_CreatThread(self, (uintptr_t)&MouseHandler, PriviledgeDriver, NULL, &MouseThread);
                Sys_Event_Bind(NULL, MouseThread, IRQ_START + MousePS2Port->IRQ, false);

                atomicUnlock(&MouseData->lock, 0);
                
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
        atomicAcquire(&MouseData->lock, 0);

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

        MouseData->leftClick    |= MousePacket[PacketGlobalInfo] & (1 << 0);
        MouseData->rightClick   |= MousePacket[PacketGlobalInfo] & (1 << 1);
        MouseData->middleClick  |= MousePacket[PacketGlobalInfo] & (1 << 2);

        if(MouseData->mouseType == mouseTypeScroll){
            MouseData->zAxisOffset += MousePacket[ExtendedInfos];
        }else if(MouseData->mouseType == mouseTypeScroll5Buttons){
            MouseData->zAxisOffset += (MousePacket[ExtendedInfos] & 0b1111);
            MouseData->button4Click |= MousePacket[PacketGlobalInfo] & (1 << 4);
            MouseData->button5Click |= MousePacket[PacketGlobalInfo] & (1 << 5);
        }

        atomicUnlock(&MouseData->lock, 0);
    }
}

void MouseHandler(enum EventType type){
    uint8_t data = (uint8_t)PS2GetData();
    MouseParser(data);

    // close event
    kthread_t MouseThread = NULL;
    SYS_GetThreadKey(&MouseThread);
    SYS_Exit(MouseThread, KSUCCESS);
}