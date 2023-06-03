#include <main/main.h>

int main(int argc, char* argv[]){
    kot_Printlog("[FS-Ext] Initializing...");
    InitializeVolumeListener();
    kot_Printlog("[FS-Ext] Initialized");

    return KSUCCESS;
}