#include <boot/limine.h>

#include <errno.h>
#include <stddef.h>
#include <lib/log.h>
#include <lib/math.h>
#include <sys/types.h>
#include <lib/string.h>
#include <lib/memory.h>
#include <global/heap.h>
#include <global/modules.h>

static volatile struct limine_module_request module_request = {
    .id = LIMINE_MODULE_REQUEST,
    .revision = 0
};

static struct limine_file* initrd_get_file_ptr(const char* path){
    if(module_request.response){
        for(uint64_t i = 0; i < module_request.response->module_count; i++){
            if(!strcmp(module_request.response->modules[i]->path, path)){
                return module_request.response->modules[i];
            }
        }
    }

    return NULL;
}

static vfs_handler_t early_vfs_handler;

void* initrd_get_file(const char* path){
    return (void*)initrd_get_file_ptr(path);
}

void* initrd_get_file_base(void* file_ptr){
    if(file_ptr){
        struct limine_file* file = (struct limine_file*)file_ptr;
        return file->address;
    }

    return NULL;
}

int initrd_read_file(void* file_ptr, void* base, size_t size){
    if(file_ptr){
        struct limine_file* file = (struct limine_file*)file_ptr;
        size_t size_to_copy = size;
        if(size_to_copy > file->size){
            size_to_copy = file->size;
        }
        memcpy(base, file->address, size_to_copy);
        return 0;
    }

    return -EINVAL;
}

ssize_t initrd_get_file_size(void* file_ptr){
    if(file_ptr){
        struct limine_file* file = (struct limine_file*)file_ptr;
        return file->size;
    }

    return -EINVAL;
}

int initrd_read(void* buffer, size_t size, size_t* bytes_read, kernel_file_t* file){
    if(file == NULL){
        return 0;
    }

    size_t file_size = initrd_get_file_size(file->internal_data);
    if(file_size < file->seek_position + size){
        size = file_size - file->seek_position;
    }

    memcpy(buffer, initrd_get_file_base(file->internal_data) + file->seek_position, size);

    *bytes_read = size;

    return 0;
}

int initrd_write(void* buffer, size_t size, size_t* bytes_write, kernel_file_t* file){
    *bytes_write = 0;
    return ENOSYS;
}

int initrd_seek(off_t offset, int whence, off_t* new_offset, kernel_file_t* file){
    switch(whence){
        case SEEK_SET:{
            file->seek_position = offset;
            *new_offset = file->seek_position;
            return 0;
        }
        case SEEK_CUR:{
            file->seek_position += offset;
            *new_offset = file->seek_position;
            return 0;
        }
        case SEEK_END:{
            file->seek_position = initrd_get_file_size(file->internal_data);
            *new_offset = file->seek_position;
            return 0;
        }
    }
    *new_offset = 0;
    return EINVAL;
}

int initrd_ioctl(uint32_t request, void* arg, int* result, kernel_file_t* file){
    return ENOTTY;
}

int initrd_stat(int flags, struct stat* statbuf, kernel_file_t* file){
    memset(statbuf, 0, sizeof(struct stat));
    statbuf->st_size = initrd_get_file_size(file->internal_data);
    statbuf->st_blocks = DIV_ROUNDUP(statbuf->st_size, 512);
    statbuf->st_blksize = 512;
    return 0;
}

int initrd_close(kernel_file_t* file){
    return 0;
}

kernel_file_t* initrd_open(fs_t* ctx, const char* path, int flags, mode_t mode, int* error){
    void* file_ptr = initrd_get_file(path);
    if(file_ptr != NULL){
        kernel_file_t* file = (kernel_file_t*)malloc(sizeof(kernel_file_t));
        file->file_size_initial = initrd_get_file_size(file_ptr);
        file->seek_position = 0;
        file->internal_data = file_ptr;
        file->read = &initrd_read;
        file->write = &initrd_write;
        file->seek = &initrd_seek;
        file->ioctl = &initrd_ioctl;
        file->stat = &initrd_stat;
        file->close = &initrd_close;
        return file;
    }else{
        return NULL;
    }
}

kernel_file_t* early_vfs_initrd_open(vfs_ctx_t* ctx, const char* path, int flags, mode_t mode, int* error){
    return initrd_open(NULL, path + sizeof("/initrd") - 1, flags, mode, error);
}

void initrd_init(void) {
    early_vfs_handler.file_open = &early_vfs_initrd_open;
    vfs_handler = &early_vfs_handler;
}