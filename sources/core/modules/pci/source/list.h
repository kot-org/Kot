#ifndef _MODULE_PCI_LIST_H
#define _MODULE_PCI_LIST_H

pci_device_list_info_t* init_pci_list(void);

void add_pci_device(pci_device_list_info_t* devices_list, pci_device_t* device);

void convert_list_to_array(pci_device_list_info_t* devices_list, pci_device_array_info_t* devices_array);

#endif