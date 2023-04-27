#include <core/main.h>

extern "C" int main(int argc, char* argv[]){
    Printlog("[Storage manager] Initializing...");
    InitializePartition();
    InitializeVFS();
    InitializeDev();
    InitialiseSrv();
    Printlog("[Storage manager] Initialized");

    return KSUCCESS;
}