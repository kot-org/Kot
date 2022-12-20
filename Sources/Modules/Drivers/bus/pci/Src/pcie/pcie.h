#pragma once
#include <core/main.h>

#define PCIE_CONFIGURATION_SPACE_SIZE 0x1000

struct DeviceConfig_t{
    uint64_t BaseAddress;
    uint16_t PCISegGroup;
    uint8_t StartBus;
    uint8_t EndBus;
    uint32_t Reserved;
}__attribute__((packed));

struct MCFGHeader_t{
    srv_system_sdtheader_t Header;
    uint64_t Reserved;
    DeviceConfig_t ConfigurationSpace[];
}__attribute__((packed));

void InitPCIe(struct PCIDeviceListInfo_t* PCIDeviceList, uintptr_t mcfgAddress);

void EnumerateBus(struct PCIDeviceListInfo_t* PCIDeviceList, uintptr_t baseAddress, uint64_t bus);
void EnumerateDevice(struct PCIDeviceListInfo_t* PCIDeviceList, uintptr_t busAddress, uint64_t device);
void EnumerateFunction(struct PCIDeviceListInfo_t* PCIDeviceList, uintptr_t deviceAddress, uint64_t function);

void ReceiveConfigurationSpacePCIe(struct PCIDevice_t* Device);
void SendConfigurationSpacePCIe(struct PCIDevice_t* Device);