#include <core/main.h>


int main(int argc, char* argv[]){
    kot_Printlog("[HID] Loading service");
    InitialiseServer();
    kot_Printlog("[HID] Service loaded");

    kot_Sys_Close(KSUCCESS); /* Don't close the process */
}