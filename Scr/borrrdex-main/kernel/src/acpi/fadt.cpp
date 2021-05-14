#include "fadt.h"
#include "string.h"

bool FADT::is_valid() const {
    return memcmp(_data->h.signature, signature, 4) == 0
        && acpi_checksum_ok(_data, _data->h.length);
}