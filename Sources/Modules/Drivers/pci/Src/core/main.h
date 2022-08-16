#pragma once

#define PCI_CONFIG_ADDR     0xCF8
#define PCI_CONFIG_DATA     0xCFC

#include <kot/sys.h>
#include <kot/arch.h>
#include <kot/heap.h>
#include <kot/cstring.h>

typedef struct {
	uint32_t VendorID;
	uint32_t DeviceID;
	uint32_t ClassCode;
	uint32_t SubClass;
	uint32_t Function;
} pci_device_t;