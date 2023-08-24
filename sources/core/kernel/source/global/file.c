#include <global/file.h>
#include <global/modules.h>
#include <lib/modules/vfs.h>

kernel_file_t* open(const char* path, int flags){
    return vfs_handler->open(path, flags);
}