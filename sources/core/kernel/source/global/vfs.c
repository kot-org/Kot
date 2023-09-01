#include <lib/string.h>
#include <global/vfs.h>
#include <global/heap.h>
#include <global/modules.h>
#include <lib/modules/vfs.h>

vfs_ctx_t* kernel_vfs_ctx = NULL;

void vfs_init(void){
    kernel_vfs_ctx = malloc(sizeof(vfs_ctx_t));
    kernel_vfs_ctx->cwd_size = sizeof((char)'\0');
    kernel_vfs_ctx->cwd = malloc(kernel_vfs_ctx->cwd_size);
    kernel_vfs_ctx->cwd[0] = '\0';
}

int vfs_rename(vfs_ctx_t* ctx, const char* old_path, const char* new_path){
    return vfs_handler->rename(ctx, old_path, new_path);
}

int vfs_link(vfs_ctx_t* ctx, const char* src_path, const char* dst_path){
    return vfs_handler->link(ctx, src_path, dst_path);
}

int vfs_mount_fs(const char* fs_mount_name, fs_t* fs){
    return vfs_handler->mount_fs(fs_mount_name, fs);
}

int vfs_unmount_fs(const char* fs_mount_name){
    return vfs_handler->unmount_fs(fs_mount_name);
}

char* vfs_request_friendly_fs_mount_name(bool is_removable){
    return vfs_handler->request_friendly_fs_mount_name(is_removable);
}

int vfs_free_friendly_fs_mount_name(const char* fs_mount_name){
    return vfs_handler->free_friendly_fs_mount_name(fs_mount_name);
}

vfs_ctx_t* vfs_create_ctx(const char* cwd){
    vfs_ctx_t* ctx = malloc(sizeof(vfs_ctx_t));

    ctx->cwd_size = strlen(cwd);
    ctx->cwd = malloc(ctx->cwd_size + 1);
    strncpy(ctx->cwd, cwd, ctx->cwd_size);

    return ctx;
}

vfs_ctx_t* vfs_copy_ctx(vfs_ctx_t* src){
    vfs_ctx_t* dst = malloc(sizeof(vfs_ctx_t));

    dst->cwd_size = src->cwd_size;
    dst->cwd = malloc(dst->cwd_size + 1);
    strncpy(dst->cwd, src->cwd, dst->cwd_size);

    return dst;
}

int vfs_free_ctx(vfs_ctx_t* ctx){
    free(ctx->cwd);
    free(ctx);
    
    return 0;
}