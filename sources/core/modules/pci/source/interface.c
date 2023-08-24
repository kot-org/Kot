#include <errno.h>
#include <global/modules.h>
#include <lib/modules/pci.h>

pci_handler_t pci_internal_handler;

size_t count_devices(pci_device_info_t* info){
    return device_search(pci_device_array, info->vendor_id, info->device_id, info->class_id, info->sub_class_id, info->prog_if);
}

pci_device_id_t find_device(pci_device_info_t* info, size_t index){
    return device_find(pci_device_array, info->vendor_id, info->device_id, info->class_id, info->sub_class_id, info->prog_if, index);
}

int get_info_device(pci_device_id_t id, pci_device_info_t* info){
    if(!device_check(pci_device_array, id)) return EINVAL;

    pci_device_t* device_info = device_get_from_index(pci_device_array, id);
    pci_device_header_t* header = (pci_device_header_t*)device_info->configuration_space;
    info->vendor_id = header->vendor_id;
    info->device_id = header->device_id;
    info->class_id = header->class_id;
    info->sub_class_id = header->sub_class_id;
    info->prog_if = header->prog_if;
    
    return 0;
}

int get_bar_device(pci_device_id_t id, uint8_t bar_index, pci_bar_info_t* info){
    if(!device_check(pci_device_array, id)) return EINVAL;

    pci_device_t* device_info = device_get_from_index(pci_device_array, id);
    info->address = device_get_bar_address(device_info, bar_index);
    info->size = device_get_bar_size(device_info, bar_index);
    info->type = device_get_bar_type(device_info, bar_index);

    return 0;
}

int bind_msi(pci_device_id_t id, uint8_t interrupt_vector, uint8_t processor, uint16_t local_device_vector, uint64_t* version){
    if(!device_check(pci_device_array, id)) return EINVAL;

    pci_device_t* device_info = device_get_from_index(pci_device_array, id);
    return device_bind_msi(device_info, interrupt_vector, processor, local_device_vector, version);
}

int unbind_msi(pci_device_id_t id, uint16_t local_device_vector){
    if(!device_check(pci_device_array, id)) return EINVAL;

    pci_device_t* device_info = device_get_from_index(pci_device_array, id);
    return device_unbind_msi(device_info, local_device_vector);
}

int config_read_word(pci_device_id_t id, uint16_t offset, uint16_t* value){
    if(!device_check(pci_device_array, id)) return EINVAL;

    pci_device_t* device_info = device_get_from_index(pci_device_array, id);
    return device_config_read_word(device_info, offset, value);
}

int config_write_word(pci_device_id_t id, uint16_t offset, uint16_t value){
    if(!device_check(pci_device_array, id)) return EINVAL;

    pci_device_t* device_info = device_get_from_index(pci_device_array, id);
    return device_config_write_word(device_info, offset, value);
}

void interface_init(void){
    pci_internal_handler.count_devices = &count_devices;
    pci_internal_handler.find_device = &find_device;
    pci_internal_handler.get_info_device = &get_info_device;
    pci_internal_handler.get_bar_device = &get_bar_device;
    pci_internal_handler.bind_msi = &bind_msi;
    pci_internal_handler.unbind_msi = &unbind_msi;
    pci_internal_handler.config_read_word = &config_read_word;
    pci_internal_handler.config_write_word = &config_write_word;
    pci_handler = &pci_internal_handler;
}