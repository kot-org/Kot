#ifndef _GLOBAL_VFS_H
#define _GLOBAL_VFS_H 1

#include <stdint.h>
#include <stddef.h>

#include <lib/modules/file.h>

#define KERNEL_VFS_CTX NULL

int vfs_rename(vfs_ctx_t* ctx, const char* old_path, const char* new_path);
int vfs_link(vfs_ctx_t* ctx, const char* src_path, const char* dst_path);
int vfs_unlink_at(vfs_ctx_t* ctx, struct kernel_dir_t* dir, const char* path, mode_t mode);
int vfs_mount_fs(const char* fs_mount_name, fs_t* fs);
int vfs_unmount_fs(const char* fs_mount_name);
char* vfs_request_friendly_fs_mount_name(bool is_removable);
int vfs_free_friendly_fs_mount_name(const char* fs_mount_name);

vfs_ctx_t* vfs_create_ctx(const char* cwd);
vfs_ctx_t* vfs_copy_ctx(vfs_ctx_t* ctx);
int vfs_free_ctx(vfs_ctx_t* ctx);

#endif // _GLOBAL_VFS_H