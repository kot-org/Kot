#ifndef LIB_MODULES_DEVFS_H
#define LIB_MODULES_DEVFS_H 1

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <lib/modules/file.h>

typedef struct{
    int (*read)(void*, size_t, size_t*, struct kernel_file_t*);
    int (*write)(void*, size_t, size_t*, struct kernel_file_t*);
    int (*seek)(off_t, int, off_t*, struct kernel_file_t*);
    int (*ioctl)(uint32_t, void*, int*, struct kernel_file_t*);
    int (*stat)(int, struct stat*, struct kernel_file_t*);
    int (*close)(struct kernel_file_t*);
} devfs_functions_t;

typedef struct{
    int (*add_dev)(const char* path, devfs_functions_t* functions);
} devfs_handler_t; 

#endif // LIB_MODULES_DEVFS_H