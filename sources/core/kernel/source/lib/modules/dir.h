#ifndef LIB_MODULES_DIR_H
#define LIB_MODULES_DIR_H 1

struct kernel_dir_t;

#include <stddef.h>
#include <stdint.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <lib/modules/vfs.h>

typedef struct kernel_dir_t{
    struct fs_t* fs_ctx;
    uint64_t seek_position;
    void* internal_data;
    int (*get_directory_entries)(void*, size_t, size_t*, struct kernel_dir_t*);
    int (*create_at)(struct kernel_dir_t* dir, const char* path, mode_t mode);
    int (*unlink_at)(struct kernel_dir_t* dir, const char* path, int flags);
    int (*stat)(int, struct stat*, struct kernel_dir_t*);
    int (*close)(struct kernel_dir_t*);
} kernel_dir_t;

typedef struct dirent_t{
    ino_t d_ino;
    off_t d_off;
    uint16_t d_reclen;
    char d_type;
    char d_name[1024];
} dirent_t;

#endif // LIB_MODULES_DIR_H