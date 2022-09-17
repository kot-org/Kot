#include <mouse/mouse.h>

uint8_t MouseMaxCycles = 0;
uint8_t MousePacket[4];
uint8_t MouseCycle = 0;

PS2Port_t* MousePS2Port;
uint8_t MouseType;
arguments_t* MouseEventParameters;
thread_t Mousethread = NULL;

event_t MouseRelativeEvent; // We don't use absolute for ps2

KResult MouseInitalize(){
    for(int i = 0; i < PS2_PORT_NUMBER; i++){
        if(PS2Ports[i].IsPresent){
            if(PS2Ports[i].Type == PS2_TYPE_MOUSE
            || PS2Ports[i].Type == PS2_TYPE_MOUSE_SCROLL
            || PS2Ports[i].Type == PS2_TYPE_MOUSE_5BUTTONS){
                Printlog("[PS2] Mouse device found");
                
                MouseEventParameters = (arguments_t*)malloc(sizeof(arguments_t));


                MousePS2Port = &PS2Ports[i];
                IRQRedirectionsArray[MousePS2Port->PortNumber] = MouseHandler;
                
                MouseRelativeEvent = GetMouseRelativeEvent();

                // Identify mouse type
                EnableMouseScroll(MousePS2Port);
                EnableMouse5Buttons(MousePS2Port);

                MouseType = mousePortTypePS2;
                
                Srv_System_BindIRQLine(MousePS2Port->IRQ, InterruptthreadHandler, false, true);

                break; // Enable only one mouse
            }            
        }
    }

    return KSUCCESS;
} 

void MouseSetRate(uint8_t rate, PS2Port_t* Self){
    Self->PS2SendDataPort(0xF3);
    PS2WaitOutput();
    PS2GetData();
    Self->PS2SendDataPort(rate);
    PS2WaitOutput();
    PS2GetData();
}

uint8_t MouseGetID(PS2Port_t* Self){
    Self->PS2SendDataPort(0xF2);
    PS2WaitOutput();
    return PS2GetData();
}

void EnableMouseScroll(PS2Port_t* Self){
    MouseSetRate(200, Self);
    MouseSetRate(100, Self);
    MouseSetRate(80, Self);
    if(MouseGetID(Self) == 3){
        MouseMaxCycles = 4;
        MouseType = mouseTypeScroll;
        Printlog("[PS2][Mouse] Scroll enabled");
    }else{
        MouseMaxCycles = 3;
        MouseType = mouseTypeGeneric;
    }
}

void EnableMouse5Buttons(PS2Port_t* Self){
    EnableMouseScroll(Self);
    if(MouseType != mouseTypeScroll) return;
    MouseSetRate(200, Self);
    MouseSetRate(200, Self);
    MouseSetRate(80, Self);
    if(MouseGetID(Self) == 4){
        MouseMaxCycles = 4;
        MouseType = mouseTypeScroll5Buttons;
        Printlog("[PS2][Mouse] 5 buttons enabled");
    }else{
        MouseType = mouseTypeScroll;
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

        bool leftClick    = MousePacket[PacketGlobalInfo] & (1 << 0);
        bool rightClick   = MousePacket[PacketGlobalInfo] & (1 << 1);
        bool middleClick  = MousePacket[PacketGlobalInfo] & (1 << 2);
        bool button4Click = false;
        bool button5Click = false;
        int8_t zAxisOffset = 0;

        if(MouseType == mouseTypeScroll){
            zAxisOffset = MousePacket[ExtendedInfos];
        }else if(MouseType == mouseTypeScroll5Buttons){
            zAxisOffset = (MousePacket[ExtendedInfos] & 0b1111);
            button4Click = MousePacket[PacketGlobalInfo] & (1 << 4);
            button5Click = MousePacket[PacketGlobalInfo] & (1 << 5);
        }

        /* Relative position */
        MouseEventParameters->arg[0] = (int64_t)MousePacket[PacketXPosition] | ((1 & (MousePacket[PacketGlobalInfo] & (1 << 4))) << 63); // add signed bit
        MouseEventParameters->arg[1] = (int64_t)MousePacket[PacketYPosition] | ((1 & (MousePacket[PacketGlobalInfo] & (1 << 5))) << 63); // add signed bit

        /* Buttons status */
        MouseEventParameters->arg[2] |= leftClick << 0;
        MouseEventParameters->arg[2] |= rightClick << 1;
        MouseEventParameters->arg[2] |= middleClick << 2;
        MouseEventParameters->arg[2] |= button4Click << 3;
        MouseEventParameters->arg[2] |= button5Click << 4;

        Sys_kevent_trigger(MouseRelativeEvent, MouseEventParameters);
    }
}

KResult MouseHandler(uint8_t data){
    MouseParser(data);
    
    return KSUCCESS;
}