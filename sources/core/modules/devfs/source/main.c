#include <lib/log.h>
#include <lib/assert.h>
#include <global/modules.h>

#define MODULE_NAME "devfs"

static vfs_handler_t vfs_handler_buffer;

int init(int argc, char* args[]){
    log_printf("[module/"MODULE_NAME"] loading start\n");

    assert(modules_request_dependency(MODULE_TYPE_VFS) == 0);

    log_printf("[module/"MODULE_NAME"] loading end\n");  
    return 0;
}

int fini(void){
    return 0;
}

module_metadata_t module_metadata = {
    &init,
    &fini,
    MODULE_TYPE_VFS,
    MODULE_NAME
};
