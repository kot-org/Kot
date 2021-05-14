#include "rsdp.h"
#include "string.h"

bool RSDP::is_valid() const {
	return memcmp(_data->signature, signature, 8) == 0
		&& acpi_checksum_ok(_data, _data->length);
}