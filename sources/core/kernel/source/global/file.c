#include <global/file.h>
#include <global/modules.h>
#include <lib/modules/vfs.h>

int f_remove(vfs_ctx_t* ctx, const char* path){
    return vfs_handler->file_remove(ctx, path);
}

kernel_file_t* f_open(vfs_ctx_t* ctx, const char* path, int flags, mode_t mode, int* error){
    return vfs_handler->file_open(ctx, path, flags, mode, error);
}

int f_read(void* buffer, size_t size, size_t* bytes_read, struct kernel_file_t* file){
    return file->read(buffer, size, bytes_read, file);
}

int f_write(void* buffer, size_t size, size_t* bytes_write, struct kernel_file_t* file){
    return file->write(buffer, size, bytes_write, file);
}

int f_close(struct kernel_file_t* file){
    return file->close(file);
}