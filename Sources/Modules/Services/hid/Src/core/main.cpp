#include <core/main.h>


extern "C" int main(int argc, char* argv[]){
    kot_Printlog("[HID] Loading service");
    InitialiseServer();
    kot_Printlog("[HID] Service loaded");

    return KSUCCESS;
}