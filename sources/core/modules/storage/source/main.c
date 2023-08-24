#include <main.h>
#include <interface.h>

#define MODULE_NAME "storage"

#include <interface.c>
#include <partition.c>
#include <partition/mbr.c>
#include <partition/gpt.c>
#include <partition/crc32.c>

int init(int argc, char* args[]){
    log_printf("[module/"MODULE_NAME"] loading start\n");

    init_partition();
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
    MODULE_TYPE_STORAGE,
    MODULE_NAME
};
