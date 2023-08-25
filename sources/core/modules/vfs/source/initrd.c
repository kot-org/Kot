#include <core.h>
#include <impl/initrd.h>

void init_vfs_initrd(void){
    fs_t* fs_vfs = malloc(sizeof(fs_t));
    fs_vfs->file_remove = (file_remove_fs_t)&vfs_return_not_implemented;
    fs_vfs->file_open = &initrd_open;
    fs_vfs->dir_create = (dir_create_fs_t)&vfs_return_not_implemented;
    fs_vfs->dir_create_at = (dir_create_at_fs_t)&vfs_return_not_implemented;
    fs_vfs->dir_remove = (dir_remove_fs_t)&vfs_return_not_implemented;
    fs_vfs->dir_open = &vfs_dir_open;
    fs_vfs->rename = (rename_fs_t)&vfs_return_not_implemented;
    fs_vfs->link = (link_fs_t)&vfs_return_not_implemented;
    fs_vfs->unlink_at = (unlink_at_fs_t)&vfs_return_not_implemented;
    mount_fs("/initrd", fs_vfs);
}