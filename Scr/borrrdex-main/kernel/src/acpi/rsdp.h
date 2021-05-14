#pragma once

#ifndef __cplusplus
#error C++ Only
#endif

#include "common.h"

// The Root System Descriptor Pointer (looked up by UEFI)
// ACPI 6.4 p.141
typedef struct {
	char signature[8];
	uint8_t checksum;
	char OEMID[6];
	uint8_t revision;
	uint32_t rsdt_address;
	uint32_t length;
	uint64_t xdst_address;
	uint8_t extended_checksum;
	uint8_t reserved[3];
} __attribute__((packed)) rsdp_t;

class RSDP {
public:
	static constexpr const char* signature = "RSD PTR ";

	RSDP(const void* data)
		:_data((rsdp_t *)data)
	{}

	bool is_valid() const;
	const rsdp_t* data() const { return _data; }
private:
	rsdp_t* _data;
};