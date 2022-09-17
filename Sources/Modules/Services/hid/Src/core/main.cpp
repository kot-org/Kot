#include <core/main.h>


extern "C" int main(int argc, char* argv[]){
    Printlog("[HID] Loading device");
    InitialiseServer();
    Printlog("[HID] Device loaded");

    return KSUCCESS;
}