#include <core/main.h>

extern "C" int main(int argc, char* argv[]){
    Printlog("[Storage Manager] Loading device");
    InitialiseStorageSrv();
    Printlog("[Storage Manager] Storage loaded");
    InitialiseVFSSrv();
    Printlog("[Storage Manager] VFS loaded");

    return KSUCCESS;
}