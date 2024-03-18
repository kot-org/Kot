#ifndef _MODULE_VFS_CORE_H
#define _MODULE_VFS_CORE_H

#include <errno.h>
#include <lib/assert.h>
#include <system.h>
#include <limits.h>
#include <lib/math.h>
#include <lib/lock.h>
#include <global/vfs.h>
#include <lib/memory.h>
#include <lib/string.h>
#include <impl/panic.h>
#include <global/heap.h>
#include <lib/hashmap.h>
#include <lib/modules/vfs.h>

#define VFS_MAX_PATH_SIZE PATH_MAX

int vfs_return_not_implemented(void);
kernel_file_t* vfs_file_open_not_implemented(fs_t* ctx, const char* path, int flags, mode_t mode, int* error);
kernel_dir_t* vfs_dir_open_not_implemented(fs_t* ctx, const char* path, int* error);

void vfs_init(void);
char* request_friendly_fs_mount_name(bool is_removable);
int free_friendly_fs_mount_name(const char* fs_mount_name);
int local_mount_fs(const char* fs_mount_name, fs_t* new_fs);
int mount_fs(const char* fs_mount_name, fs_t* new_fs);
int unmount_fs(const char* fs_mount_name);
int file_remove(vfs_ctx_t* ctx, const char* path);
kernel_file_t* file_open(vfs_ctx_t* ctx, const char* path, int flags, mode_t mode, int* error);
int dir_create(vfs_ctx_t* ctx, const char* path, mode_t mode);
int dir_remove(vfs_ctx_t* ctx, const char* path);
kernel_dir_t* dir_open(vfs_ctx_t* ctx, const char* path, int* error);
int vfs_rename(vfs_ctx_t* ctx, const char* old_path, const char* new_path);
int vfs_link(vfs_ctx_t* ctx, const char* src_path, const char* dst_path);

#endif // _MODULE_VFS_CORE_H
