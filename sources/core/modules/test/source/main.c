#include <lib/log.h>
#include <global/modules.h>

#define MODULE_NAME "test"

int init(int argc, char* args[]){
    log_printf("I am the %s module !!\n", MODULE_NAME);
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
