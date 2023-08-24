#include <main.h>

pci_device_list_info_t* init_pci_list(void){
    pci_device_list_info_t* return_value = (pci_device_list_info_t*)malloc(sizeof(pci_device_list_info_t));
    return_value->devices = vector_create();
    return return_value;
}

void add_pci_device(pci_device_list_info_t* devices_list, pci_device_t* device){
    // setup command register
    pci_device_header_t* header = (pci_device_header_t*)device->configuration_space;
    device->send_configuration_space(device);

    // add device to list
    vector_push(devices_list->devices, device);
}

void convert_list_to_array(pci_device_list_info_t* devices_list, pci_device_array_info_t* devices_array){
    devices_array->devices_count = 1; // 0 is invalid index
    devices_array->devices = (pci_device_t**)malloc((devices_list->devices->length + 1) * sizeof(pci_device_t*));
    for(size_t i = 0; i < devices_list->devices->length; i++){
        devices_array->devices[devices_array->devices_count] = (pci_device_t*)vector_get(devices_list->devices, i);
        devices_array->devices_count++;
    }
}
