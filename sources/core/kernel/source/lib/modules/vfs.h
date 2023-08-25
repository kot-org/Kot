#ifndef LIB_MODULES_VFS_H
#define LIB_MODULES_VFS_H 1

#include <stdbool.h>
#include <lib/modules/dir.h>
#include <lib/modules/file.h>

struct fs_t;

typedef int (*file_remove_fs_t)(struct fs_t*, const char*);
typedef struct kernel_file_t* (*file_open_fs_t)(struct fs_t*, const char*, int, mode_t, int*);

typedef int (*dir_create_fs_t)(struct fs_t*, const char*, mode_t);
typedef int (*dir_create_at_fs_t)(struct fs_t*, struct kernel_dir_t*, const char*, mode_t);
typedef int (*dir_remove_fs_t)(struct fs_t*, const char*);
typedef struct kernel_dir_t* (*dir_open_fs_t)(struct fs_t*, const char*, int*);

typedef int (*rename_fs_t)(struct fs_t*, const char*, const char*);
typedef int (*link_fs_t)(struct fs_t*, const char*, const char*);
typedef int (*unlink_at_fs_t)(struct fs_t*, struct kernel_dir_t*, const char*, mode_t);

typedef struct fs_t{
    void* internal_data;

    file_remove_fs_t file_remove;
    file_open_fs_t file_open;
    dir_create_fs_t dir_create;
    dir_create_at_fs_t dir_create_at;
    dir_remove_fs_t dir_remove;
    dir_open_fs_t dir_open;
    rename_fs_t rename;
    link_fs_t link;
    unlink_at_fs_t unlink_at;
} fs_t;

typedef struct{
    /*  the cwd musn't contain the first / to make the path absolute because 
        it should be absolute in every case we will have current/working/directory 
        instead of /current/working/directory instead but the vfs will understand 
        it as /current/working/directory
    */
    char* cwd;
    size_t cwd_size;
} vfs_ctx_t;

typedef int (*file_remove_vfs_t)(vfs_ctx_t*, const char*);
typedef struct kernel_file_t* (*file_open_vfs_t)(vfs_ctx_t*, const char*, int, mode_t, int*);

typedef int (*dir_create_vfs_t)(vfs_ctx_t*, const char*, mode_t);
typedef int (*dir_create_at_vfs_t)(vfs_ctx_t*, struct kernel_dir_t*, const char*, mode_t);
typedef int (*dir_remove_vfs_t)(vfs_ctx_t*, const char*);
typedef struct kernel_dir_t* (*dir_open_vfs_t)(vfs_ctx_t*, const char*, int*);

typedef int (*rename_vfs_t)(vfs_ctx_t*, const char*, const char*);
typedef int (*link_vfs_t)(vfs_ctx_t*, const char*, const char*);
typedef int (*unlink_at_vfs_t)(vfs_ctx_t*, struct kernel_dir_t*, const char*, mode_t);


typedef struct{
    file_remove_vfs_t file_remove;
    file_open_vfs_t file_open;
    dir_create_vfs_t dir_create;
    dir_create_at_vfs_t dir_create_at;
    dir_remove_vfs_t dir_remove;
    dir_open_vfs_t dir_open;
    rename_vfs_t rename;
    link_vfs_t link;
    unlink_at_vfs_t unlink_at;

    int (*mount_fs)(const char* fs_mount_name, fs_t* fs);
    int (*unmount_fs)(const char* fs_mount_name);

    char* (*request_friendly_fs_mount_name)(bool is_removable);
    int (*free_friendly_fs_mount_name)(const char* fs_mount_name);
} vfs_handler_t;

#endif // LIB_MODULES_VFS_H