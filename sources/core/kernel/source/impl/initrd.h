#ifndef _IMPL_INITRD_H
#define _IMPL_INITRD_H 1

#include <stddef.h>
#include <sys/types.h>
#include <global/modules.h>

void initrd_init(void);

void* initrd_get_file(const char* path);
void* initrd_get_file_base(void* file_ptr);
void initrd_read_file(void* file_ptr, void* base, size_t size);
ssize_t initrd_get_file_size(void* file_ptr);
kernel_file_t* initrd_open(fs_t* ctx, const char* path, int flags, mode_t mode, int* error);

#endif