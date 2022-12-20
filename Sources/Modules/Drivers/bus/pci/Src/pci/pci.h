#pragma once
#include <core/main.h>


#define PCI_CONFIG_ADDR     0xCF8
#define PCI_CONFIG_DATA     0xCFC

#define PCI_CONFIGURATION_SPACE_SIZE 0x100

void InitPCI(struct PCIDeviceListInfo_t* PCIDeviceList);

void ReceiveConfigurationSpacePCI(struct PCIDevice_t* Device);
void SendConfigurationSpacePCI(struct PCIDevice_t* Device);