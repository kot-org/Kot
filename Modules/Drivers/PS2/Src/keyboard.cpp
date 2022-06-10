#include <keyboard.h>

kthread_t KeyboardThread = NULL;

KResult KeyboardInitialize(){
    Sys_CreatThread(self, (uintptr_t)&KeyboardHandler, PriviledgeDriver, NULL, &KeyboardThread);
    Sys_Event_Bind(NULL, KeyboardThread, IRQ_START + PS2_IRQ_KEYBOARD);

    return KSUCCESS;
}

void KeyboardHandler(enum EventType type, void* data){
    char key[1];
    *key = (char)IoRead8(PS2_DATA);
    Sys_Logs(key, 1);
    SYS_Exit(KeyboardThread, KSUCCESS);
}