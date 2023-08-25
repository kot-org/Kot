#include <global/dir.h>
#include <global/modules.h>
#include <lib/modules/vfs.h>

int d_create(vfs_ctx_t* ctx, const char* path, mode_t mode){
    return vfs_handler->dir_create(ctx, path, mode);
}

int d_create_at(vfs_ctx_t* ctx, struct kernel_dir_t* dir, const char* path, mode_t mode){
    return vfs_handler->dir_create_at(ctx, dir, path, mode);
}

int d_remove(vfs_ctx_t* ctx, const char* path){
    return vfs_handler->dir_remove(ctx, path);
}

kernel_dir_t* d_open(vfs_ctx_t* ctx, const char* path, int* error){
    return vfs_handler->dir_open(ctx, path, error);
}

int d_get_entries(void* buffer, size_t max_size, size_t* size_read, struct kernel_dir_t* dir){
    return dir->get_directory_entries(buffer, max_size, size_read, dir);
}