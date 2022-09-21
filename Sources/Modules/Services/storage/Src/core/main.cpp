#include <core/main.h>

extern "C" int main(int argc, char* argv[]){
    Printlog("[Storage manager] Initializing....");
    InitialiseSrv();
    Printlog("[Storage manager] Initialized");

    return KSUCCESS;
}