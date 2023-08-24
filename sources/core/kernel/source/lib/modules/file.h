#ifndef _MODULES_FILE_H
#define _MODULES_FILE_H 1

#include <stddef.h>
#include <stdint.h>


typedef struct kernel_file_t{
    size_t seek_position;
    size_t size;
    void* internal_data;
    size_t (*read)(void*, size_t, struct kernel_file_t*);
    size_t (*write)(void*, size_t, struct kernel_file_t*);
    int (*close)(struct kernel_file_t*);
} kernel_file_t;

#endif // _MODULES_FILE_H