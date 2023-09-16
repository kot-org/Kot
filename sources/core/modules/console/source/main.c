#include <lib/log.h>
#include <lib/assert.h>
#include <global/modules.h>

#define MODULE_NAME "console"

#include <interface.c>

int init(int argc, char* args[]){
    log_printf("[module/"MODULE_NAME"] loading start\n");

    assert(modules_request_dependency(MODULE_TYPE_DEVFS) == 0);

    interface_init();

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
