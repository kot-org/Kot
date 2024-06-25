#ifndef _MODULE_DEVFS_CORE_H
#define _MODULE_DEVFS_CORE_H

#include <poll.h>
#include <errno.h>
#include <lib/math.h>
#include <lib/memory.h>
#include <lib/string.h>
#include <lib/vector.h>
#include <global/vfs.h>
#include <global/heap.h>
#include <lib/modules/devfs.h>

struct devfs_context_t;

typedef struct{
    size_t size;
    file_open_fs_t open_handler;
} devfs_file_internal_t;

typedef struct{
    vector_t* entries;
} devfs_directory_internal_t;

typedef struct devfs_directory_entry_t{
    char* name;
    bool is_file;
    uint64_t index;
    struct devfs_context_t* ctx;
    struct devfs_directory_entry_t* parent;
    union {
        devfs_file_internal_t* file;
        devfs_directory_internal_t* directory;
    } data;
} devfs_directory_entry_t;

typedef struct devfs_context_t{
    devfs_directory_entry_t* root_entry;
} devfs_context_t;

#endif  // _MODULE_DEVFS_CORE_H