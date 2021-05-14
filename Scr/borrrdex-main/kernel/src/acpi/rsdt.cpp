#include "rsdt.h"
#include "string.h"

size_t RSDT::count() const {
    return (_data->h.length - sizeof(_data->h)) / 8;
}

acpi_desc_header_t* RSDT::get(const char* signature) const {
    size_t rsdtEntries = (_data->h.length - sizeof(_data->h)) / 8;
    for(size_t i = 0; i < rsdtEntries; i++) {
        acpi_desc_header_t* h = (acpi_desc_header_t *)_data->entries[i];
        if(memcmp(h->signature, signature, 4) == 0) {
            return (acpi_desc_header_t *)h;
        }
    }

    return nullptr;
}

acpi_desc_header_t* RSDT::get_at(size_t index) const {
    size_t rsdtEntries = (_data->h.length - sizeof(_data->h)) / 8;
    if(index >= rsdtEntries) {
        return nullptr;
    }

    return (acpi_desc_header_t *)_data->entries[index];
}

bool RSDT::is_valid() const {
    return memcmp(_data->h.signature, signature, 4) == 0
        && acpi_checksum_ok(_data, _data->h.length);
}