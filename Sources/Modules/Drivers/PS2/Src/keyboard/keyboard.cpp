#include <keyboard/keyboard.h>

static char qwerty[] = "\0\e1234567890-=\b\tqwertyuiop[]\n\0asdfghjkl;'`\0\\zxcvbnm,./\0*\0 ";
static char azerty[] = "\0\e1234567890-=\b\tazertyuiop[]\n\0qsdfghjkl;'`\0\\wxcvbnm,./\0*\0 ";


PS2Port_t* KeyboardPS2Port;

uint8_t LedStateSaver = 0;

static uint64_t KeyboardLock;

KResult KeyboardInitialize(){
    for(int i = 0; i < PS2_PORT_NUMBER; i++){
        if(PS2Ports[i].Type == PS2_TYPE_KEYBOARD && PS2Ports[i].IsPresent){
            Printlog("[PS2] Keyboard device found");
            KeyboardPS2Port = &PS2Ports[i];
            KeyboardPS2Port->PS2SendDataPort(0xF6);
            KeyboardPS2Port->PS2SendDataPort(0xF4);
            IRQRedirectionsArray[KeyboardPS2Port->PortNumber] = KeyboardHandler;
            PS2WaitOutput();
            PS2GetData();

            Sys_Event_Bind(NULL, InterruptThreadHandler, IRQ_START + KeyboardPS2Port->IRQ, false);
            
            break;
        }
    }
    
    return KSUCCESS;
}

KResult KeyboardHandler(uint8_t data){
    if(data < 0x80){
        char key[1];
        *key = (char)qwerty[data];
        Sys_Logs(key, 1);
    }

    return KSUCCESS;
}

KResult KeyboardSetLedState(enum KeyboardLEDS LEDID, bool IsOn){
    atomicAcquire(&KeyboardLock, 0);
    LedStateSaver = WriteBit(LedStateSaver, LEDID, IsOn);
    KeyboardPS2Port->PS2SendDataPort(0xED);
    KeyboardPS2Port->PS2SendDataPort(LedStateSaver);
    atomicUnlock(&KeyboardLock, 0);
}