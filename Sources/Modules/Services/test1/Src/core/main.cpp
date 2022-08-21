#include <core/main.h>

extern "C" int main(int argc, char* argv[]){
    Printlog("[Test1] Hello world");

    uintptr_t addressReceive = getFreeAlihnedSpace(sizeof(graphics_t));
    GetControllerUISD(ControllerTypeEnum_Graphics, &addressReceive, true);

    Printlog((char*)addressReceive);
    Printlog("[Test1] UISD response");
}