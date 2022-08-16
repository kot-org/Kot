#include <core/main.h>

uint16_t ReadWord(uint16_t bus, uint16_t device, uint16_t func, uint16_t offset) {
    uint64_t addr;
    uint16_t r;

    /**
     *   -------------------------------------------------------------------------------------------------------------
     *   |  Bit 31	    |   Bits 30-24	|  Bits 23-16	|  Bits 15-11	  |   Bits 10-8	        |   Bits 7-0         |
     *   |  Enable Bit	|   Reserved	|  Bus Number	|  Device Number  |   Function Number	|   Register Offset  |
     *   -------------------------------------------------------------------------------------------------------------
     */
    addr = (uint64_t) (0x80000000 | (bus << 16) | (device << 11) | (func << 8) | (offset & 0xFC));
    /* Write address */
    IoWrite32(PCI_CONFIG_ADDR, addr);
    /* Read data */
    r = (uint16_t) ((IoRead32(PCI_CONFIG_DATA) >> ((offset & 2) * 8)) & 0xFFFF);

    return r;
}

uint16_t GetDeviceID(uint16_t bus, uint16_t device, uint16_t func) {
    uint32_t dID = ReadWord(bus, device, func, 2);
    return dID;
}

uint16_t GetVendorID(uint16_t bus, uint16_t device, uint16_t func) {
    uint32_t vID = ReadWord(bus, device, func, 0);
    return vID;
}

void AddPCIDevice(uint16_t vendor, uint16_t device, uint16_t func) {
    pci_device_t* _device = (pci_device_t*) malloc(sizeof(pci_device_t));

    _device->VendorID = vendor;
    _device->DeviceID = device;
    _device->Function = func;
}

void PciInit() {
    for(uint32_t bus = 0; bus < 256; bus++) {
        for(uint32_t device = 0; device < 32; device++) {
            for(uint32_t func = 0; func < 8; func++) {

                uint16_t vendorID = GetVendorID(bus, device, func);
                uint16_t deviceID = GetDeviceID(bus, device, func);

                if(deviceID == 0 || vendorID == 0) return; 
                if(deviceID == 0xFFFF || vendorID == 0) return;
                
                AddPCIDevice(vendorID, deviceID, func);
            }
        }
    }
}

extern "C" int main(int argc, char* argv[]) {
    Printlog("[PCI] Initialization ...");

    PciInit();

    return KSUCCESS;
}