#include <core/main.h>

extern "C" int main(int argc, char* argv[]) {
    kot_Printlog("[USB/EHCI] Initialization ...");

    

    kot_Printlog("[USB/EHCI] Driver initialized successfully");

    return KSUCCESS;
}