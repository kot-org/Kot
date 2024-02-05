#include <lib/log.h>
#include <global/modules.h>

#define MODULE_NAME "ps2"

#include <core.c>
#include <mouse.c>
#include <keyboard.c>

int init(int argc, char* args[]){
    log_printf("[module/"MODULE_NAME"] loading start\n");

    assert(modules_request_dependency(MODULE_TYPE_HID) == 0);

    int error = ports_initalize();
    if(error) return error;

    disable_ports();
    
    error = keyboard_initialize();
    if(error) return error;

    error = mouse_initalize();
    if(error) return error;
    
    /* clear buffer */
    ps2_get_data();

    enable_ports();

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
