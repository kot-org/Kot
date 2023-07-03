#include <core/main.h>

int main(int argc, char* argv[]) {
    kot_Printlog("[USB/EHCI] Initialization ...");

    

    kot_Printlog("[USB/EHCI] Driver initialized successfully");

    kot_Sys_Close(KSUCCESS); /* Don't close the process */
}