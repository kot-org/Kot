#include <core/main.h>

int main(int argc, char* argv[]){
    kot_Printlog("[Storage manager] Initializing...");
    InitializePartition();
    InitializeVFS();
    InitializeDev();
    InitialiseSrv();
    kot_Printlog("[Storage manager] Initialized");

    return KSUCCESS;
}