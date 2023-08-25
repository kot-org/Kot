#ifndef _GLOBAL_FILE_H
#define _GLOBAL_FILE_H 1

#include <stdint.h>
#include <stddef.h>
#include <global/vfs.h>
#include <lib/modules/file.h>

int f_remove(vfs_ctx_t* ctx, const char* path);
kernel_file_t* f_open(vfs_ctx_t* ctx, const char* path, int flags, mode_t mode, int* error);
int f_read(void* buffer, size_t size, size_t* bytes_read, struct kernel_file_t* file);
int f_write(void* buffer, size_t size, size_t* bytes_write, struct kernel_file_t* file);
int f_close(struct kernel_file_t* file);

#endif // _GLOBAL_FILE_H