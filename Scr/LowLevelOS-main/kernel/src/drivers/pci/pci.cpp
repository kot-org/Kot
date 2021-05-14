#include "pci.h"
#include "../../memory/PageTableManager.h"
#include "../../drivers/display/displaydriver.h"
#include "../../libc/stdio.h"
#include "../ahci/ahci.h"
#include "../../memory/heap.h"

PCITranslate translate;

void PCI::EnumFunc(uint64_t addr,uint64_t function) {
    uint64_t offset = function << 12;
    
    uint64_t busAddress = addr + offset;
    GlobalTableManager.MapMemory((void*)busAddress,(void*)busAddress);

    PCIDevice* deviceZ = (PCIDevice*)busAddress;

    if(deviceZ->DeviceID == 0) return;
    if(deviceZ->DeviceID == 0xFFFF) return;
    
    Devices[DevicesIndex] = translate.TranslateDevice(deviceZ);
    DevicesIndex++;
    switch (deviceZ->Class){
        case 0x01:
            switch (deviceZ->Subclass){
                case 0x06:
                    switch (deviceZ->ProgramInterface){
                        case 0x01:
                            new AHCIDriver(deviceZ);
                    }
            }
        }
}

void PCI::EnumDevice(uint64_t addr, uint64_t device) {
    uint64_t offset = device << 15;
    
    uint64_t busAddress = addr + offset;
    GlobalTableManager.MapMemory((void*)busAddress,(void*)busAddress);

    PCIDevice* deviceZ = (PCIDevice*)busAddress;

    if(deviceZ->DeviceID == 0) return;
    if(deviceZ->DeviceID == 0xFFFF) return;

    for(uint64_t device = 0; device < 8;device++) {
        EnumFunc(busAddress,device);
    }
}

void PCI::EnumBus(uint64_t addr, uint64_t bus) {
    uint64_t offset = bus << 20;
    
    uint64_t busAddress = addr + offset;
    GlobalTableManager.MapMemory((void*)busAddress,(void*)busAddress);

    PCIDevice* device = (PCIDevice*)busAddress;

    if(device->DeviceID == 0) return;
    if(device->DeviceID == 0xFFFF) return;

    for(uint64_t device = 0; device < 32;device++) {
        EnumDevice(busAddress,device);
    }
}

void PCI::EnumeratePCI(MCFG* mcfg) {
    int entries = ((mcfg->Header.Lenght) - sizeof(MCFG)) / sizeof(DeviceConfig);
    for(int t = 0; t < entries;t++) {
        DeviceConfig* conf = (DeviceConfig*)((uint64_t)mcfg + sizeof(MCFG) + (sizeof(DeviceConfig) * t));
        for(uint64_t bus = conf->StartBus; bus < conf->EndBus;bus++) {
            EnumBus(conf->BaseAddress,bus);
        }
    }
}