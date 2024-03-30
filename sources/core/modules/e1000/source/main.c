#include <lib/log.h>
#include <controller.h>
#include <lib/assert.h>
#include <global/heap.h>
#include <global/modules.h>

#include <arch/include.h>
#include ARCH_INCLUDE(asm.h)


#define MODULE_NAME "e1000"

e1000_controller_t** e1000_controllers = NULL;
size_t e1000_controller_count = 0;

#include <controller.c>

int init(int argc, char* args[]){
    log_printf("[module/"MODULE_NAME"] loading start\n");

    assert(modules_request_dependency(MODULE_TYPE_PCI) == 0);
    assert(modules_request_dependency(MODULE_TYPE_VFS) == 0);
    assert(modules_request_dependency(MODULE_TYPE_NET) == 0);
    assert(modules_request_dependency(MODULE_TYPE_TIME) == 0);

    pci_device_info_t device_info = {
        0x8086,
        E1000_DEV_ID_82574L,
        PCI_SEARCH_NO_PARAMETER,
        PCI_SEARCH_NO_PARAMETER,
        PCI_SEARCH_NO_PARAMETER
    };

    e1000_controller_count = pci_handler->count_devices(&device_info);

    e1000_controllers = (e1000_controller_t**)malloc(e1000_controller_count * sizeof(e1000_controller_t*));

    for(size_t i = 0; i < e1000_controller_count; i++){
        pci_device_id_t device_id = pci_handler->find_device(&device_info, i);
        e1000_controllers[i] = controller_init(device_id);
        net_handler->add_net_device(e1000_controllers[i]->net_device);
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
