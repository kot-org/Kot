#include <lib/log.h>
#include <global/modules.h>

#include <arch/include.h>
#include ARCH_INCLUDE(acpi.h)

#include <deps.c>

#include "../odi-light/odi-ocf.c"
#include "../odi-light/examples/pci.c"

#define MODULE_NAME "odi"

int init(int argc, char* args[]){
    log_printf("[module/"MODULE_NAME"] loading start\n");

    log_printf("[module/"MODULE_NAME"] loading end\n");

    return 0;
}

int fini(void){
    return 0;
}

module_metadata_t module_metadata = {
    &init,
    &fini,
    MODULE_TYPE_UNDEFINE,
    MODULE_NAME
};
