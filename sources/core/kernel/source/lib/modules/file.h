#ifndef _MODULES_FILE_H
#define _MODULES_FILE_H 1

#include <stddef.h>
#include <stdint.h>
#include <dirent.h>
#include <sys/types.h>
#include <lib/modules/vfs.h>

typedef struct kernel_file_t{
    struct fs_t* fs;
    size_t seek_position;
    size_t size;
    void* internal_data;
    size_t (*read)(void*, size_t, struct kernel_file_t*);
    size_t (*write)(void*, size_t, struct kernel_file_t*);
    int (*close)(struct kernel_file_t*);
} kernel_file_t;

typedef struct kernel_dir_t{
    struct fs_t* fs;
} kernel_dir_t;

typedef struct dirent_t{
    ino_t d_ino;
    off_t d_off;
    uint16_t d_reclen;
    char d_type;
    char d_name[1024];
} dirent_t;

#endif // _MODULES_FILE_H