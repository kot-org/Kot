#ifndef _MODULE_AHCI_MAIN_H
#define _MODULE_AHCI_MAIN_H

#include <lib/log.h>
#include <impl/vmm.h>
#include <lib/lock.h>
#include <lib/assert.h>
#include <global/heap.h>
#include <global/modules.h>
#include <lib/modules/storage.h>


static inline void* ahci_map_physical(void* address, size_t size){
    return vmm_get_virtual_address(address);
}

#endif // _MODULE_AHCI_MAIN_H