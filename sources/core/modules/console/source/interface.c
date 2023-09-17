#include <errno.h>
#include <lib/log.h>
#include <global/heap.h>
#include <global/devfs.h>

int console_interface_read(void* buffer, size_t size, size_t* bytes_read, struct kernel_file_t* file){
    *bytes_read = 0;
    return ENOSYS;
}

int console_interface_write(void* buffer, size_t size, size_t* bytes_write, kernel_file_t* file){
    log_printf("%.*s", size, buffer);
    *bytes_write = size;
    return 0;
}

int console_interface_seek(off_t offset, int whence, off_t* new_offset, kernel_file_t* file){
    return 0;
}

int console_interface_ioctl(uint32_t request, void* arg, int* result, kernel_file_t* file){
    return ENOSYS;
}

int console_interface_stat(int flags, struct stat* statbuf, kernel_file_t* file){
    return ENOSYS;
}

int console_interface_close(kernel_file_t* file){
    free(file);
    return 0;
}

kernel_file_t* console_interface_open(struct fs_t* ctx, const char* path, int flags, mode_t mode, int* error){
    kernel_file_t* file = malloc(sizeof(kernel_file_t));

    file->fs_ctx = ctx;
    file->seek_position = 0;
    file->file_size_initial = 0;
    file->internal_data = NULL;

    file->read = console_interface_read;
    file->write = console_interface_write;
    file->seek = console_interface_seek;
    file->ioctl = console_interface_ioctl;
    file->stat = console_interface_stat;
    file->close = console_interface_close;

    return file;
}

void interface_init(void){
    devfs_add_dev("tty0", &console_interface_open);
}