#ifndef _MODULES_FILE_H
#define _MODULES_FILE_H 1

struct kernel_file_t;

#include <stddef.h>
#include <stdint.h>
#include <dirent.h>
#include <sys/types.h>
#include <lib/modules/vfs.h>

typedef struct kernel_file_t{
    struct fs_t* fs_ctx;
    uint64_t seek_position;
    uint64_t size;
    void* internal_data;
    int (*read)(void*, size_t, size_t*, struct kernel_file_t*);
    int (*write)(void*, size_t, size_t*, struct kernel_file_t*);
    int (*close)(struct kernel_file_t*);
} kernel_file_t;

#endif // _MODULES_FILE_H