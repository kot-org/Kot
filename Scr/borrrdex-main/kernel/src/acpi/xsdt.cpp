#include "xsdt.h"
#include "fadt.h"
#include "hpet.h"
#include "mcfg.h"
#include "../graphics/BasicRenderer.h"
#include "string.h"
#include "apic.h"

size_t XSDT::count() const {
    return (_data->h.length - sizeof(_data->h)) / 8;
}

acpi_desc_header_t* XSDT::get(const char* signature) const {
    size_t xsdtEntries = (_data->h.length - sizeof(_data->h)) / 8;
    for(size_t i = 0; i < xsdtEntries; i++) {
        acpi_desc_header_t* h = (acpi_desc_header_t *)_data->entries[i];
        if(memcmp(h->signature, signature, 4) == 0) {
            return (acpi_desc_header_t *)h;
        }
    }

    return nullptr;
}

acpi_desc_header_t* XSDT::get_at(size_t index) const {
    size_t xsdtEntries = (_data->h.length - sizeof(_data->h)) / 8;
    if(index >= xsdtEntries) {
        return nullptr;
    }

    return (acpi_desc_header_t *)_data->entries[index];
}

bool XSDT::is_valid() const {
    return memcmp(_data->h.signature, signature, 4) == 0
        && acpi_checksum_ok(_data, _data->h.length);
}