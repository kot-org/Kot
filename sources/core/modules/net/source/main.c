#include <main.h>

#define MODULE_NAME "net"

#include <arp.c>
#include <ethernet.c>
#include <interface.c>

int init(int argc, char* args[]){
    log_printf("[module/"MODULE_NAME"] loading start\n");

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
    MODULE_TYPE_NET,
    MODULE_NAME
};
