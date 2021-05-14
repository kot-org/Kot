#include "pci/pci.h"
#include "uhci.h"
#include "ohci.h"

constexpr uint8_t INTERFACE_UHCI = 0x0;
constexpr uint8_t INTERFACE_OHCI = 0x10;
constexpr uint8_t INTERFACE_EHCI = 0x20;
constexpr uint8_t INTERFACE_XHCI = 0x30;

static int usb_init(pci_header_t* mem) {
    pci_device_t* dev = (pci_device_t *)mem;
    switch(dev->common.prog_interface) {
        case INTERFACE_XHCI:
        case INTERFACE_EHCI:
            return -1;
        case INTERFACE_OHCI:
            return ohci_init(dev);
        case INTERFACE_UHCI:
            return uhci_init(dev);
        default:
            return -1;
    }
}

PCI_MODULE_INIT(USB_PCI_MODULE, usb_init, 0xC, 0x3);