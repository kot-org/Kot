#include <core/main.h>

extern "C" int main(int argc, char* argv[]){
    Printlog("[FS-Ext] Initializing...");
    InitializeVolumeListener();
    Printlog("[FS-Ext] Initialized");

    return KSUCCESS;
}