#include <lib/log.h>
#include <global/heap.h>
#include <global/modules.h>

#include <arch/include.h>
#include ARCH_INCLUDE(acpi.h)

#define MODULE_NAME "pci"

#include <main.h>
pci_device_array_info_t* pci_device_array = NULL;

#include <pci.c>
#include <pcie.c>
#include <list.c>
#include <device.c>
#include <interface.c>


int init(int argc, char* args[]){
    log_printf("[module/"MODULE_NAME"] loading start\n");

    void* mcfg_table = acpi_find_table(acpi_rsdp, "MCFG");
    
    pci_device_list_info_t* pci_device_list = init_pci_list();

    if(mcfg_table != NULL){
        init_pcie(pci_device_list, mcfg_table);
    }else{
        init_pci(pci_device_list);
    }

    pci_device_array = (pci_device_array_info_t*)malloc(sizeof(pci_device_array_info_t));

    convert_list_to_array(pci_device_list, pci_device_array);

    interface_init();

    log_printf("[module/"MODULE_NAME"] loading end\n");
    return 0;
}

int fini(void){
    return 0;
}

module_metadata_t module_metadata = {
    &init,
    &fini,
    MODULE_TYPE_PCI,
    MODULE_NAME
};
