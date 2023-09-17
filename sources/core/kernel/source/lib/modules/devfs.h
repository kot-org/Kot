#ifndef LIB_MODULES_DEVFS_H
#define LIB_MODULES_DEVFS_H 1

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <lib/modules/file.h>

typedef struct{
    int (*add_dev)(const char* path, file_open_fs_t open_handler);
} devfs_handler_t; 

#endif // LIB_MODULES_DEVFS_H