#include <main.h>
#include <port.h>
#include <controller.h>

#define MODULE_NAME "ahci"

#include <port.c>
#include <device.c>
#include <controller.c>

ahci_controller_t** controllers;

int init(int argc, char* args[]){
    log_printf("[module/"MODULE_NAME"] loading start\n");

    assert(modules_request_dependency(MODULE_TYPE_PCI) == 0);
    assert(modules_request_dependency(MODULE_TYPE_STORAGE) == 0);

    pci_device_info_t device_info = {
        PCI_SEARCH_NO_PARAMETER,
        PCI_SEARCH_NO_PARAMETER,
        0x1,
        0x6,
        0x1
    };

    size_t ahci_controller_count = pci_handler->count_devices(&device_info);

    controllers = (ahci_controller_t**)malloc(ahci_controller_count * sizeof(ahci_controller_t*));

    for(size_t i = 0; i < ahci_controller_count; i++){
        pci_device_id_t device_id = pci_handler->find_device(&device_info, i);
        controllers[i] = init_controller(device_id);
    }

    log_printf("[module/"MODULE_NAME"] loading end\n");
    return 0;
}

int fini(void){
    return 0;
}

module_metadata_t module_metadata = {
    &init,
    &fini,
    MODULE_TYPE_UNDEFINE,
    MODULE_NAME
};
