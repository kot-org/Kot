#include <main.h>
#include <device.h>

static uint8_t controller_get_port_type(hba_port_t* port){
    uint32_t sata_status = port->sata_status;

    uint8_t interface_power_management = (sata_status >> 8) & 0b111;
    uint8_t device_detection = sata_status & 0b111;

    if(device_detection != HBA_PORT_DEV_PRESENT) return PORT_TYPE_NONE;
    if(interface_power_management != HBA_PORT_IPM_ACTIVE) return PORT_TYPE_NONE;

    switch (port->signature){
        case SATA_SIG_ATAPI:
            return PORT_TYPE_SATAPI;
        case SATA_SIG_ATA:
            return PORT_TYPE_SATA;
        case SATA_SIG_PM:
            return PORT_TYPE_PM;
        case SATA_SIG_SEMB:
            return PORT_TYPE_SEMB;
        default:
            return PORT_TYPE_NONE;
    }
}

static void controller_find_ports(ahci_controller_t* controller){
    controller->port_count = 0;

    uint8_t ports_implemented = controller->abar->ports_implemented;
    for(uint8_t i = 0; i < PORT_MAX_COUNT; i++){
        if(ports_implemented & (1 << i)){
            uint8_t port_type = controller_get_port_type(&controller->abar->ports[i]);
            if(port_type == PORT_TYPE_SATA){
                ahci_device_t* device = init_sata_device(&controller->abar->ports[i]);
                ahci_init_device(device);
            }
        }
    }
}

ahci_controller_t* init_controller(pci_device_id_t device_id){
    ahci_controller_t* controller = (ahci_controller_t*)malloc(sizeof(ahci_controller_t));

    pci_bar_info_t bar_info;

    assert(pci_handler->get_bar_device(device_id, 5, &bar_info) == 0);

    assert(bar_info.type == PCI_BAR_TYPE_64 || bar_info.type == PCI_BAR_TYPE_32);

    controller->abar = ahci_map_physical(bar_info.address, bar_info.size);
    
    controller_find_ports(controller);

    return controller;
}