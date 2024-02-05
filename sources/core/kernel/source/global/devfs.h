#ifndef _GLOBAL_DEVFS_H
#define _GLOBAL_DEVFS_H 1

#include <stdint.h>
#include <stddef.h>
#include <lib/modules/devfs.h>

int devfs_add_dev(const char* path, file_open_fs_t open_handler);

#endif // _GLOBAL_DEVFS_H