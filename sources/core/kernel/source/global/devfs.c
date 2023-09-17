#include <global/file.h>
#include <global/modules.h>
#include <lib/modules/devfs.h>

int devfs_add_dev(const char* path, file_open_fs_t open_handler){
    return devfs_handler->add_dev(path, open_handler);
}
