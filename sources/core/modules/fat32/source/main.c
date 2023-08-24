#include <lib/log.h>
#include <lib/assert.h>
#include <global/heap.h>
#include <global/modules.h>

#define MODULE_NAME "fat32"

#include <core.c>
#include <interface.c>

int init(int argc, char* args[]){
    log_printf("[module/"MODULE_NAME"] loading start\n");

    assert(modules_request_dependency(MODULE_TYPE_STORAGE) == 0);

    init_interface();

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
