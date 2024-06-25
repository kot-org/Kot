#ifndef LIB_MODULES_FILE_H
#define LIB_MODULES_FILE_H 1

struct kernel_file_t;

#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <lib/modules/vfs.h>

typedef struct kernel_file_t{
    struct fs_t* fs_ctx;
    uint64_t seek_position;
    uint64_t file_size_initial;
    void* internal_data;
    int (*read)(void*, size_t, size_t*, struct kernel_file_t*);
    int (*write)(void*, size_t, size_t*, struct kernel_file_t*);
    int (*seek)(off_t, int, off_t*, struct kernel_file_t*);
    int (*ioctl)(uint32_t, void*, int*, struct kernel_file_t*);
    int (*stat)(int, struct stat*, struct kernel_file_t*);
    int (*close)(struct kernel_file_t*);
    int (*get_event)(struct kernel_file_t*, short, short*);
} kernel_file_t;

#endif // LIB_MODULES_FILE_H