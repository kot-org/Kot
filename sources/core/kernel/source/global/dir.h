#ifndef _GLOBAL_DIR_H
#define _GLOBAL_DIR_H 1

#include <stdint.h>
#include <stddef.h>
#include <global/vfs.h>
#include <lib/modules/dir.h>

int d_create(vfs_ctx_t* ctx, const char* path, mode_t mode);
int d_remove(vfs_ctx_t* ctx, const char* path);
kernel_dir_t* d_open(vfs_ctx_t* ctx, const char* path, int* error);
int d_get_entries(void* buffer, size_t max_size, size_t* size_read, struct kernel_dir_t* dir);
int d_create_at(struct kernel_dir_t* dir, const char* path, mode_t mode);
int d_unlink_at(struct kernel_dir_t* dir, const char* path, int flags);
int d_close(struct kernel_dir_t* dir);

#endif // _GLOBAL_DIR_H