#include <core/main.h>

int main(int argc, char* argv[]) {
    kot_Printlog("[USB/UHCI] Initialization ...");

    

    kot_Printlog("[USB/UHCI] Driver initialized successfully");

    kot_Sys_Close(KSUCCESS); /* Don't close the process */
}