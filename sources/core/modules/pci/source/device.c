#include <main.h>
#include <errno.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>


static void* device_map_physical(void* address, size_t size){
    return vmm_get_virtual_address(address);
}


bool device_check(pci_device_array_info_t* devices_array, pci_device_id_t device){
    if(devices_array->devices_count != 0 && device < devices_array->devices_count){
        return true;
    }
    return false;
}

pci_device_t* device_get_from_index(pci_device_array_info_t* devices_array, pci_device_id_t device){
    return devices_array->devices[device];
}

size_t device_search(pci_device_array_info_t* devices_array, uint16_t vendor_id, uint16_t device_id, uint16_t class_id, uint16_t sub_class_id, uint16_t prog_if){
    uint8_t check_required = 0;
    size_t device_num = 0;

    if(vendor_id != PCI_SEARCH_NO_PARAMETER)
        check_required++;
    if(device_id != PCI_SEARCH_NO_PARAMETER)
        check_required++;
    if(sub_class_id != PCI_SEARCH_NO_PARAMETER)
        check_required++;
    if(class_id != PCI_SEARCH_NO_PARAMETER)
        check_required++;
    if(prog_if != PCI_SEARCH_NO_PARAMETER)
        check_required++;

    for(size_t i = 1; i < devices_array->devices_count; i++){
        pci_device_header_t* header = (pci_device_header_t*)devices_array->devices[i]->configuration_space;

        uint8_t check_num = 0;
        
        if(header->vendor_id == vendor_id)
            check_num++;
        if(header->device_id == device_id)
            check_num++;
        if(header->sub_class_id == sub_class_id)
            check_num++;
        if(header->class_id == class_id)
            check_num++;
        if(header->prog_if == prog_if)
            check_num++;

        if(check_required == check_num) device_num++;
    }
    return device_num;
}

pci_device_id_t device_find(pci_device_array_info_t* devices_array, uint16_t vendor_id, uint16_t device_id, uint16_t class_id, uint16_t sub_class_id, uint16_t prog_if, size_t index){
    uint8_t check_required = 0;
    uint32_t device_num = 0;

    if(vendor_id != PCI_SEARCH_NO_PARAMETER)
        check_required++;
    if(device_id != PCI_SEARCH_NO_PARAMETER)
        check_required++;
    if(sub_class_id != PCI_SEARCH_NO_PARAMETER)
        check_required++;
    if(class_id != PCI_SEARCH_NO_PARAMETER)
        check_required++;
    if(prog_if != PCI_SEARCH_NO_PARAMETER)
        check_required++;

    for(size_t i = 1; i < devices_array->devices_count; i++){
        pci_device_header_t* header = (pci_device_header_t*)devices_array->devices[i]->configuration_space;

        uint8_t check_num = 0;
        
        if(header->vendor_id == vendor_id)
            check_num++;
        if(header->device_id == device_id)
            check_num++;
        if(header->sub_class_id == sub_class_id)
            check_num++;
        if(header->class_id == class_id)
            check_num++;
        if(header->prog_if == prog_if)
            check_num++;

        if(check_required == check_num){
            if(index == device_num){
                return i;
            }
            device_num++;
        } 


    }
    return 0;
}


uint8_t device_get_bar_type_with_bar_value(uint32_t value){
    if(value & 0b1){ /* i/o */
        return PCI_BAR_TYPE_IO;
    }else{
        if(!(value & 0b110)){ /* 32bits */
            return PCI_BAR_TYPE_32;
        }else if((value & 0b110) == 0b110){ /* 64bits */
            return PCI_BAR_TYPE_64;
        }
    }
    return PCI_BAR_TYPE_NULL;
}

void* device_get_bar_address(pci_device_t* device, uint8_t index){
    pci_device_header_t* header = (pci_device_header_t*)device->configuration_space;
    switch (header->header_type & 0x7f){
        case 0x0:{
            if(index < 6){
                pci_header0_t* header0 = (pci_header0_t*)header;
                switch (device_get_bar_type_with_bar_value(header0->bar[index])){
                    case PCI_BAR_TYPE_IO:
                        return (void*)(uintptr_t)(header0->bar[index] & 0xfffffffc);
                    case PCI_BAR_TYPE_32:
                        return (void*)(uintptr_t)(header0->bar[index] & 0xfffffff0);
                    case PCI_BAR_TYPE_64:
                        return (void*)((uintptr_t)((header0->bar[index] & 0xfffffff0)) | (((uintptr_t)(header0->bar[index + 1] & 0xffffffff)) << 32));
                    default:
                        break;
                }
            }
            break;
        }    
        default:
            break;
    }
    return NULL;
}

size_t device_get_bar_size_with_address(pci_device_t* device, void* addresslow){
    uint32_t bar_value_low = *(uint32_t*)addresslow;
    uint8_t type = device_get_bar_type_with_bar_value(bar_value_low);

    if(type != PCI_BAR_TYPE_NULL){
        /* write into bar low */
        *(uint32_t*)addresslow = 0xffffffff;
        device->send_configuration_space(device);

        /* read bar low */
        device->receive_configuration_space(device);
        uint32_t size_low = *(uint32_t*)addresslow;

        if(type == PCI_BAR_TYPE_IO){
            size_low &= 0xfffffffc;
        }else{
            size_low &= 0xfffffff0;
        }


        uint32_t size_high = 0xffffffff;

        if(type == PCI_BAR_TYPE_64){
            void* addresshigh = (void*)((uint64_t)addresslow + 0x4);

            uint32_t bar_value_high = *(uint32_t*)addresshigh;
            /* write into bar high */
            *(uint32_t*)addresshigh = 0xffffffff;
            device->send_configuration_space(device);

            /* read bar high */
            device->receive_configuration_space(device);
            size_high = *(uint32_t*)addresshigh;

            /* restore value */
            *(uint32_t*)addresshigh = bar_value_high;   
            device->send_configuration_space(device);    
        }

        /* restore value */
        *(uint32_t*)addresslow = bar_value_low;
        device->send_configuration_space(device);

        uint64_t size = ((uint64_t)(size_high & 0xffffffff) << 32) | (size_low & 0xffffffff);
        size = ~size + 1;
        return size;
    }

    return 0;
}

size_t device_get_bar_size(pci_device_t* device, uint8_t index){
    pci_device_header_t* header = (pci_device_header_t*)device->configuration_space;
    switch (header->header_type & 0x7f){
        case 0x0:{
            if(index < 6){
                pci_header0_t* header0 = (pci_header0_t*)header;
                return device_get_bar_size_with_address(device, &header0->bar[index]);
            }
            break;
        }    
        default:
            break;
    }
    return 0;
}

uint8_t device_get_bar_type(pci_device_t* device, uint8_t index){
    pci_device_header_t* header = (pci_device_header_t*)device->configuration_space;
    switch (header->header_type & 0x7f){
        case 0x0:{
            if(index < 6){
                pci_header0_t* header0 = (pci_header0_t*)header;
                return device_get_bar_type_with_bar_value(header0->bar[index]);
            }
            break;
        }    
        default:
            break;
    }
    return 0;
}

int device_bind_msi(pci_device_t* device, uint8_t interrupt_vector, uint8_t processor, uint16_t local_device_vector, uint64_t* version){
    pci_device_header_t* header = (pci_device_header_t*)device->configuration_space;
    switch (header->header_type & 0x7f){
        case 0x0:{
            pci_header0_t* header0 = (pci_header0_t*)header;
            uint8_t capability_offset = header0->capabilities_ptr;
            pci_capability_t* capability = (pci_capability_t*)((uint64_t)header + (uint64_t)capability_offset);
            pci_capability_t* capability_msi = NULL;
            pci_capability_t* capability_msix = NULL;
            while(capability_offset){
                capability_offset = capability->capability_next;
                if(capability->capability_id == pci_capabilities_msi){
                    capability_msi = capability;
                }else if(capability->capability_id == pci_capabilities_msix){
                    capability_msix = capability;
                    break;
                }
                capability = (pci_capability_t*)((uint64_t)header + (uint64_t)capability_offset);
            }
            if(capability_msix){
                capability_msi->msix.control |= 1 << 15; // enable msi_x
                uint64_t table_address = ((uint64_t)device_map_physical(device_get_bar_address(device, capability_msix->msix.bir), device_get_bar_size(device, capability_msix->msix.bir)) + (uint64_t)capability_msix->msix.table_offset);
                uint16_t entries = capability_msi->msix.control & 0x7ff;
                if(entries <= local_device_vector){
                    pcimsix_table_t* table = (pcimsix_table_t*)(table_address + sizeof(pcimsix_table_t) * local_device_vector);
                    table->address = 0xfee00000 | (processor << 12);
                    table->data = interrupt_vector; 
                    table->control &= ~(1 << 0); // clear first to unmasked msi
                    device->send_configuration_space(device);
                    *version = PCI_MSIX_VERSION;
                    return 0;
                }
            }else if(capability_msi){
                if(capability_msi->msi.control & (1 << 7)){ // check if support 64 bits
                    capability_msi->msi.address = 0xfee00000 | (processor << 12);
                    capability_msi->msi.data = (interrupt_vector & 0xff) | (1 << 14) | (1 << 15); 
                    capability_msi->msi.control &= ~((0b111 << 4) | (1 << 8)); // set 0 for mme and for msi ma_sk
                    capability_msi->msi.control |= (1 << 0);                   

                    header->command |= PCI_COMMAND_INTERRUPT_DISABLE;
                    device->send_configuration_space(device);
                    *version = PCI_MSI_VERSION;
                    return 0;
                }
            }
            break;
        }    
        default:
            break;
    }
    return ENOSYS;
}

int device_unbind_msi(pci_device_t* device, uint16_t local_device_vector){
    pci_device_header_t* header = (pci_device_header_t*)device->configuration_space;
    switch (header->header_type & 0x7f){
        case 0x0:{
            pci_header0_t* header0 = (pci_header0_t*)header;
            uint8_t capability_offset = header0->capabilities_ptr;
            pci_capability_t* capability = (pci_capability_t*)((uint64_t)header + (uint64_t)capability_offset);
            pci_capability_t* capability_msi = NULL;
            pci_capability_t* capability_msix = NULL;
            while(capability_offset){
                capability_offset = capability->capability_next;
                if(capability->capability_id == pci_capabilities_msi){
                    capability_msi = capability;
                } else if(capability->capability_id == pci_capabilities_msix){
                    capability_msix = capability;
                    break;
                }
                capability = (pci_capability_t*)((uint64_t)header + (uint64_t)capability_offset);
            }
            if(capability_msix){
                capability_msi->msix.control |= 1 << 15; // enable msi_x
                uint64_t table_address = ((uint64_t)device_map_physical(device_get_bar_address(device, capability_msix->msix.bir), device_get_bar_size(device, capability_msix->msix.bir)) + (uint64_t)capability_msix->msix.table_offset);
                uint16_t entries = capability_msi->msix.control & 0x7ff;
                if(entries <= local_device_vector){
                    pcimsix_table_t* table = (pcimsix_table_t*)(table_address + sizeof(pcimsix_table_t) * local_device_vector);
                    table->control |= 1 << 0; // masked msi
                    table->address = 0;
                    table->data = 0; 
                    device->send_configuration_space(device);
                    return 0;
                }
            }else if(capability_msi){
                capability_msi->msi.control &= ~(1 << 0);  // disable msi
                capability_msi->msi.address = 0;
                capability_msi->msi.data = 0; 
                device->send_configuration_space(device);
                return 0;
            }
            break;
        }    
        default:
            break;
    }
    return ENOSYS;
}

int device_config_read_word(pci_device_t* device, uint16_t offset, uint16_t* value){
    // check offset
    if(device->is_pcie){
        if(offset > (PCI_CONFIGURATION_SPACE_PCIE - sizeof(uint16_t))){
            return EINVAL;
        }
    }else{
        if(offset > (PCI_CONFIGURATION_SPACE_PCI - sizeof(uint16_t))){
            return EINVAL;
        }
    }
    *value = *(uint16_t*)((uint64_t)device->configuration_space + (uint64_t)offset);
    device->receive_configuration_space(device);
    return 0;
}

int device_config_write_word(pci_device_t* device, uint16_t offset, uint16_t value){
    // check offset
    if(device->is_pcie){
        if(offset > (PCI_CONFIGURATION_SPACE_PCIE - sizeof(uint16_t))){
            return EINVAL;
        }
    }else{
        if(offset > (PCI_CONFIGURATION_SPACE_PCI - sizeof(uint16_t))){
            return EINVAL;
        }
    }
    *(uint16_t*)((uint64_t)device->configuration_space + (uint64_t)offset) = value;
    device->send_configuration_space(device);
    return 0;
}