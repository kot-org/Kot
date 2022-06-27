#include <keyboard/keyboard.h>


PS2Port_t* KeyboardPS2Port;
parameters_t* KeyboardEventParameters;
KeyboardData_t* KeyboardData;

uint8_t LedStateSaver = 0;

static uint64_t KeyboardLock;

KResult KeyboardInitialize(){
    KeyboardData = (KeyboardData_t*)malloc(sizeof(KeyboardData_t));
    for(int i = 0; i < PS2_PORT_NUMBER; i++){
        if(PS2Ports[i].Type == PS2_TYPE_KEYBOARD && PS2Ports[i].IsPresent){
            Printlog("[PS2] Keyboard device found");

            Sys_Event_Create(&KeyboardData->onKeyboardStateChanged);

            KeyboardEventParameters = (parameters_t*)malloc(sizeof(parameters_t));
            KeyboardPS2Port = &PS2Ports[i];
            Sys_Event_Bind(NULL, InterruptThreadHandler, IRQ_START + KeyboardPS2Port->IRQ, false);

            KeyboardPS2Port->PS2SendDataPort(0xF6);
            KeyboardPS2Port->PS2SendDataPort(0xF4);
            IRQRedirectionsArray[KeyboardPS2Port->PortNumber] = KeyboardHandler;
            PS2WaitOutput();
            PS2GetData();


            KeyboardData->IsInitialized = true;
            
            break;
        }
    }
    
    return KSUCCESS;
}

KResult KeyboardHandler(uint8_t data){
    KeyboardEventParameters->Parameter0 = (uint64_t)data;

    Sys_Event_Trigger(KeyboardData->onKeyboardStateChanged, KeyboardEventParameters);

    return KSUCCESS;
}

KResult KeyboardSetLedState(enum KeyboardLEDS LEDID, bool IsOn){
    atomicAcquire(&KeyboardLock, 0);
    LedStateSaver = WriteBit(LedStateSaver, LEDID, IsOn);
    KeyboardPS2Port->PS2SendDataPort(0xED);
    KeyboardPS2Port->PS2SendDataPort(LedStateSaver);
    atomicUnlock(&KeyboardLock, 0);
}