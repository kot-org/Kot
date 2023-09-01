#include <lib/log.h>
#include <global/modules.h>

#define MODULE_NAME "vfs"

#include <core.c>
#include <initrd.c>
#include <system.c>

static vfs_handler_t vfs_handler_buffer;

int init(int argc, char* args[]){
    log_printf("[module/"MODULE_NAME"] loading start\n");

    vfs_init();

    vfs_initrd_init();

    system_tasks_init();
    
    vfs_handler_buffer.file_remove = &file_remove;
    vfs_handler_buffer.file_open = &file_open;
    vfs_handler_buffer.dir_create = &dir_create;
    vfs_handler_buffer.dir_remove = &dir_remove;
    vfs_handler_buffer.dir_open = &dir_open;
    vfs_handler_buffer.rename = &vfs_rename;
    vfs_handler_buffer.link = &vfs_link;

    vfs_handler_buffer.mount_fs = &mount_fs;
    vfs_handler_buffer.unmount_fs = &unmount_fs;

    vfs_handler_buffer.request_friendly_fs_mount_name = &request_friendly_fs_mount_name;
    vfs_handler_buffer.free_friendly_fs_mount_name = &free_friendly_fs_mount_name;

    vfs_handler = &vfs_handler_buffer;


    log_printf("[module/"MODULE_NAME"] loading end\n");  
    return 0;
}

int fini(void){
    return 0;
}

module_metadata_t module_metadata = {
    &init,
    &fini,
    MODULE_TYPE_VFS,
    MODULE_NAME
};
