#include <core/main.h>

extern "C" int main(int argc, char* argv[]){
    Printlog("[Storage] Loading device");
    InitialiseStorageSrv();
    InitialiseVFSSrv();
    Printlog("[Storage] Device loaded");

    return KSUCCESS;
}