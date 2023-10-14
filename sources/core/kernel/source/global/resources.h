#ifndef _GLOBAL_RESOURCES_H
#define _GLOBAL_RESOURCES_H 1

#include <lib/lock.h>
#include <global/dir.h>
#include <global/file.h>

#define MAX_DESCRIPTORS         256

#define DESCRIPTOR_TYPE_FILE    0
#define DESCRIPTOR_TYPE_DIR     1

typedef uint8_t descriptor_type_t;
typedef uint8_t descriptor_index_t;

typedef struct{
    descriptor_type_t type;
    union{
        kernel_file_t* file;
        kernel_dir_t* dir;
    }data;
}descriptor_t;

typedef struct{
    descriptor_t* descriptors[MAX_DESCRIPTORS];
    descriptor_index_t index_search_begin;
    spinlock_t lock;
}descriptors_ctx_t;

int add_descriptor(descriptors_ctx_t* ctx, descriptor_t* descriptor);
descriptor_t* get_descriptor(descriptors_ctx_t* ctx, int index);
int remove_descriptor(descriptors_ctx_t* ctx, int index);
int move_descriptor(descriptors_ctx_t* ctx, int old_index, int new_index);
int copy_process_descriptors(descriptors_ctx_t* dst, descriptors_ctx_t* src);

#endif // _GLOBAL_RESOURCES_H