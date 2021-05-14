#include "mcfg.h"
#include "../pci/pci.h"
#include "../graphics/BasicRenderer.h"
#include "../paging/PageTableManager.h"
#include "string.h"

void MCFG::print() const {
    if(!is_valid()) {
        GlobalRenderer->Printf(" [ERROR Corrupted]");
        return;
    }

    GlobalRenderer->Next();
    GlobalRenderer->Printf("        PCI Devices:");
    GlobalRenderer->Next();
    size_t entries = count();
    for(int i = 0; i < entries; i++) {
        mcfg_config_entry_t e = _data->entries[i];
        pci_print_bus((uint8_t *)e.base_address, 0);
    }
}

size_t MCFG::count() const {
    return (_data->h.length - (sizeof(_data->h) + sizeof(_data->reserved))) / sizeof(mcfg_config_entry_t);
}

mcfg_config_entry_t* MCFG::get(size_t idx) const {
    return &(_data->entries[idx]);
}

bool MCFG::is_valid() const {
    return memcmp(_data->h.signature, signature, 4) == 0
        && acpi_checksum_ok(_data, _data->h.length);
}