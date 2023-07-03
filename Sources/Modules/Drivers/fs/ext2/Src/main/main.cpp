#include <main/main.h>

int main(int argc, char* argv[]){
    kot_Printlog("[FS-Ext] Initializing...");
    InitializeVolumeListener();
    kot_Printlog("[FS-Ext] Initialized");

    kot_Sys_Close(KSUCCESS); /* Don't close the process */
}