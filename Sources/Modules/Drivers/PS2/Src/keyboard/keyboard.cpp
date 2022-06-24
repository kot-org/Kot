#include <keyboard/keyboard.h>

static char qwerty[] = "\0\e1234567890-=\b\tqwertyuiop[]\n\0asdfghjkl;'`\0\\zxcvbnm,./\0*\0 ";
static char azerty[] = "\0\e1234567890-=\b\tazertyuiop[]\n\0qsdfghjkl;'`\0\\wxcvbnm,./\0*\0 ";


PS2Port_t* KeyboardPS2Port;


KResult KeyboardInitialize(){
    for(int i = 0; i < PS2_PORT_NUMBER; i++){
        if(PS2Ports[i].Type == PS2_TYPE_KEYBOARD && PS2Ports[i].IsPresent){
            Printlog("[PS2] Keyboard device found");
            KeyboardPS2Port = &PS2Ports[i];

            if(KeyboardPS2Port->PortNumber == 0){
                PS2SendDataPort1(0xF6);
                PS2SendDataPort1(0xF4);
                IRQRedirectionsArray[0] = KeyboardHandler;
            }else if(KeyboardPS2Port->PortNumber == 1){
                PS2SendDataPort2(0xF6);
                PS2SendDataPort2(0xF4);
                IRQRedirectionsArray[1] = KeyboardHandler;
            }
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