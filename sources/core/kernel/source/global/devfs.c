#include <global/file.h>
#include <global/modules.h>
#include <lib/modules/devfs.h>

int devfs_add_dev(const char* path, devfs_functions_t* functions){
    return devfs_handler->add_dev(path, functions);
}
