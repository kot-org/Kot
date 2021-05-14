#include "pci.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wwrite-strings"

uint16_t PCI::pciConfigReadWord (uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset) {
    uint32_t address;
    uint32_t lbus  = (uint32_t)bus;
    uint32_t lslot = (uint32_t)slot;
    uint32_t lfunc = (uint32_t)func;
    uint16_t tmp = 0;
    address = (uint32_t)((lbus << 16) | (lslot << 11) | (lfunc << 8) | (offset & 0xfc) | ((uint32_t)0x80000000));
    outportl(0xCF8, address);
    tmp = (uint16_t)((inportl(0xCFC) >> ((offset & 2) * 8)) & 0xffff);
    return (tmp);
}

uint16_t PCI::getVendorID(uint16_t bus, uint16_t device, uint16_t function) {
    return pciConfigReadWord(bus,device,function,0);
}

uint16_t PCI::getDeviceID(uint16_t bus, uint16_t device, uint16_t function) {
    return pciConfigReadWord(bus,device,function,2);
}

uint16_t PCI::getClassId(uint16_t bus, uint16_t device, uint16_t function)
{
        return (pciConfigReadWord(bus,device,function,0xA) & ~0x00FF) >> 8;
}

uint16_t PCI::getSubClassId(uint16_t bus, uint16_t device, uint16_t function)
{
        return (pciConfigReadWord(bus,device,function,0xA) & ~0xFF00);
}

void PCI::detectDevices() {
    int index = 0;
	for(uint32_t bus = 0; bus < 256; bus++)
    {
        for(uint32_t slot = 0; slot < 32; slot++)
        {
            for(uint32_t function = 0; function < 8; function++)
            {
                    uint16_t vendor = getVendorID(bus, slot, function);
                    if(vendor == 0xffff) continue;
                    uint16_t device = getDeviceID(bus, slot, function);
                    uint16_t classid = getClassId(bus, slot, function);
                    PCIDevice pcidevice;
                    pcidevice.VendorID = vendor;
                    pcidevice.DeviceID = device;
                    pcidevice.Function = function;

                    switch (classid)
                    {
                    case 1:
                        pcidevice.Class = "Mass Storage Controller";
                        break;
                    case 2:
                        pcidevice.Class = "Network Controller";
                        break;
                    case 3:
                        pcidevice.Class = "Display Controller";
                        break;
                    case 4:
                        pcidevice.Class = "Multimedia Controller";
                        break;
                    case 5:
                        pcidevice.Class = "Memory Controller";
                        break;
                    case 6:
                        pcidevice.Class = "Bridge Device";
                        break;
                    case 7:
                        pcidevice.Class = "Simple Communication Controller";
                        break;
                    case 8:
                        pcidevice.Class = "Base System Peripheral";
                        break;
                    case 9:
                        pcidevice.Class = "Input Device Controller";
                        break;
                    case 10:
                        pcidevice.Class = "Docking Station";
                        break;
                    case 11:
                        pcidevice.Class = "Processor";
                        break;
                    case 12:
                        pcidevice.Class = "Serial Bus Controller";
                        break;
                    case 13:
                        pcidevice.Class = "Wireless Controller";
                        break;
                    case 14:
                        pcidevice.Class = "Intelligent Controller";
                        break;
                    case 15:
                        pcidevice.Class = "Satellite Communication Controller";
                        break;
                    case 16:
                        pcidevice.Class = "Encryption Controller";
                        break;
                    case 17:
                        pcidevice.Class = "Signal Processing Controller";
                        break;
                    default:
                        pcidevice.Class = "Unclassified";
                        break;
                    }
                    pcidevice.Bus = bus;
                    pcidevice.Slot = slot;
                    Devices[index] = pcidevice;
                    index++;
            }
        }
    }
    DeviceCount = index;
}
#pragma GCC diagnostic pop