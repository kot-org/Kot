#pragma once

#ifndef __cplusplus
#error C++ Only
#endif

#include "common.h"
#include <cstdint>

// The Extended System Descriptor Table, which holds the addreses
// of all other system descriptor tables on the system (Root
// System Descriptor Table also exists, for 32-bit implementations)
// ACPI 6.4 p.146
typedef struct {
	acpi_desc_header_t h;
	void* entries[0];
} __attribute__((packed)) xsdt_t;

class XSDT {
public:
	static constexpr const char* signature = "XSDT";

	XSDT(const void* data)
		:_data((xsdt_t *)data)
	{}

	size_t count() const;
	acpi_desc_header_t* get_at(size_t index) const;
	acpi_desc_header_t* get(const char* signature) const;

	bool is_valid() const;
	const xsdt_t* data() const { return _data; }
private:
	xsdt_t* _data;
};
