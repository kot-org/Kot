#include <core/main.h>

extern "C" int main(int argc, char* argv[]) {
    Printlog("[AHCI] Initialization ...");

    Srv_Pci_CountDevices()

    Printlog("[AHCI] Driver initialized successfully");

    return KSUCCESS;
}