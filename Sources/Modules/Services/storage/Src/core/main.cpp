#include <core/main.h>

extern "C" int main(int argc, char* argv[]){
    Printlog("[Storage manager] Initializing....");
    InitializePartition();
    InitialiseSrv();
    Printlog("[Storage manager] Initialized");

    return KSUCCESS;
}