#ifndef _MODULES_PCI_H
#define _MODULES_PCI_H 1

#include <lib/modules/file.h>

#define PCI_BAR_TYPE_NULL           0x0
#define PCI_BAR_TYPE_IO             0x1
#define PCI_BAR_TYPE_32             0x2
#define PCI_BAR_TYPE_64             0x3

#define PCI_SEARCH_NO_PARAMETER     0xFFFF

#define PCI_MSI_VERSION             0x1
#define PCI_MSIX_VERSION            0x2

typedef uint64_t pci_device_id_t;

typedef struct {
    uint16_t vendor_id;
    uint16_t device_id;
    uint16_t class_id;
    uint16_t sub_class_id;
    uint16_t prog_if;
} pci_device_info_t;

typedef struct {
    void* address;
    size_t size;
    uint8_t type;
} pci_bar_info_t;

typedef struct{
    size_t (*count_devices)(pci_device_info_t*); // arg 0: info | return count
    pci_device_id_t (*find_device)(pci_device_info_t*, uint64_t); // arg 0: info | arg 1: index | return id
    int (*get_info_device)(pci_device_id_t, pci_device_info_t*); // arg 0: id | arg1: info pointer | return result
    int (*get_bar_device)(pci_device_id_t, uint8_t, pci_bar_info_t*); // arg 0: id | arg1: bar index | arg2: info pointer | return result
    int (*bind_msi)(pci_device_id_t, uint8_t, uint8_t, uint16_t, uint64_t*); // arg 0: id | arg1: interrupt vector | arg2: processor | arg3: local device vector | arg4: version pointer | return result
    int (*unbind_msi)(pci_device_id_t, uint16_t); // arg 0: id | arg1: local device vector | return result
    int (*config_read_word)(pci_device_id_t, uint16_t, uint16_t*); // arg 0: id | arg1: offset | arg2: value pointer | return result
    int (*config_write_word)(pci_device_id_t, uint16_t, uint16_t); // arg 0: id | arg1: offset | arg2: value | return result
} pci_handler_t;

#endif // _MODULES_PCI_H