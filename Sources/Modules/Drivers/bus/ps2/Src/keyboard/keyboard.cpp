#include <keyboard/keyboard.h>


PS2Port_t* KeyboardPS2Port;
kot_arguments_t* KeyboardEventParameters;
kot_event_t KeyboardEvent;

uint8_t LedStateSaver = 0;

static uint64_t KeyboardLock;

KResult KeyboardInitialize(){
    for(uint8_t i = 0; i < PS2_PORT_NUMBER; i++){
        if(PS2Ports[i].Type == PS2_TYPE_KEYBOARD && PS2Ports[i].IsPresent){
            kot_Printlog("[BUS/PS2] Keyboard device found");

            KeyboardEventParameters = (kot_arguments_t*)malloc(sizeof(kot_arguments_t));
            KeyboardPS2Port = &PS2Ports[i];
            IRQRedirectionsArray[KeyboardPS2Port->PortNumber] = KeyboardHandler;

            KeyboardEvent = kot_GetKeyboardEvent();

            kot_Srv_System_BindIRQLine(KeyboardPS2Port->IRQ, InterruptThreadHandler[i], false, true);
            
            break;
        }
    }
    
    return KSUCCESS;
}

KResult KeyboardHandler(uint8_t data){
    KeyboardEventParameters->arg[0] = (uint64_t)data;
    kot_Sys_Event_Trigger(KeyboardEvent, KeyboardEventParameters);

    return KSUCCESS;
}

KResult KeyboardSetLedState(enum KeyboardLEDS LEDID, bool IsOn){
    atomicAcquire(&KeyboardLock, 0);
    BIT_SETV(LedStateSaver, LEDID, IsOn);
    KeyboardPS2Port->PS2SendDataPort(0xED);
    KeyboardPS2Port->PS2SendDataPort(LedStateSaver);
    atomicUnlock(&KeyboardLock, 0);
}