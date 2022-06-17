#include <keyboard/keyboard.h>

static char qwerty[] = "\0\e1234567890-=\b\tqwertyuiop[]\n\0asdfghjkl;'`\0\\zxcvbnm,./\0*\0 ";
static char azerty[] = "\0\e1234567890-=\b\tazertyuiop[]\n\0qsdfghjkl;'`\0\\wxcvbnm,./\0*\0 ";


KResult KeyboardInitialize(){
    for(int i = 0; i < PS2_PORT_NUMBER; i++){
        if(PS2Ports[i].Type == PS2_TYPE_KEYBOARD && PS2Ports[i].IsPresent){
            Printlog("[PS2] Keyboard device found");
            PS2Port_t* KeyboardPS2Port = &PS2Ports[i];

            if(KeyboardPS2Port->PortNumber == 0){
                PS2SendDataPort1(0xF6);
                PS2SendDataPort1(0xF4);
            }else if(KeyboardPS2Port->PortNumber == 1){
                PS2SendDataPort2(0xF6);
                PS2SendDataPort2(0xF4);
            }
            PS2WaitOutput();
            PS2GetData();

            kthread_t KeyboardThread = NULL;
            Sys_CreatThread(self, (uintptr_t)&KeyboardHandler, PriviledgeDriver, NULL, &KeyboardThread);
            Sys_Event_Bind(NULL, KeyboardThread, IRQ_START + KeyboardPS2Port->IRQ, false);
            
        }
    }
    
    return KSUCCESS;
}

void KeyboardHandler(enum EventType type, void* data){
    uint8_t scancode = PS2GetData();
    if(scancode < 0x80){
        char key[1];
        *key = (char)qwerty[scancode];
        Sys_Logs(key, 1);
    }
    kthread_t KeyboardThread = NULL;
    SYS_GetThreadKey(&KeyboardThread);
    SYS_Exit(KeyboardThread, KSUCCESS);
}