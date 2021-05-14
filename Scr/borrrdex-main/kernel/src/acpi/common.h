#pragma once

#include <stdint.h>
#include <stddef.h>

// Common header for all ACPI System Descriptor Tables
// ACPI 6.4 p.141
typedef struct {
	char signature[4];
	uint32_t length;
	uint8_t revision;
	uint8_t checksum;
	char OEMID[6];
	char OEMTABLEID[8];
	uint32_t OEMREVISION;
	uint32_t creator_id;
	uint32_t creator_revision;
} acpi_desc_header_t;

// The Generic Address Structure (GAS) provides the platform with a robust means to describe register locations.
// ACPI 6.4 p.138
typedef struct {
	uint8_t address_space_id;
	uint8_t register_bit_width;
	uint8_t register_bit_offset;
	uint8_t access_size;
	uint64_t address;
} __attribute__((packed)) acpi_generic_addr_t;

bool acpi_checksum_ok(void* ptr, size_t len);