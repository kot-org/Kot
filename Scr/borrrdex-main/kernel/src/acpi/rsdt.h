#pragma once

#include "common.h"
#include <cstdint>

// The Root System Descriptor Table, which holds the addreses
// of all other system descriptor tables on the system
// ACPI 6.4 p.145
typedef struct {
    acpi_desc_header_t h;
    uint32_t entries[0];
} __attribute__((packed)) rsdt_t;

class RSDT {
public:
	static constexpr const char* signature = "RSDT";

	RSDT(const void* data)
		:_data((rsdt_t *)data)
	{}

	size_t count() const;
	acpi_desc_header_t* get_at(size_t index) const;
	acpi_desc_header_t* get(const char* signature) const;

	bool is_valid() const;
	const rsdt_t* data() const { return _data; }
private:
	rsdt_t* _data;
};