#include <main.h>

kprocess_t self;

int main(int argc, char* argv[]){
    /* Initialize PS2 drivers */
    SYS_GetProcessKey(&self);

    /* Initialize keyboard */
    KeyboardInitialize();

    /* Initialize mouse */
    MouseInitalize();

    return KSUCCESS;
}