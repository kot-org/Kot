#include <core/main.h>


extern "C" int main(int argc, char* argv[]){
    Printlog("[HID] Loading service");
    InitialiseServer();
    Printlog("[HID] Service loaded");

    return KSUCCESS;
}