#ifndef _MODULES_VFS_H
#define _MODULES_VFS_H 1

#include <lib/modules/file.h>

typedef struct fs_t{
    int (*file_remove)(const char*);
    kernel_file_t* (*file_open)(const char*, int, mode_t, int*);
    
    int (*dir_create)(const char*, mode_t);
    int (*dir_create_at)(kernel_dir_t*, const char*, mode_t);
    int (*dir_remove)(const char*);
    kernel_dir_t* (*dir_open)(const char*, int*);

    int (*rename)(const char*, const char*);
    int (*link)(const char*, const char*);
    int (*unlink_at)(kernel_dir_t*, const char*, mode_t);
} fs_t;

typedef struct{
    fs_t* fs;

    int (*mount_fs)(const char* fs_mount_name, fs_t* fs);
    int (*unmount_fs)(const char* fs_mount_name);
} vfs_handler_t;

#endif // _MODULES_VFS_H