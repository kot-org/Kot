#ifndef _MODULES_VFS_H
#define _MODULES_VFS_H 1

#include <lib/modules/file.h>

typedef struct{
    kernel_file_t* (*open)(const char*, int);
} fs_t;

typedef struct{
    kernel_file_t* (*open)(const char*, int);

    int (*add_fs)(const char* fs_mount_name, fs_t* fs);
} vfs_handler_t;

#endif // _MODULES_VFS_H