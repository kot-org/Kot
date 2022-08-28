#include <core/main.h>

#include <tools/config.h>
#include <tools/memory.h>

process_t proc;

extern "C" int main(int argc, char* argv[]) {
    Printlog("[PCI] Initialization ...");
    
    EnumerateDevices();

    InitSrv();

    Printlog("[PCI] Driver initialized successfully");

    return KSUCCESS;
}