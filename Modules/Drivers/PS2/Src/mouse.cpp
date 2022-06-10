#include <mouse.h>

kthread_t MouseThread = NULL;

KResult MouseInitalize(){
    IoWrite8(PS2_COMMAND, 0xA8); 
    MouseWait();
    IoWrite8(PS2_COMMAND, 0x20); 
    MouseWait();
    IoWrite8(PS2_COMMAND, 0x60);
    MouseWait();
    MouseWrite(0xF6);
    MouseWait();
    MouseWrite(0xF4);
    MouseWait();

    Sys_CreatThread(self, (uintptr_t)&MouseHandler, PriviledgeDriver, NULL, &MouseThread);
    Sys_Event_Bind(NULL, MouseThread, IRQ_START + PS2_IRQ_MOUSE);

    return KSUCCESS;
}

void MouseWait(){
    while (IoRead8(PS2_COMMAND) & 0b10 != 0){
        return;
    }
}

void MouseWrite(uint8_t value){
    MouseWait();
    IoWrite8(PS2_COMMAND, 0xD4);
    MouseWait();
    IoWrite8(PS2_DATA, value);
}

void MouseHandler(enum EventType type, void* data){
    Sys_Logs("M", 1);
    SYS_Exit(MouseThread, KSUCCESS);
}